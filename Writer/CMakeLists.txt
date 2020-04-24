cmake_minimum_required (VERSION 3.15)

add_executable (Writer "Writer.cpp" "Writer.h")

set_target_properties(Writer PROPERTIES CXX_STANDARD 17)

find_package(fmt CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system date_time)
find_package(Flatbuffers CONFIG REQUIRED)

if(Boost_FOUND)
	target_include_directories(Writer PRIVATE ${Boost_INCLUDE_DIRS})
	target_link_libraries(Writer PRIVATE ${Boost_LIBRARIES})
endif(Boost_FOUND)

target_link_libraries(Writer PRIVATE 
		fmt::fmt fmt::fmt-header-only
        flatbuffers::flatbuffers)

if (WIN32)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # It seems that when using Clang on Windows we have to define BOOST_USE_WINDOWS_H to avoid errors
	# More info here: https://github.com/boostorg/thread/issues/139
	target_compile_definitions(Writer PRIVATE BOOST_USE_WINDOWS_H)
endif ()
endif (WIN32)

if (UNIX)
	target_link_libraries(Writer PRIVATE pthread rt)
endif (UNIX)