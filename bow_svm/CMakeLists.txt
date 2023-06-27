set(SAMPLE_FILENAME bow_svm.cpp)

# get file name without extension
get_filename_component(sample ${SAMPLE_FILENAME} NAME_WE)

set(SOURCES
  ${SAMPLE_FILENAME}
  CifarReader.cpp
  CifarReader.h
  BaseReader.h
)

# add and configure executable file to be produced 
add_executable(${sample} ${SOURCES})
target_link_libraries(${sample} ${DEPENDENCIES})

set_target_properties(${sample} PROPERTIES
  OUTPUT_NAME "${sample}"
  PROJECT_LABEL "${sample}"
  RUNTIME_OUTPUT_DIRECTORY "../")