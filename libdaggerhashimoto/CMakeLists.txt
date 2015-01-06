include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(..)

set(EXECUTABLE daggerhashimoto)

add_library(${EXECUTABLE} daggerhashimoto.c num.c SHA3.cpp)
target_link_libraries(${EXECUTABLE} gmp) 
target_link_libraries(${EXECUTABLE} devcrypto)

add_executable (Test test.cpp)
target_link_libraries (Test 
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} 
  ${EXECUTABLE})

enable_testing ()
add_test(NAME DaggerHashimoto COMMAND Test)
