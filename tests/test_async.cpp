#include <gtest/gtest.h>
#include <fstream>
#include <regex>
#include <filesystem>
#include "stdoutput.h"
#include "async.h"
using namespace std;

TEST(Async, StandardOutput)
{
	deque<shared_ptr<Command>> cmds;
	auto cmd1 = make_unique<Command>();
	cmd1->m_name = "cmd1";
	cmds.push_back(std::move(cmd1));

	testing::internal::CaptureStdout();

	{
		StdOutput output;
		output.writeBlock(std::move(cmds));
	}

	string outStr = testing::internal::GetCapturedStdout();
	EXPECT_EQ("bulk: cmd1\n", outStr);
}

shared_ptr<Command> createCmd(const string& from)
{
	auto cmd = make_unique<Command>();
	cmd->m_name = from;
	return cmd;
}

void deleteAllLogs()
{
	namespace fs = std::filesystem;
	fs::path dir = fs::current_path();
	regex regExpName{ "(.*\\.log)" };
	for (const auto& element : fs::directory_iterator(dir))
		if (! element.is_directory() && regex_match(element.path().filename().string(), regExpName))
			fs::remove(element.path());
}

string readFile(const string& fname)
{
	string res;
	ifstream file{ fname };
	string line;
	while (getline(file, line))
		res += line;
	file.close();
	return res;
}

string readAllLogs()
{
	string res;
	namespace fs = std::filesystem;
	fs::path dir = fs::current_path();
	regex regExpName{ "(.*\\.log)" };
	for (const auto& element : fs::directory_iterator(dir))
		if (!element.is_directory() && regex_match(element.path().filename().string(), regExpName))
			res += readFile(element.path().string());
	return res;
}

TEST(Async, LimitedStateOnly)
{
	deleteAllLogs();

	async::handle_t h = async::connect(3);

	testing::internal::CaptureStdout();

	vector<string> input{ "cmd1", "cmd2", "cmd3", "cmd4", "cmd5" };
	for (auto& str : input)
	{
		string tmp = str + '\n';
		async::receive(h, tmp.c_str(), tmp.size());
	}
	async::disconnect(h);

	string outStr = testing::internal::GetCapturedStdout();
	EXPECT_EQ("bulk: cmd1, cmd2, cmd3\nbulk: cmd4, cmd5\n", outStr);

	string textFromLogFiles = readAllLogs();
	for (auto& s : input)
		EXPECT_TRUE(textFromLogFiles.find(s) != string::npos);
}

TEST(Async, MixedStates)
{
	deleteAllLogs();

	async::handle_t h = async::connect(3);

	testing::internal::CaptureStdout();

	vector<string> input{ "cmd1", "cmd2", "{", "cmd3", "cmd4", "}", "{", "cmd5", "cmd6",
		"{", "cmd7", "cmd8", "}", "cmd9", "}", "{", "cmd10", "cmd11" }; // NOTE remainder (cmd10, cmd11) is not going to output
	for (auto& str : input)
	{
		string tmp = str + "\n";
		async::receive(h, tmp.c_str(), tmp.size());
	}
	async::disconnect(h);

	string outStr = testing::internal::GetCapturedStdout();
	EXPECT_EQ("bulk: cmd1, cmd2\nbulk: cmd3, cmd4\nbulk: cmd5, cmd6, cmd7, cmd8, cmd9\n", outStr);

	string textFromLogFiles = readAllLogs();
	for (auto& s : input)
	{
		if (s != "{" && s != "}" && s != "cmd10" && s != "cmd11" )
			EXPECT_TRUE(textFromLogFiles.find(s) != string::npos);
	}
}

TEST(Async, TwoHandles)
{
	deleteAllLogs();

	async::handle_t first = async::connect(3);
	async::handle_t second = async::connect(3);

	testing::internal::CaptureStdout();

	vector<string> input1{ "1cmd1", "1cmd2", "{", "1cmd3", "1cmd4", "}", "{", "1cmd5", "1cmd6",
		"{", "1cmd7", "1cmd8", "}", "1cmd9", "}", "{", "1cmd10", "1cmd11" }; // NOTE remainder (1cmd10, 1cmd11) is not going to output
	for (auto& str : input1)
	{
		string tmp = str + "\n";
		async::receive(first, tmp.c_str(), tmp.size());
	}

	vector<string> input2{ "2cmd1", "2cmd2", "{", "2cmd3", "2cmd4", "}", "{", "2cmd5", "2cmd6",
		"{", "2cmd7", "2cmd8", "}", "2cmd9", "}", "{", "2cmd10", "2cmd11" }; // NOTE remainder (2cmd10, 2cmd11) is not going to output
	for (auto& str : input2)
	{
		string tmp = str + "\n";
		async::receive(second, tmp.c_str(), tmp.size());
	}

	async::disconnect(first);
	async::disconnect(second);

	string outStr = testing::internal::GetCapturedStdout();
	for (auto& s : input1)
	{
		if (s != "{" && s != "}" && s != "1cmd10" && s != "1cmd11" && s != "2cmd10" && s != "2cmd11")
			EXPECT_TRUE(outStr.find(s) != string::npos);
	}
	for (auto& s : input2)
	{
		if (s != "{" && s != "}" && s != "1cmd10" && s != "1cmd11" && s != "2cmd10" && s != "2cmd11")
			EXPECT_TRUE(outStr.find(s) != string::npos);
	}

	string textFromLogFiles = readAllLogs();
	for (auto& s : input1)
	{
		if (s != "{" && s != "}" && s != "1cmd10" && s != "1cmd11" && s != "2cmd10" && s != "2cmd11")
			EXPECT_TRUE(textFromLogFiles.find(s) != string::npos);
	}
	for (auto& s : input2)
	{
		if (s != "{" && s != "}" && s != "1cmd10" && s != "1cmd11" && s != "2cmd10" && s != "2cmd11")
			EXPECT_TRUE(textFromLogFiles.find(s) != string::npos);
	}
}

TEST(Async, LimitedStateOneLine)
{
	deleteAllLogs();

	async::handle_t h = async::connect(3);

	testing::internal::CaptureStdout();

	string input{ "cmd1\ncmd2\ncmd3\ncmd4\ncmd5\n" };
	async::receive(h, input.c_str(), input.size());
	async::disconnect(h);

	string outStr = testing::internal::GetCapturedStdout();
	EXPECT_EQ("bulk: cmd1, cmd2, cmd3\nbulk: cmd4, cmd5\n", outStr);

	vector<string> expected = { "cmd1", "cmd2", "cmd3", "cmd4", "cmd5" };
	string textFromLogFiles = readAllLogs();
	for (auto& s : expected)
		EXPECT_TRUE(textFromLogFiles.find(s) != string::npos);
}

TEST(Async, NoTrailingDelimiter)
{
	deleteAllLogs();

	async::handle_t h = async::connect(3);

	testing::internal::CaptureStdout();

	string input = "cmd1\ncmd2";
	async::receive(h, input.c_str(), input.size());
	input = "\ncmd3\n";
	async::receive(h, input.c_str(), input.size());
	async::disconnect(h);

	string outStr = testing::internal::GetCapturedStdout();
	EXPECT_EQ("bulk: cmd1, cmd2, cmd3\n", outStr);

	vector<string> expected = { "cmd1", "cmd2", "cmd3" };
	string textFromLogFiles = readAllLogs();
	for (auto& s : expected)
		EXPECT_TRUE(textFromLogFiles.find(s) != string::npos);
}