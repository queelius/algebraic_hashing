cmake_minimum_required (VERSION 3.12)
project (bit_viewers VERSION 0.1 LANGUAGES CXX)

add_library(bit_view STATIC
	bit_view.cpp
	../include/packed_container/bit_view.h
)

add_library(bit_matrix_view STATIC
	bit_matrix_view.cpp
	../include/packed_container/bit_matrix_view.h
)

add_library(codec elias_omega_codec.h )

target_include_directories(bit_view PUBLIC ../include)
target_include_directories(bit_matrix_view PUBLIC ../include)
