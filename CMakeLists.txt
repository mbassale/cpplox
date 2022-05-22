cmake_minimum_required(VERSION 3.10)

project(CppLox VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#------------------------------------------------#
# LIBRARIES                                      #
#------------------------------------------------#

# AddressSanitizer
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

if(UNIX AND NOT APPLE)
  set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
  set(THREADS_PREFER_PTHREAD_FLAG TRUE)
  find_package(Threads REQUIRED)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#------------------------------------------------#
# CPPLOX SOURCES                                 #
#------------------------------------------------#
configure_file(config.h.in src/config.h)

include_directories(src)
set(SOURCES
  src/common.h
  src/ast.h
  src/ast.cpp
  src/chunk.h
  src/chunk.cpp
  src/compiler.h
  src/compiler.cpp
  src/compiler_v2.h
  src/compiler_v2.cpp
  src/config.h
  src/debug.h
  src/debug.cpp
  src/lineinfo.h
  src/lineinfo.cpp
  src/scanner.h
  src/scanner.cpp
  src/parser.h
  src/parser.cpp
  src/object.h
  src/object.cpp
  src/function.h
  src/function.cpp
  src/native.h
  src/native.cpp
  src/closure.h
  src/closure.cpp
  src/upvalue.h
  src/upvalue.cpp
  src/value.h
  src/value.cpp
  src/vm.h
  src/vm.cpp
)
add_library(libcpplox STATIC ${SOURCES})

add_executable(
  cpplox 
  src/main.cpp
)

if(UNIX AND NOT APPLE)
  set(ADDITIONAL_LIBRARIES Threads::Threads)
else()
  set(ADDITIONAL_LIBRARIES "")
endif()

target_link_libraries(cpplox libcpplox ${ADDITIONAL_LIBRARIES})

#------------------------------------------------#
# GOOGLE TEST                                    #
#------------------------------------------------#
enable_testing()


add_executable(
  cpplox_test
  tests/value_test.cpp
  tests/scanner_test.cpp
  tests/parser_test.cpp
  tests/compiler_test.cpp
  tests/compiler_v2_test.cpp
)

target_link_libraries(
  cpplox_test
  libcpplox
  gtest
  gtest_main
  ${ADDITIONAL_LIBRARIES}
)

include(GoogleTest)
gtest_discover_tests(cpplox_test)