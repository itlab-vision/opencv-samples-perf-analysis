set(SAMPLE_FILENAME bow_svm.cpp)

include_directories("${DATA_READERS}/include")

FILE(GLOB readers_hdrs "${DATA_READERS}/include/*.h")
FILE(GLOB readers_srcs "${DATA_READERS}/src/*.cpp")

set(SOURCES
  ${SAMPLE_FILENAME}
  "${readers_hdrs}"
  "${readers_srcs}"
)

# get file name without extension
get_filename_component(sample ${SAMPLE_FILENAME} NAME_WE)

# add and configure executable file to be produced 
add_executable(${sample} ${SOURCES})
target_link_libraries(${sample} ${DEPENDENCIES})

set_target_properties(${sample} PROPERTIES
  OUTPUT_NAME "${sample}"
  PROJECT_LABEL "${sample}"
  RUNTIME_OUTPUT_DIRECTORY "../")