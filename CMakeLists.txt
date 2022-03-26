cmake_minimum_required(VERSION 3.10)

project(CppLox VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#------------------------------------------------#
# LIBRARIES                                      #
#------------------------------------------------#
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(Boost 1.7)
include_directories(${Boost_INCLUDE_DIRS})

#------------------------------------------------#
# CPPLOX SOURCES                                 #
#------------------------------------------------#
configure_file(config.h.in config.h)

set(SOURCES
  common.h
  chunk.h
  chunk.cpp
  compiler.h
  compiler.cpp
  config.h
  debug.h
  debug.cpp
  lineinfo.h
  lineinfo.cpp
  scanner.h
  scanner.cpp
  object.h
  object.cpp
  value.h
  value.cpp
  vm.h
  vm.cpp
)

add_executable(
  cpplox 
  main.cpp 
  ${SOURCES}
)

#------------------------------------------------#
# GOOGLE TEST                                    #
#------------------------------------------------#
enable_testing()

add_executable(
  cpplox_test
  value_test.cpp
  ${SOURCES}
)

target_link_libraries(
  cpplox_test
  gtest
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(cpplox_test)