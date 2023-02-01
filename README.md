# IA-EXIF library

Simple library for detecting and removing exif metadata from .jpeg image.
Including a little console programm to use it.

There are some images for testing.

# Highligths:
* C99
* Compliling only with 32-bit version of compliler

# TO-DO:
* Removing specific tags
* Addition new tags
* Graphical interface
* RIFF (Wav) supporting

# API

## Structures

### Main structure

ExifMetadata is a structure used to store the parsed EXIF metadata from a JPEG file. The EXIF metadata is organized into different IFD (Image File Directory) tables, each containing different types of metadata tags. It contains all information you need.

```cpp
typedef struct EXIFTablesAndItsTags_s {
	IFDArray *IFD0thTags, *InterTags, *GPSTags, *ExifTags, *IFD1stTags;
	APP1Header* app1_header;
	int App1StartOffset;
	char* fileName;
} ExifMetadata;
```
The structure contains pointers to each of the IFD tables, including IFD0thTags, InterTags, GPSTags, ExifTags, and IFD1stTags. Additionally, it contains a pointer to the APP1 header of the EXIF data, the starting offset of the APP1 header in the file, and the name of the file being parsed. This structure allows easy access to all of the EXIF metadata in a organized manner for further processing or manipulation.


Each IFD table is discribed by this structure

``` cpp
typedef struct IFDArray_s {
	Tag* tags;
	char** tagsNames;
	int lenght;
} IFDArray;
```
IFDArray is a structure used to store the parsed EXIF metadata tags of an IFD table. The structure contains a pointer to the beginning of metadata tags, a list of the names of the tags, and the length of the tag's array. This structure allows for efficient storage and access of the metadata tags in a particular IFD table.

Tag's structure
``` cpp
#pragma pack(push, 1)
typedef struct Tag_s {
	SHORT tag;
	SHORT type;
	LONG  count;
	BYTE* ptr;
} Tag;
#pragma pack(pop)
```
It contains standard EXIF tag structure.
* tag - number meaning tag's name
* type - type of tag's value
* count - number of values
* ptr - value or offset to tags' value

## Functions

To get started, you need to initialize a ```cpp ExifMetadata``` structure
``` cpp
ExifMetadata* initIFDTables(const char* fileName);
```

This function is used to free allocated memory for ExifMetadata structure
``` cpp
int freeIFDTables(ExifMetadata* metadata);
```

This function is used for removing ALL metadata from your image. 
When it works, it creates a new image with name ```cpp newImageName``` contains the same data but without exif table.
If ``` cpp newImageName``` is NULL, creates a file with an old name with "No_Exif_" tag.
``` cpp
int removeEXIFFromJPEG(ExifMetadata* metadata, const char* newImageName);
```

Print metadata in given stream.
if mode == STD_STREAM_MODE, it calls ```cpp printMetadata```, which print metadata in stdout.
In case mode == TXT_STREAM_MODE, it calls ``` cpp saveMetadataInTXT``, which creates new file and saves the data in it.
``` cpp
int printMetadataIntoStream(ExifMetadata* metadata, const char* txtFileName, int mode);
```
Macros:
```cpp
#define STD_STREAM_MODE 1
#define TXT_STREAM_MODE 0
```

Print metadata into stdout
``` cpp
void printMetadata(ExifMetadata* metadata);
```

Save metadata in txt file with ```cpp txtFileName``` name.
``` cpp
// Printing a founded metadata in 'txtFileName' file
void saveMetadataInTXT(ExifMetadata* metadata, const char* txtFileName);
```

If some value is array containing a lot of elements, its data can take up most of the screen, which is not very convenient.
This two functions are used to limit/unlimin maximal length of output data.
``` cpp
// Limiting length of string data
void limitOutputLength();

void unlimitOutlutLength();
```
The maximum length is determined by the macro:
``` cpp 
#define MAX_LENGTH_OF_STRING_DATA 16
```

This function are used to change output mode of fractions
```cpp
void showDecimalValuesAsFractions();

void showDecimalValuesAsDecimalFractions();
```
