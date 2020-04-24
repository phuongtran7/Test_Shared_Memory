cmake_minimum_required (VERSION 3.15)

project ("Test_Shared_Memory")

# Include sub-projects.
add_subdirectory ("Test_Shared_Memory/Writer")
add_subdirectory ("Test_Shared_Memory/Reader")
