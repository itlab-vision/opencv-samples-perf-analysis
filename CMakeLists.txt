CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
set(CMAKE_CXX_STANDARD 11)

set(PROJECT_NAME opencv_samples_perf_analysis)
project(${PROJECT_NAME})

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
set(DEPENDENCIES ${OpenCV_LIBS})

set(DATA_READERS "${CMAKE_CURRENT_SOURCE_DIR}/reader")

add_subdirectory(filter)
add_subdirectory(resize)
add_subdirectory(bow_svm)
add_subdirectory(nn)

add_subdirectory(utils)

# REPORT
message(STATUS "")
message(STATUS "General configuration for ${PROJECT_NAME}")
message(STATUS "======================================")
message(STATUS "")
message(STATUS "   Configuration:              ${CMAKE_BUILD_TYPE}")
message(STATUS "   OpenCV build path:          ${OpenCV_DIR}")
message(STATUS "   OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "   OpenCV library directories: ${OpenCV_LIBS}")
message( STATUS "")