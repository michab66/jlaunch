cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

set(headers
    jlaunch_resource_ids.h
)
set(implementation
    jlaunch.cpp
)

add_executable(jlaunch WIN32
    ${implementation}
    ${headers}
)

target_link_libraries(jlaunch
    smack_cpp
    jvm.lib
)

file(TO_CMAKE_PATH $ENV{JAVA_HOME} NORMALIZED_JAVA_HOME)

target_link_directories(jlaunch
    PRIVATE
    "${NORMALIZED_JAVA_HOME}/lib"
)

target_include_directories(jlaunch
    PRIVATE
	"${NORMALIZED_JAVA_HOME}/include"
	"${NORMALIZED_JAVA_HOME}/include/win32"
)

add_custom_command(
    TARGET jlaunch 
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy  
                     "$<TARGET_FILE:jlaunch>"
                     "${PROJECT_BINARY_DIR}"
)
