#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <exif.h>
#include <raw_exif.h>

void swabBytes(LONG count, ...) {
	SHORT* short_v;
	SSHORT* sshort_v;
	LONG* long_v;
	SLONG* slong_v;
	LONG	type;

	va_list list;
	va_start(list, count);

	for (size_t i = 0; i < count; i++)
	{
		type = va_arg(list, LONG);

		switch (type)
		{
		case(TYPE_SHORT):
		case(TYPE_SSHORT):
			short_v = va_arg(list, SHORT*);
			*short_v = ((*short_v) << 8) | (((*short_v) >> 8) & 0x00FF);
			break;

		case(TYPE_LONG):
		case(TYPE_SLONG):
			long_v = va_arg(list, LONG*);
			*long_v = (((*long_v) << 24) & 0xFF000000) | (((*long_v) << 8) & 0x00FF0000) |
				(((*long_v) >> 8) & 0x0000FF00) | (((*long_v) >> 24) & 0x000000FF);
			break;
		}
	}

	va_end(list);
}

int isBigEndian(APP1Header* app1_header) {
	if (!app1_header) {
		return HEADER_ERROR;
	}

	if (app1_header->tiff_header.byte_order == L_ENDIAN) {
		return 0;
	}

	if (app1_header->tiff_header.byte_order == B_ENDIAN) {
		return 1;
	}
}

int getTypeSize(int type) {
	switch (type)
	{
	case(TYPE_BYTE):
	case(TYPE_SBYTE):
		return sizeof(BYTE);
		break;

	case(TYPE_SHORT):
	case(TYPE_SSHORT):
		return sizeof(SHORT);
		break;

	case(TYPE_LONG):
	case(TYPE_SLONG):
		return sizeof(LONG);
		break;

	case(TYPE_RATIONAL):
	case(TYPE_SRATIONAL):
		return sizeof(RATIONAL);
		break;

	case(TYPE_ASCII):
	case(TYPE_UNDEFINED):
		return sizeof(char);
		break;

	default:
		return 0;
	}
}

// Creates App1 Header structure
static APP1Header* createApp1Header() {
	APP1Header* new_app1_header = (APP1Header*)calloc(1, sizeof(APP1Header));
	if (!new_app1_header) {
		return NULL;
	}
	return new_app1_header;
}

static EXIF_STATUS checkSOIMarker(FILE* image) {
    SHORT SOI_marker;

    fseek(image, 0, SEEK_SET);
    fread(&SOI_marker, sizeof(SOI_marker), 1, image);

    if (SOI_marker == JPEG_SOI_TAG) return EXIF_SUCCESS;
    else return HEADER_ERROR;
}

static int searchForAPP1Marker(FILE* image) {
	SHORT buf;
	int offset = 1;
	while (fread(&buf, sizeof(buf), 1, image))
	{
		offset = ftell(image) - SIZE_OF_APP1_MARKER;
		if (buf == 0xE1FF) return offset;
	}
	return 0;
}

// Reads app1 header
static EXIF_STATUS readApp1Header(FILE* image, APP1Header* header) {
    // Check for the SOI marker
    if (!checkSOIMarker(image)) {
			return NOT_JPEG_ERROR;
		}

    // Search for the offset of the app1 structure
    int offset = searchForAPP1Marker(image);
    if (offset == -1) {
			return APP1_MARKER_NOT_FOUND_ERROR;
    }

    fseek(image, offset, SEEK_SET);
    fread(header, SIZE_OF_DEFAULT_HEADER, 1, image);

		int is_big_endian = isBigEndian(header);

    // Swap the bytes of the count variable
    swabBytes(1, TYPE_SHORT, &header->count);

    if (is_big_endian) {
			// Swap the bytes of the marker, Ifd0thOffset, and reserved variables
			swabBytes(3, TYPE_SHORT, &header->marker,
					TYPE_LONG, &header->tiff_header.ifd_0th_offset, TYPE_SHORT, &header->tiff_header.reserved);
    }

		return offset;
}

// Free app1 header
void freeApp1Header(APP1Header* header) {
	if (header) {
		free(header);
	}
}

// Creates raw IFD array
static RawIFDArray* createRawIfdArray() {
	RawIFDArray* new_array = (RawIFDArray*)calloc(1, sizeof(RawIFDArray));

	if (!new_array) {
		return NULL;
	}

	return new_array;
}

