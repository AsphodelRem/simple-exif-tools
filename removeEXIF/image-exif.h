#ifndef AIEXIFIMAGE
#define AIEXIFIMAGE

#include "exif.h"

#define EXIF_SUCCESS 1
#define EXIF_FAILURE 0

#define STD_STREAM_MODE 1
#define TXT_STREAM_MODE 0

#define MAX_IFD_TBL_NUM 5
#define NUM_OF_IFD_TABLES 5

#define MAX_LENGTH_OF_STRING_DATA 16

enum errors
{
	APP1_MARKER_NOT_FOUND_ERROR = -6,
	HEADER_ERROR,
	NO_DATA_ERROR,
	NO_FILE_ERROR,
	NO_IFD_TABLE_ERROR,
	NO_EXIF_STRUCT_ERROR,
	NOT_JPEG_ERROR
};

typedef Tag Tag;

typedef struct IFDArray_s {
	Tag* tags;
	char** tagsNames;
	int lenght;
} IFDArray;

typedef struct EXIFTablesAndItsTags_s {
	IFDArray *IFD0thTags, *InterTags, *GPSTags, *ExifTags, *IFD1stTags;
	APP1Header* app1_header;
	int App1StartOffset;
	char* fileName;
} ExifMetadata;

static int isBigEndian = 0;
static int showAsFraction = 1;
static int isLongDataAreLimited = 1;

static char* nameOfTables[5] = { "IFD0", "Inter", "GPS", "Exif", "IFD1" };

// Creating EXIF structure 
ExifMetadata* initIFDTables(const char* fileName);

// Deleting EXIF structure
int freeIFDTables(ExifMetadata* metadata);

// Parse tags from JPEG file into IFD table
int parseIFDs(ExifMetadata* meta);

// Removing exif structure from JPEG image
// Creates new image without EXIF fields
// If newImageName is NULL, then a new image will have the old name with 'NoEXIF_' tag
int removeEXIFFromJPEG(ExifMetadata* metadata, const char* newImageName);

int printMetadataIntoStream(ExifMetadata* metadata, const char* txtFileName, int mode);

void printMetadata(ExifMetadata* metadata);

// Printing a founded metadata in 'txtFileName' file
void saveMetadataInTXT(ExifMetadata* metadata, const char* txtFileName);

// Limiting length of string data
void limitOutputLength();

void unlimitOutlutLength();

//Sets the fractions output mode
void showDecimalValuesAsFractions();

//Set the fractions output mode
void showDecimalValuesAsDecimalFractions();

#endif 

