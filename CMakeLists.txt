cmake_minimum_required(VERSION 3.16.1)

project(MurTree VERSION 1.0.0)

# Guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

# When type is not set the default is Release
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Guard against bad build-type strings
string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_tolower)
if(NOT cmake_build_type_tolower STREQUAL "debug"
   AND NOT cmake_build_type_tolower STREQUAL "release")
  message(FATAL_ERROR "Unknown build type \"${CMAKE_BUILD_TYPE}\". Allowed values are Debug, Release (case-insensitive).")
endif()

# Set C++ Standard
set(CMAKE_CXX_STANDARD 14)

# Set compiler flags
if(MSVC)
  # Force to always compile with warning level 3
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -O3 -std=c++14 -DNDEBUG -Wall")
endif()

# Set paths to header files
include_directories(
  "code/MurTee/Data Structures/"
  code/MurTree/Engine/
  code/MurTree/Utilities/
)

# Set paths to cpp files
set(DATA_STRUCTURES_SRC
  "code/MurTree/Data Structures/binary_data.cpp"
  "code/MurTree/Data Structures/branch.cpp"
  "code/MurTree/Data Structures/decision_tree.cpp"
  "code/MurTree/Data Structures/feature_vector_binary.cpp"
  "code/MurTree/Data Structures/internal_node_description.cpp"
  "code/MurTree/Data Structures/key_value_heap.cpp"
  "code/MurTree/Data Structures/symmetric_matrix_counter.cpp"
  "code/MurTree/Data Structures/symmetric_matrix_positive_negative_counter_2d.cpp"
)
set(ENGINE_SRC
  code/MurTree/Engine/binary_data_difference_computer.cpp
  code/MurTree/Engine/branch_cache.cpp
  code/MurTree/Engine/cache_closure.cpp
  code/MurTree/Engine/dataset_cache.cpp
  code/MurTree/Engine/feature_selector_abstract.cpp
  code/MurTree/Engine/hyper_parameter_tuner.cpp
  code/MurTree/Engine/similarity_lower_bound_computer.cpp
  code/MurTree/Engine/solver.cpp
  code/MurTree/Engine/specialised_binary_classification_decision_tree_solver.cpp
  code/MurTree/Engine/specialised_branch_misclassification_computer.cpp
  code/MurTree/Engine/specialised_general_branch_misclassification_computer.cpp
  code/MurTree/Engine/specialised_general_classification_decision_tree_solver.cpp
)
set(UTILITIES_SRC
  code/MurTree/Utilities/file_reader.cpp
  code/MurTree/Utilities/parameter_handler.cpp
)

# MurTree can be built as a:
#   - standalone app (default)
#   - shared library
option(BUILD_STANDALONE_APPLICATION "Build Murtree as a standalone application" OFF)
option(BUILD_SHARED_LIBRARY "Build MurTree as a shared library" OFF)
option(BUILD_PYTHON_MODULE "Build MurTree as a Python moduke" OFF)

if(BUILD_STANDALONE_APPLICATION AND NOT BUILD_SHARED_LIBRARY AND NOT BUILD_PYTHON_MODULE)
  # Build as a standalone app
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
  add_executable(murtree 
    main.cpp
    ${DATA_STRUCTURES_SRC}
    ${ENGINE_SRC}
    ${UTILITIES_SRC}
  ) 
elseif(BUILD_SHARED_LIBRARY AND NOT BUILD_STANDALONE_APPLICATION AND NOT BUILD_PYTHON_MODULE)
  # Build as a shared library
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
  add_compile_definitions(BUILD_LIBRARY)
  add_library(murtree SHARED
    main.cpp
    ${DATA_STRUCTURES_SRC}
    ${ENGINE_SRC}
    ${UTILITIES_SRC}
    )
elseif(BUILD_PYTHON_MODULE AND NOT BUILD_STANDALONE_APPLICATION AND NOT BUILD_SHARED_LIBRARY)
  # Build as a Python module
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
  add_compile_definitions(BUILD_PYTHON_MODULE)
  include(FetchContent)
  FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG        5b0a6fc2017fcc176545afe3e09c9f9885283242 # Version 2.10.4
  )
  FetchContent_MakeAvailable(pybind11)
  include_directories(${pybind11_SOURCE_DIR})
  add_subdirectory(${pybind11_SOURCE_DIR})
  pybind11_add_module(murtree_python_module
    main.cpp 
   ${DATA_STRUCTURES_SRC}
    ${ENGINE_SRC}
    ${UTILITIES_SRC}
  )
else()
    # More than one or none of the options are set to ON, show error message
    message(FATAL_ERROR "Please select exactly one of the following options to build: BUILD_STANDALONE_APPLICATION, BUILD_SHARED_LIBRARY or BUILD_PYTHON_MODULE.")
endif()

