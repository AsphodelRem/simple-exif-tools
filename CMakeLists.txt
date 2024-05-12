cmake_minimum_required(VERSION 3.14)
project(simple_exif_editor LANGUAGES C)

include_directories(exif_editor exif_editor/src)
add_library(exif_editor_lib 
  exif_editor/exif_reader.c 
  exif_editor/src/linked_list.c 
  exif_editor/src/raw_exif.c) 

add_executable(exif_editor main.c)
target_link_libraries(exif_editor exif_editor_lib)