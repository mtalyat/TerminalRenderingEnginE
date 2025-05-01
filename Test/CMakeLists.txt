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

# Link to the library built from Source
set(LIBRARY_OUTPUT_PATH "../Source/Build/")
find_library(TREE NAMES TREE PATHS ${LIBRARY_OUTPUT_PATH})

# Link to the TREE library
target_link_libraries(Test PRIVATE ${TREE})

# Add include directory for TREE.h
target_include_directories(Test PRIVATE "../Source/")