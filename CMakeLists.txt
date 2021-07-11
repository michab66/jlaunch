cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)

# Use -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON for command debugging.
# Use -DCMAKE_BUILD_TYPE=Release

# Set the variables before the project definition to get their
# default values int the cmake cache.
#set( CMAKE_BUILD_TYPE "Release" CACHE STRING "Default.")
set( CMAKE_INSTALL_PREFIX "./dist" CACHE STRING "Default.")

project(jlaunch VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
#FetchContent_Declare(
#  googletest
#  GIT_REPOSITORY https://github.com/google/googletest.git
#  GIT_TAG        release-1.10.0
#)

FetchContent_Declare(
  smack_cpp
  GIT_REPOSITORY https://github.com/smacklib/dev_smack_cpp.git
  GIT_TAG        v17.3
)

# After the following call, the CMake targets defined by googletest
# will be defined and available to the rest of the build
FetchContent_MakeAvailable(smack_cpp)

enable_testing()

#
# Common for all architectures..
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_subdirectory(jlgen)
add_subdirectory(jlaunch)

add_dependencies(jlgen jlaunch)
