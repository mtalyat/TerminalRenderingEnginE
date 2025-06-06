﻿# CMakeList.txt : CMake project for Demo, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Demo")

# Add the executable for this project.
add_executable (Demo "Demo.c")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Demo PROPERTY CXX_STANDARD 20)
endif()

# Link to the library built from Source
find_library(TREE NAMES TREE PATHS "../Source/Build/" "../Source/Build/${CMAKE_BUILD_TYPE}/")

# Link to the TREE library
target_link_libraries(Demo PRIVATE ${TREE})

# Add include directory for TREE.h
target_include_directories(Demo PRIVATE "../Source/")