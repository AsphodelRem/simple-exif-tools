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
			new_tag->_tag_type = raw_table[table_index]->tags[tag_index].type;

			// Value is stored in offset
			if (valueSize <= sizeof(LONG)) {
				memcpy(new_tag->value, &offset, sizeof(LONG));
			} 
			else {
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

        free(metadata);
    }
}

void printMetadataIntoStream(ExifMetadata* metadata, char* file_name, int mode) {
    if (!metadata) {
        return;
    }

    char* names_of_tables[] = { "IFD0", "Inter", "GPS", "Exif", "IFD1" };

	RATIONAL	RationalValue = { 0 };
	SRATIONAL	SRationalValue = { 0 };

	BYTE* bytePtr;
	SBYTE* sbytePtr;
	SHORT* shortPtr;
	SSHORT* sshortPtr;
	LONG* longPtr;
	SLONG* slongPtr;

    //if (mode != TXT_STREAM_MODE && mode != STD_STREAM_MODE) mode = TXT_STREAM_MODE;

    if (mode) {
        freopen(file_name, "wt", stdout);
    }

    List** table = (List*)(metadata);
    for (int i = 0; i < NUMBER_OF_IFD_TABLES; i++) {
        printf("%s:\n", names_of_tables[i]);
		
		Node* iter = table[i]->head;
		while (iter) {
			// printf("%s: ", iter->tag_name);

			int type		= iter->_tag_type;
			int count		= iter->length;
			BYTE* curr_ptr	= iter->value;

			switch (type)
			{
			case(TYPE_ASCII):
				// In case current value has a '\n' symbol in the end, we just delete it to better output
				if (curr_ptr[count - 2] == '\n') curr_ptr[count - 2] = '\0';
				printf("%s\n", curr_ptr);
				break;

			case(TYPE_BYTE):
				bytePtr = (BYTE*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					printf("%u ", (BYTE)bytePtr[x]);
				printf("%s", "\n");
				break;

			case(TYPE_SBYTE):
				sbytePtr = (SBYTE*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					printf("%c ", sbytePtr[x]);
				printf("%s", "\n");
				break;

			case(TYPE_SHORT):
				shortPtr = (SHORT*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					printf("%d ", shortPtr[x]);
				printf("%s", "\n");
				break;

			case(TYPE_SSHORT):
				sshortPtr = (SSHORT*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					printf("%hd ", sshortPtr[x]);
				printf("%s", "\n");
				break;

			case(TYPE_LONG):
				longPtr = (LONG*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					printf("%I32d ", longPtr[x]);
				printf("%s", "\n");
				break;

			case(TYPE_SLONG):
				slongPtr = (SLONG*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					printf("%d ", slongPtr[x]);
				printf("%s", "\n");
				break;

			case(TYPE_RATIONAL):
				for (size_t x = 0; x < 1ULL * count; x += 1)
				{
					RATIONAL* fraction = (RATIONAL*)&(iter->value[x * sizeof(RATIONAL)]);
					RationalValue.nominator = fraction->nominator;
					RationalValue.denominator = fraction->denominator;

					printf("%u/%u ", RationalValue.nominator, RationalValue.denominator);
					// else print(stream, "%g ", (double)RationalValue.nominator / (double)RationalValue.denominator);
				}
				printf("%s", "\n");
				break;

			case(TYPE_SRATIONAL):
				for (size_t x = 0; x < 1ULL * count; x += 1)
				{
					SRATIONAL* fraction = (SRATIONAL*)&(iter->value[x * sizeof(SRATIONAL)]);
					SRationalValue.nominator = fraction->nominator;
					SRationalValue.denominator = fraction->denominator;

					printf("%d/%d ", SRationalValue.nominator, SRationalValue.denominator);
					// else print(stream, "%g ", (double)SRationalValue.nominator / (double)SRationalValue.denominator);
				}
				printf("%s", "\n");
				break;

			case(TYPE_UNDEFINED):
				for (size_t x = 0; x < count; x++)
				{
					// if (isLongDataAreLimited && x == MAX_LENGTH_OF_STRING_DATA) break;

					unsigned char* temp = iter->value;
					if (isgraph(temp[x])) {
						printf("%c ", temp[x]);
					}
					else printf("0x%02x ", temp[x]);
				}
				printf("%s", "\n");
				break;
			}

			iter = iter->next;
		}

		printf("%s", "\n");
	}

    if (mode) {
        fclose(stdout);
    }
}

