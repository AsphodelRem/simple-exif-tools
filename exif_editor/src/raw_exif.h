#pragma once

#define JPEG_SOI_TAG			0xD8FF
#define EXIF_TAG				0xFFE1

#define INTEL					0x4949
#define L_ENDIAN				INTEL
#define MOTOROLLA				0x4D4D
#define B_ENDIAN				MOTOROLLA

#define SIZE_OF_SOI_MARKER		2
#define SIZE_OF_APP1_MARKER		2
#define SIZE_OF_DEFAULT_HEADER	18
#define SIZE_OF_TIFF_HEADER		12

#define MAX_IFD_TBL_NUM			5

typedef unsigned char			BYTE;
typedef char					SBYTE;
typedef unsigned short			SHORT;
typedef short					SSHORT;
typedef unsigned int			LONG;
typedef int						SLONG;
typedef char*					UNDEFINED;
typedef char*					ASCII;

typedef int EXIF_STATUS;

static int isBigEndian = 0;

enum EXIF_OPERATION_STATUS
{
	APP1_MARKER_NOT_FOUND_ERROR = -6,
	HEADER_ERROR,
	NO_DATA_ERROR,
	NO_FILE_ERROR,
	NO_IFD_TABLE_ERROR,
	NO_EXIF_STRUCT_ERROR,
	NOT_JPEG_ERROR,
	EXIF_SUCCESS,
};

enum IFDs
{
	IFD0th = 1,
	Inter,
	Exif,
	GPS,
	IFD1st
};

enum EXIF_TYPES
{
	TYPE_BYTE = 1,
	TYPE_ASCII,
	TYPE_SHORT,
	TYPE_LONG,
	TYPE_RATIONAL,
	TYPE_SBYTE,
	TYPE_UNDEFINED,
	TYPE_SSHORT,
	TYPE_SLONG,
	TYPE_SRATIONAL
};

typedef struct RATIONAL_s {
	LONG nominator;
	LONG denominator;
} RATIONAL;

typedef struct SRATIONAL_s {
	SLONG nominator;
	SLONG denominator;
} SRATIONAL;

#pragma pack(push, 1)
struct TIFF_header_s {
	SHORT byte_order;
	SHORT reserved;
	LONG ifd_0th_offset;
};
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct APP1Header_s {
	SHORT	marker;
	SHORT	count;
	BYTE	EXIF[6];
	struct TIFF_header_s
		tiff_header;
} APP1Header;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct Tag_s {
	SHORT tag;
	SHORT type;
	LONG  count;
	LONG  ptr;
} Tag;
#pragma pack(pop)

typedef struct InternalIFDArray_s {
	Tag* tags;
	char** tags_names;
	int number_of_tags;
} RawIFDArray;

typedef struct InternalEXIFData_s {
	RawIFDArray	*IFD0thTags, 
				*InterTags, 
				*GPSTags, 
				*ExifTags, 
				*IFD1stTags;

	APP1Header* app1_header;
	int app1_start_offset;
	char* file_name;
} RawEXIFTable;


// // Reads app1 header
// EXIF_STATUS readApp1Header(FILE* image, APP1Header* header);

// // Free app1 header
// void freeApp1Header(APP1Header* header);

// // Creates raw IFD array
// RawIFDArray* createInternalIFDArray();

// // Read raw ifd array
// EXIF_STATUS fillInternalIfdArray(FILE* image, RawIFDArray* array);

// // Free raw ifd array
// void freeRawIfdArray(RawIFDArray* array);

// Creates raw EXIF table
RawEXIFTable* createInternalEXIFTable();

int findTagInIfdArray(RawIFDArray* array, LONG tag);

// Fill raw EXIF table
EXIF_STATUS fillInternalExifTable(char* file_name, RawEXIFTable* table);

// Free internal EXIF Table
void freeRawExifTable(RawEXIFTable* table);
