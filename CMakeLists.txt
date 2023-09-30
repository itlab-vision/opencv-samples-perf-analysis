CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
set(CMAKE_CXX_STANDARD 11)

set(PROJECT_NAME opencv_samples_perf_analysis)
project(${PROJECT_NAME})

# Set build type
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Check OpenMP flag
option(WITH_OPENMP "Enable OpenMP support" OFF) # OFF by default
if(WITH_OPENMP)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else()
        message(FATAL_ERROR "OpenMP was not found")
    endif()
endif()

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})

# Find OpenCV package
find_package(OpenCV REQUIRED)
# Include OpenCV header files
include_directories(${OpenCV_INCLUDE_DIRS})
# Set OpenCV libraries to link with samples
set(DEPENDENCIES ${OpenCV_LIBS})


# Set include directory for data readers
set(DATA_READERS "${CMAKE_CURRENT_SOURCE_DIR}/reader")
# Add subdirectories to create applications
add_subdirectory(filter)
add_subdirectory(resize)
add_subdirectory(erosion)
add_subdirectory(bow_svm)
add_subdirectory(nn)
add_subdirectory(utils)


# Report
message(STATUS "")
message(STATUS "======================================")
message(STATUS "General configuration for ${PROJECT_NAME}")
message(STATUS "======================================")
message(STATUS "")
message(STATUS "  Configuration:                ${CMAKE_BUILD_TYPE}")
message(STATUS "    CMake C compiler:           ${CMAKE_C_COMPILER}")
message(STATUS "    CMake C++ compiler:         ${CMAKE_CXX_COMPILER}")
message(STATUS "    CMake C flags:              ${CMAKE_C_FLAGS}")
message(STATUS "    CMake C++ flags:            ${CMAKE_CXX_FLAGS}")
message(STATUS "    CMake linker flags:         ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "--------------------------------------")
message(STATUS "    OpenCV build path:          ${OpenCV_DIR}")
message(STATUS "    OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "    OpenCV install path:        ${OpenCV_INSTALL_PATH}")
message(STATUS "    OpenCV libraries:           ${OpenCV_LIBS}")
message(STATUS "--------------------------------------")
message(STATUS "    OpenMP support:             ${WITH_OPENMP}")
message(STATUS "    OpenMP C flags:             ${OpenMP_C_FLAGS}")
message(STATUS "    OpenMP C++ flags:           ${OpenMP_CXX_FLAGS}")
message(STATUS "    OpenMP linker flags:        ${OpenMP_EXE_LINKER_FLAGS}")
message(STATUS "======================================")
message(STATUS "")
