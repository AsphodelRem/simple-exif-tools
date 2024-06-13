#include <stdlib.h>
#include <stdio.h>

#include <exif_delete.h>

#define MAX_FILENAME_SIZE 256

int removeEXIFFromJPEG(char* file_name, char* new_file_name)
{
	SHORT SOImarker = 0xD8FF;

	char prefix[MAX_FILENAME_SIZE] = "NoEXIF_";

	FILE* input_image = fopen(file_name, "rb");
	FILE* output_image = fopen(new_file_name, "wb");

	if (input_image == NULL) {
		return NO_DATA_ERROR;
	}

	fseek(input_image, 0, SEEK_END);
	unsigned long long int fileSize = ftell(input_image);
	rewind(input_image);

	//write all data before app1 header
	fwrite(&SOImarker, sizeof(SHORT), 1, output_image);

	RawEXIFTable* metadata = createRawExifTable();
	fillRawExifTable(file_name, metadata);
	LONG offset = metadata->app1_header->count + SIZE_OF_APP1_MARKER + metadata->app1_start_offset;

	fseek(input_image, offset, SEEK_CUR);

	BYTE* buffer = (BYTE*)malloc(sizeof(BYTE) * fileSize - offset - SIZE_OF_SOI_MARKER);
	if (buffer != NULL) {
		fread(buffer, sizeof(BYTE), fileSize - offset - SIZE_OF_SOI_MARKER, input_image);
		fwrite(buffer, sizeof(BYTE), fileSize - offset - SIZE_OF_SOI_MARKER, output_image);
	}

	free(buffer);

	fclose(input_image);
	fclose(output_image);

	freeRawExifTable(metadata);

	return EXIF_SUCCESS;
}