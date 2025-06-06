﻿# CMakeList.txt : CMake project for TREE, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Set to static runtime library for MSVC.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project ("TREE" C)

# Add source to this project's static library.
add_library (TREE STATIC "TREE.c" "TREE.h")

# Set the output directory for the static library.
set_target_properties(TREE PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build")

# Set target link options to avoid linking against the default MSVC runtime library.
target_link_options(TREE PRIVATE /NODEFAULTLIB:MSVCRTD)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TREE PROPERTY CXX_STANDARD 20)
endif()