// Read raw ifd array
static EXIF_STATUS fillRawIfdArray(FILE* image, RawIFDArray* array, int is_big_endian) {
	if (!image) {
		return NO_FILE_ERROR;
	}

	SHORT num_of_tags;
	fread(&num_of_tags, sizeof(num_of_tags), 1, image);

	if (is_big_endian) {
		swabBytes(1, TYPE_SHORT, &num_of_tags);
	}

	array->number_of_tags = num_of_tags;
	array->tags = (Tag*)calloc(array->number_of_tags, sizeof(Tag));

	if (array->tags == NULL) {
		return MEMORY_ALLOCATION_ERROR;
	}

	if (array->tags != NULL) {
		fread(array->tags, sizeof(Tag), num_of_tags, image);
	}

	if (is_big_endian) {
		for (size_t i = 0; i < num_of_tags; i++) {
			swabBytes(3, TYPE_LONG, &array->tags[i].count, 
				TYPE_SHORT, &array->tags[i].tag, 
				TYPE_SHORT, &array->tags[i].type);

			if (array->tags[i].type != TYPE_UNDEFINED) {
				swabBytes(1, TYPE_LONG, &array->tags[i].ptr);
			}
		}
	}

	array->number_of_tags = num_of_tags;

	return EXIF_SUCCESS;
}

// Free raw ifd array
static void freeRawIfdArray(RawIFDArray* array) {
	if (!array) {
		return;
	}
	
	if (array->tags_names) {
		for (int i = 0; i < array->number_of_tags; i++) {
			if (array->tags_names[i]) {
				free(array->tags_names[i]);
			}
		}
		free(array->tags);
		free(array);
	}
}

// Creates raw EXIF table
RawEXIFTable* createRawExifTable() {
	RawEXIFTable* new_table = (RawEXIFTable*)malloc(sizeof(RawEXIFTable));

	if (!new_table) {
		return NULL;
	}

	new_table->app1_header = createApp1Header();

	RawIFDArray** ptr_to_ifd_arrays = (RawIFDArray*)new_table;
	for (int i = 0; i < NUMBER_OF_IFD_TABLES; i++) {
		ptr_to_ifd_arrays[i] = createRawIfdArray();
	}

	return new_table;
}

static int findTagInIfdArray(RawIFDArray* array, LONG tag) {
	if (!array) {
		return NO_IFD_TABLE_ERROR;
	}

	for (size_t i = 0; i < array->number_of_tags; i++) {
		if (array->tags[i].tag == tag) {
			return array->tags[i].ptr;
		}
	}

	return 0;
}

// Fill raw EXIF table
EXIF_STATUS fillRawExifTable(char* file_name, RawEXIFTable* table) {
	FILE* image = fopen(file_name, "rb");

	if (!image) {
		return NO_FILE_ERROR;
	}

	table->app1_start_offset = readApp1Header(image, table->app1_header);
	int is_big_endian = isBigEndian(table->app1_header);

	fseek(image, SIZE_OF_DEFAULT_HEADER + table->app1_start_offset, SEEK_SET);
	fillRawIfdArray(image, table->IFD0thTags, is_big_endian);

	//Find offsets to next structures.
	LONG inter_table_offset = findTagInIfdArray(table->IFD0thTags, InteroperabilityIFDPointer);
	LONG exif_table_offset = findTagInIfdArray(table->IFD0thTags, ExifIFDPointer);
	LONG GPS_table_offset = findTagInIfdArray(table->IFD0thTags, GPSInfoIFDPointer);

	//Read an offset to the next IFD table if it exists.
	LONG IFD1st_table_offset = 0;
	fread(&IFD1st_table_offset, sizeof(IFD1st_table_offset), 1, image);

	if (is_big_endian) {
		swabBytes(1, TYPE_LONG, &IFD1st_table_offset);
	}

	int start_offset = sizeof(APP1Header) - sizeof(struct TIFF_header_s) + table->app1_start_offset;

	if (inter_table_offset) {
		fseek(image, start_offset + inter_table_offset, SEEK_SET);
		fillRawIfdArray(image, table->InterTags, is_big_endian);
	}

	if (exif_table_offset) {
		fseek(image, start_offset + exif_table_offset, SEEK_SET);
		fillRawIfdArray(image, table->ExifTags, is_big_endian);
	}

	if (GPS_table_offset) {
		fseek(image, start_offset + GPS_table_offset, SEEK_SET);
		fillRawIfdArray(image, table->GPSTags, is_big_endian);
	}

	if (IFD1st_table_offset) {
		fseek(image, start_offset + IFD1st_table_offset, SEEK_SET);
		fillRawIfdArray(image, table->IFD1stTags, is_big_endian);
	}

	fclose(image);

	return EXIF_SUCCESS;
}

// Free raw EXIF Table
void freeRawExifTable(RawEXIFTable* table) {
	if (!table) {
		return;
	}

	freeApp1Header(table->app1_header);

	RawIFDArray** ptr_to_ifd_arrays = (RawIFDArray*)table;
	for (int i = 0; i < NUMBER_OF_IFD_TABLES; i++) {
		freeRawIfdArray(ptr_to_ifd_arrays[i]);
	}

	free(table);
}
