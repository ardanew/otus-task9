set(TARGET_NAME async)

set(ASYNC_LIB_SOURCES
    async.cpp
    command.h
    iinput.h input.h input.cpp
    icmdprocessor.h cmdprocessor.h cmdprocessor.cpp
    basestate.h basestate.cpp 
    limitedstate.h limitedstate.cpp
    unlimitedstate.h unlimitedstate.cpp
    ioutput.h stdoutput.h stdoutput.cpp logoutput.h logoutput.cpp
    CACHE FILEPATH "" # will be used in tests
)
add_library(${TARGET_NAME} SHARED ${ASYNC_LIB_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(${TARGET_NAME} PRIVATE BUILDING_LIBRARY)

target_sources(${TARGET_NAME} PUBLIC
    FILE_SET interfaces TYPE HEADERS BASE_DIRS . FILES
    async.h
)
include(GNUInstallDirs)
install(
    TARGETS ${TARGET_NAME} COMPONENT app
    EXPORT ${TARGET_NAME}
    PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET interfaces DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${TARGET_NAME}
    NAMESPACE async::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
    FILE ${TARGET_NAME}Config.cmake
)