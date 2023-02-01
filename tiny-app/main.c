#include <stdio.h>

#include "image-exif.h"

#define SIZE_OF_BUFFER 1024

#define SAVE_META	(int)('s')
#define OPEN_FILE	(int)('o')
#define REMOVE_META (int)('r')
#define SHOW_HELP	(int)('h')
#define SHOW_META	(int)('p')
#define EXIT		(int)('e')
#define LIMIT_LEN	(int)('l')
#define USE_FRACT	(int)('f')

char buffer[SIZE_OF_BUFFER];

ExifMetadata* exifMd = NULL;

void showReference()
{
	printf_s("%s\t\t\t\t\n", "IMAGE EXIF");
	printf_s("%s\n", "Commands:");
	printf_s("%c: %s\n%c: %s\n%c: %s\n%c: %s\n%c: %s\n%c: %s\n%c: %s\n%c: %s\n\n", 
		OPEN_FILE, "open file",
		SAVE_META, "save metadata in file",
		SHOW_META, "show metadata",
		REMOVE_META, "remove metadata from jpeg",
		LIMIT_LEN, "limit maximum size of data (yes(1), no(0))",
		USE_FRACT, "change fraction output mode (usual fraction(0), decimal fraction(1))",
		SHOW_HELP, "show reference",
		EXIT, "exit");
}

int main()
{
	showReference();

	while (1)
	{
		int choice = 0;

		gets_s(buffer, SIZE_OF_BUFFER);
		choice = (int)buffer[0];

		switch (choice)
		{
		case SAVE_META:
			if (exifMd != NULL)
{
				printf_s("> %s\n", "Enter a file name");
				gets_s(buffer, SIZE_OF_BUFFER);
				saveMetadataInTXT(exifMd, buffer);
			}
			break;

		case OPEN_FILE:
			printf_s("> %s\n", "Enter a file name");
			gets_s(buffer, SIZE_OF_BUFFER);
			
			if (exifMd != NULL) freeIFDTables(exifMd);
			exifMd = initIFDTables(buffer);

			if (exifMd == NULL)
			{
				printf_s("> %s\n\n", "There's no such file");
				continue;
			}

			parseIFDs(exifMd);
			break;

		case REMOVE_META:
			if (exifMd != NULL) 
				removeEXIFFromJPEG(exifMd, NULL);
			break;

		case SHOW_HELP:
			showReference();
			break;

		case SHOW_META:
			if (exifMd != NULL) 
				printMetadata(exifMd);
			break;

		case USE_FRACT:
			printf_s("> %s\n", "Choose fraction output mode(usual fraction(0), decimal fraction(1))");
			gets_s(buffer, SIZE_OF_BUFFER);
			if ((int)buffer[0] <= '0') showDecimalValuesAsFractions();
			else showDecimalValuesAsDecimalFractions();  
			printf_s("> %s\n", "Ok");
			continue;

		case LIMIT_LEN:
			printf_s("> %s\n", "Do you want to limit maximum size of data ? (yes(1), no(0))");
			gets_s(buffer, SIZE_OF_BUFFER);
			if ((int)buffer[0] >= '1') limitOutputLength();
			else unlimitOutlutLength();
			printf_s("> %s\n", "Ok");
			continue;
			
		case EXIT:
			if (exifMd != NULL) freeIFDTables(exifMd);
			return 0;

		default:
			printf_s("> %s\n", "Unknown command :(");
			continue;
		}

		if (exifMd == NULL) printf_s("> %s\n\n", "No file");
		else printf_s("> %s\n\n", "Done!");
	}


	return 0;
}