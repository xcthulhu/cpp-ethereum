include_directories (${CMAKE_CURRENT_SOURCE_DIR})
add_library (daggerhashimoto num.c)
target_link_libraries (daggerhashimoto gmp) 

add_executable (Test test.cpp)
target_link_libraries (Test 
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} 
  daggerhashimoto)

enable_testing ()
add_test(NAME DaggerHashimoto COMMAND Test)
