cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

set(headers
    jlaunch_resource_ids.h
)
set(implementation
    jlaunch.cpp
)

add_executable(jlaunch
    ${implementation}
    ${headers}
)

#target_link_libraries(jlaunch
#    smack_cpp
#)

message( MichaelBinz )

file(TO_CMAKE_PATH $ENV{JAVA_HOME} NORMALIZED_JAVA_HOME)
message( "${NORMALIZED_JAVA_HOME}/include" )

target_include_directories(jlaunch
    PRIVATE
	"${NORMALIZED_JAVA_HOME}/include"
	"${NORMALIZED_JAVA_HOME}/include/win32"
)
