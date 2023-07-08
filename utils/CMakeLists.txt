# get all cpp-files in current directory
file(GLOB utils_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach(util_filename ${utils_list})
  # get file name without extension
  get_filename_component(util ${util_filename} NAME_WE)

  if(${util} STREQUAL check_svm_accuracy)
    include_directories("${DATA_READERS}/include")

    FILE(GLOB readers_hdrs "${DATA_READERS}/include/*.h")
    FILE(GLOB readers_srcs "${DATA_READERS}/src/*.cpp")

    set(SOURCES
      ${util_filename}
      "${readers_hdrs}"
      "${readers_srcs}"
    )
  else()
    set(SOURCES
        ${util_filename}
    )
  endif()

  # add and configure executable file to be produced 
  add_executable(${util} ${SOURCES})
  target_link_libraries(${util} ${DEPENDENCIES})
  set_target_properties(${util} PROPERTIES
    OUTPUT_NAME "${util}"
    PROJECT_LABEL "${util}"
    RUNTIME_OUTPUT_DIRECTORY "../"
  )
endforeach()