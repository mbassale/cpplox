cmake_minimum_required(VERSION 3.10)

project(CppLox VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#------------------------------------------------#
# LIBRARIES                                      #
#------------------------------------------------#

# AddressSanitizer
if(UNIX AND NOT APPLE)
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -fsanitize=address -fno-omit-frame-pointer -static-libstdc++ -static-libgcc -static-libasan -lrt")
  set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -ggdb -fsanitize=address -fno-omit-frame-pointer -static-libstdc++ -static-libgcc -static-libasan -lrt")
elseif(APPLE)
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -fsanitize=address -fno-omit-frame-pointer")
  set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -g3 -fsanitize=address -fno-omit-frame-pointer")
endif()

# GTest
find_package(GTest REQUIRED)

# GFlags
find_package(gflags REQUIRED)

# GLog
find_package (glog REQUIRED)

# Bison
find_package(BISON REQUIRED)

# Flex
find_package(FLEX REQUIRED)

#------------------------------------------------#
# CPPLOX SOURCES                                 #
#------------------------------------------------#
configure_file(config.h.in ${PROJECT_SOURCE_DIR}/src/config.h)

# Bison
BISON_TARGET(PythonLikeParser ${PROJECT_SOURCE_DIR}/src/parser.yy ${PROJECT_SOURCE_DIR}/src/python_parser.cpp)

# Flex
set(LEXER_INPUT_FILE ${PROJECT_SOURCE_DIR}/src/lexer.l)
FLEX_TARGET(
  PythonLikeLexer 
  ${LEXER_INPUT_FILE} 
  ${PROJECT_SOURCE_DIR}/src/lexer.cpp)

include_directories(src ${gflags_INCLUDE_DIR})
set(SOURCES
  src/common.h
  src/ast.h
  src/ast.cpp
  src/astbuilder.h
  src/astbuilder.cpp
  src/config.h
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
  src/environment.h
  src/environment.cpp
  src/evaluator.h
  src/evaluator.cpp
  src/settings.h
  src/settings.cpp
  src/lexer.h
  src/lexer_impl.cpp
  ${BISON_PythonLikeParser_OUTPUTS}
  ${FLEX_PythonLikeLexer_OUTPUTS}
)
add_library(libcpplox STATIC ${SOURCES})

add_executable(
  cpplox 
  src/main.cpp
)

if(UNIX AND NOT APPLE)
  set(ADDITIONAL_LIBRARIES Threads::Threads)
else()
  set(ADDITIONAL_LIBRARIES)
endif()

target_link_libraries(cpplox libcpplox ${ADDITIONAL_LIBRARIES} gflags glog::glog)

#------------------------------------------------#
# GOOGLE TEST                                    #
#------------------------------------------------#
enable_testing()


add_executable(
  cpplox_test
  tests/scanner_test.cpp
  tests/parser_test.cpp
  tests/object_test.cpp
  tests/environment_test.cpp
  tests/evaluator_test.cpp
)

target_link_libraries(
  cpplox_test
  libcpplox
  glog::glog
  GTest::gtest
  GTest::gtest_main
  ${ADDITIONAL_LIBRARIES}
)

include(GoogleTest)
gtest_discover_tests(cpplox_test)