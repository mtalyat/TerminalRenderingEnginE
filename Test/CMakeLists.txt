# CMakeList.txt : CMake project for Test, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Test")

# Add the executable for this project.
add_executable (Test "Test.c")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Test PROPERTY CXX_STANDARD 20)
endif()

# Link to the library built from "../Source"
set(LIBRARY_OUTPUT_PATH "../Source/out/build/x64-debug/")
find_library(TREE NAMES TREE PATHS ${LIBRARY_OUTPUT_PATH})

message(STATUS "Looking for TREE in ${LIBRARY_OUTPUT_PATH}")
message(STATUS "TREE library found: ${TREE}")

if (TREE)
  target_link_libraries(Test PRIVATE ${TREE})
else()
  message(WARNING "Library TREE not found in ${LIBRARY_OUTPUT_PATH}. Falling back to manual path.")
  target_link_libraries(Test PRIVATE "../Source/out/build/x64-debug/TREE.lib")
endif()

# Add include directory for TREE.h
target_include_directories(Test PRIVATE "../Source/")

# Set the build directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../Source/out/build/x64-debug/")
