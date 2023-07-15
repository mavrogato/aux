
cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_C_STANDARD "17")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Wno-unknown-cuda-version")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR})

project(aux)

include(FetchContent)
FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest/
  GIT_TAG main)

if(WIN32)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(algebra-test
  test/algebra.cc)

target_link_libraries(algebra-test
  PRIVATE
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(algebra-test)

add_custom_target(run
  DEPENDS algebra-test
  COMMAND ctest)
