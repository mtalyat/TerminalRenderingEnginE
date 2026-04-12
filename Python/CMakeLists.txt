cmake_minimum_required(VERSION 3.12)
project(TREE_Python_Module)

# Find Python
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Add the executable or library
add_library(TREE SHARED TREEmodule.c ../Source/TREE.c)

# Include Python headers and TREE.h directory
target_include_directories(TREE PRIVATE ${Python3_INCLUDE_DIRS} ../Source)

# Link against Python libraries
target_link_libraries(TREE PRIVATE ${Python3_LIBRARIES})

# Set the output name to match the Python module name
set_target_properties(TREE PROPERTIES PREFIX "" SUFFIX ".pyd") # ".pyd" for Windows, ".so" for Linux/macOS