# get all cpp-files in current directory
file(GLOB utils_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)


foreach(util_filename ${utils_list})
  # get file name without extension
  get_filename_component(util ${util_filename} NAME_WE)

  # add and configure executable file to be produced 
  add_executable(${util} ${util_filename})
  target_link_libraries(${util} ${DEPENDENCIES})
  set_target_properties(${util} PROPERTIES
    OUTPUT_NAME "${util}"
    PROJECT_LABEL "${util}"
    RUNTIME_OUTPUT_DIRECTORY "../")
endforeach()