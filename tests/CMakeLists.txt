set(TARGET_NAME async_tests)
add_executable(${TARGET_NAME})

set(LIB_SOURCES ${ASYNC_LIB_SOURCES})
list(TRANSFORM LIB_SOURCES PREPEND ../src/async_lib/)
target_sources(${TARGET_NAME} PRIVATE
    test_async.cpp
    ${LIB_SOURCES}
)
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/async_lib)

find_package(GTest REQUIRED)

target_compile_definitions(${TARGET_NAME} PRIVATE VIRTUAL_TESTS UNIT_TESTS)

target_link_libraries(${TARGET_NAME} 
    PRIVATE 
        GTest::gtest_main
)

if(MSVC) # for vs debugging
    set_property(TARGET ${TARGET_NAME} PROPERTY
        VS_DEBUGGER_WORKING_DIRECTORY $(OutDir)
    )
endif()

add_test(AsyncTest "${CMAKE_INSTALL_PREFIX}/tests/async-tests")

include(GNUInstallDirs)
install(
    TARGETS ${TARGET_NAME} COMPONENT tests
    PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    RUNTIME DESTINATION tests
)
