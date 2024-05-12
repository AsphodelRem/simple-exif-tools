#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <exif_reader.h>
#include <linked_list.h>
#include <raw_exif.h>

static void fillExifTable(FILE *data, 
                          ExifMetadata *metadata,
                          RawEXIFTable *raw_exif_data) {

  RawIFDArray** raw_table = (RawIFDArray*)raw_exif_data;
  List **ifd_array = (List*)metadata;
  
  int is_big_endian = isBigEndian(raw_exif_data->app1_header);

  for (int table_index = 0; table_index < NUMBER_OF_IFD_TABLES; table_index++) {
    if (raw_table[table_index] == NULL) {
      continue;
    }

    ifd_array[table_index] = createList(0);

    for (int tag_index = 0; tag_index < raw_table[table_index]->number_of_tags; tag_index++) {
      int valueSize = raw_table[table_index]->tags[tag_index].count *
                      getTypeSize(raw_table[table_index]->tags[tag_index].type);

      int offset = (int)(raw_table[table_index]->tags[tag_index].ptr) +
                   sizeof(APP1Header) - sizeof(struct TIFF_header_s) +
                   raw_exif_data->app1_start_offset;

			if (is_big_endian) {
        swabBytes(1, TYPE_LONG, &offset);
      }

      size_t pos = ftell(data);
      fseek(data, offset, SEEK_SET);

      Node *new_tag = createNode(NULL, (valueSize <= sizeof(LONG)) ? sizeof(LONG) : valueSize, 0);

      // Value is stored in offset
      if (valueSize <= sizeof(LONG)) {
        memcpy(new_tag->value, &offset, sizeof(LONG));
      } else {
        // Read data into buffer
        fread(new_tag->value, sizeof(char), valueSize, data);

        // Check if byte swapping is required
        if (is_big_endian) {
          BYTE *temp = (BYTE *)(new_tag->value);

          // Determine the type and size of the data
          int type = raw_table[table_index]->tags[tag_index].type;
          int size = getTypeSize(type);

          // Special handling for RATIONAL and SRATIONAL types
          if (type == TYPE_RATIONAL || type == TYPE_SRATIONAL) {
            size = sizeof(LONG);
            type = TYPE_LONG;
          }

          // Swap bytes for each block of data
          for (size_t k = 0; k < valueSize; k += size) {
            swabBytes(sizeof(BYTE), type, &temp[k]);
          }
        }
      }

      addNode(ifd_array[table_index], new_tag);
      fseek(data, pos, SEEK_SET);
    }
  }
}

ExifMetadata* getExifMetadata(char *file_name) {
  ExifMetadata *new_table = (ExifMetadata *)calloc(1, sizeof(ExifMetadata));

  RawEXIFTable *raw_exif_data = createRawExifTable();
  fillRawExifTable(file_name, raw_exif_data);

  // read and fill values
	FILE* data = fopen(file_name, "rb");
	fillExifTable(data, new_table, raw_exif_data);

  freeRawExifTable(raw_exif_data);

  return new_table;
}

void freeExifMetadata(ExifMetadata *metadata) {
  if (metadata) {
    List **table = (List *)(metadata);
    for (int i = 0; i < NUMBER_OF_IFD_TABLES; i++) {
      deleteList(table[i]);
    }
  }
}

