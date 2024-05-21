#include <linked_list.h>

typedef struct ExifMetadata_s {
  List	*IFD0thTags, 
        *InterTags, 
        *GPSTags, 
        *ExifTags, 
        *IFD1stTags;
} ExifMetadata;

ExifMetadata *getExifMetadata(char* file_name);

void freeExifMetadata(ExifMetadata *metadata);

void printMetadataIntoStream(ExifMetadata* metadata, char* file_name, int mode);
