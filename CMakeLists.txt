cmake_minimum_required(VERSION 3.14)
project(simple_exif_tools LANGUAGES C)

include_directories(exif_tools exif_tools/src)
add_library(exif_tools_lib 
  exif_tools/exif_reader.c 
  exif_tools/exif_delete.c
  exif_tools/src/linked_list.c 
  exif_tools/src/raw_exif.c) 

# add_executable(exif_editor main.c)
# target_link_libraries(exif_editor exif_tools_lib)
