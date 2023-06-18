CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

set(PROJECT_NAME opencv_samples_perf_analysis)
project(${PROJECT_NAME})

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
set(DEPENDENCIES ${OpenCV_LIBS})

add_subdirectory(filter)
# add_subdirectory(resize)
# add_subdirectory(bow_svm)
# add_subdirectory(nn)

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