cmake_minimum_required (VERSION 3.12)
project (bit_view_unit_tests VERSION 0.1 LANGUAGES CXX)

add_executable(bit_view_test bit_view_test.cxx)
target_include_directories(bit_view PUBLIC ../include)
target_include_directories(bit_view_test PUBLIC ../include)
target_link_libraries(bit_view_test bit_view)
target_link_libraries(bit_view_test gtest)

#install(TARGETS bit_view_test DESTINATION bin)
