#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "exif.h"
#include "image-exif.h"

static int print(FILE* stream, char* format, ...)
{
	va_list list;
	va_start(list, format);
	vfprintf(stream, format, list);
	va_end(list);

	return EXIF_SUCCESS;
}

static int getTypeSize(int type)
{
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

static void swabBytes(LONG count, ...)
{
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

static int setEndianess(APP1Header* app1_header)
{
	if (!app1_header) return HEADER_ERROR;

	if (app1_header->tiff_header.byteOrder == L_ENDIAN)
		isBigEndian = 0;
	else if (app1_header->tiff_header.byteOrder == B_ENDIAN)
		isBigEndian = 1;
	else return HEADER_ERROR;

	return EXIF_SUCCESS;
}

void limitOutputLength()
{
	isLongDataAreLimited = 1;
}

void unlimitOutlutLength()
{
	isLongDataAreLimited = 0;
}

//Sets the fractions output mode
void showDecimalValuesAsFractions()
{
	showAsFraction = 1;
}

//Set the fractions output mode
void showDecimalValuesAsDecimalFractions()
{
	showAsFraction = 0;
}

static char* getFileNameFromPath(char* path)
{
	if (!path) return NULL;

	int iter = 0;
	char* name = (char*)calloc(FILENAME_MAX, sizeof(char));

	size_t length = strlen(path);
	for (int i = length - 1; i >= 0; i--)
	{
		if (path[i] == '\\' || path[i] == '/') break;
		name[iter++] = path[i];
	}

	_strrev(name);

	return name;
}

static APP1Header* createAPP1Structure()
{
	APP1Header* newAPP1_header = (APP1Header*)calloc(1, sizeof(APP1Header));
	if (!newAPP1_header) return NULL;
	return newAPP1_header;
}

ExifMetadata* initIFDTables(const char* fileName)
{
	FILE* fin = fopen(fileName, "rb");
	if (fin == NULL) return NULL;

	ExifMetadata* newIFDArray = (ExifMetadata*)malloc(sizeof(ExifMetadata));
	size_t size = strlen(fileName) + 1;

	newIFDArray->fileName = (char*)calloc(1, size);
	strcpy_s(newIFDArray->fileName, size, fileName);

	IFDArray** ptr = (IFDArray*)newIFDArray;
	for (int i = 0; i < NUM_OF_IFD_TABLES; i++)
	{
		(ptr[i]) = (IFDArray*)calloc(1, sizeof(IFDArray));
	}

	newIFDArray->app1_header = createAPP1Structure();
	newIFDArray->App1StartOffset = readAPP1Header(fin, newIFDArray->app1_header);

	fclose(fin);

	return newIFDArray;
}

int freeIFDTables(ExifMetadata* metadata)
{
	IFDArray** ptr = (IFDArray*)metadata;
	for (int i = 0; i < NUM_OF_IFD_TABLES; i++)
	{
		if (ptr[i] != NULL)
		{
			for (int j = 0; j < ptr[i]->lenght; j++)
			{
				free(ptr[i]->tags[j].ptr);
				free(ptr[i]->tagsNames[j]);
			}
			free(ptr[i]->tags);
			free(ptr[i]->tagsNames);
			free(ptr[i]);
		}
	}
	free(metadata->fileName);
	free(metadata->app1_header);
	free(metadata);

	return EXIF_SUCCESS;
}

static int checkSOIMarker(FILE* image)
{
    SHORT SOI_marker;

    fseek(image, 0, SEEK_SET);
    fread(&SOI_marker, sizeof(SOI_marker), 1, image);

    if (SOI_marker == JPEG_SOI_TAG) return EXIF_SUCCESS;
    else return EXIF_FAILURE;
}

static int searchForAPP1Marker(FILE* image)
{
    SHORT buf;
    int offset = 1;
    while (1)
    {
        fread(&buf, sizeof(buf), 1, image);
        offset = ftell(image) - SIZE_OF_APP1_MARKER;
        if (buf == 0xE1FF) return offset;
    }

    return 0;
}

static int findTagInIFDTable(IFDArray* IFDTable, LONG tag)
{
	if (!IFDTable) return NO_IFD_TABLE_ERROR;

	for (size_t i = 0; i < IFDTable->lenght; i++)
	{
		if (IFDTable->tags[i].tag == tag) return IFDTable->tags[i].ptr;
	}

	return 0;
}

static int readAPP1Header(FILE* image, APP1Header* app1_header)
{
    size_t success;

    // Check for the SOI marker
    if (!checkSOIMarker(image))
        return NOT_JPEG_ERROR;

    // Search for the offset of the app1 structure
    int offset = searchForAPP1Marker(image);
    if (offset == -1)
        return APP1_MARKER_NOT_FOUND_ERROR;

    fseek(image, offset, SEEK_SET);

    success = fread(app1_header, SIZE_OF_DEFAULT_HEADER, 1, image);
    setEndianess(app1_header);

    // Swap the bytes of the count variable
	swabBytes(1, TYPE_SHORT, &app1_header->count);

    if (isBigEndian)
    {
        // Swap the bytes of the marker, Ifd0thOffset, and reserved variables
        swabBytes(3, TYPE_SHORT, &app1_header->marker,
            TYPE_LONG, &app1_header->tiff_header.Ifd0thOffset, TYPE_SHORT, &app1_header->tiff_header.reserved);
    }

    return offset;
}

static int readTagsInfo(FILE* image, IFDArray* array)
{
	if (!image) return NO_FILE_ERROR;

	SHORT numOfTags;

	fread(&numOfTags, sizeof(numOfTags), 1, image);
	if (isBigEndian) swabBytes(1, TYPE_SHORT, &numOfTags);

	array->lenght = numOfTags;
	array->tags = (Tag*)calloc(array->lenght, sizeof(Tag));

	if(array->tags != NULL) fread(array->tags, 12, numOfTags, image);

	if (isBigEndian)
	{
		for (size_t i = 0; i < numOfTags; i++)
		{
			swabBytes(8, TYPE_LONG, &array->tags[i].count, TYPE_LONG, &array->tags[i].ptr,
				TYPE_SHORT, &array->tags[i].tag, TYPE_SHORT, &array->tags[i].type);
		}
	}

	return numOfTags;
}

static int fillIFDTable(ExifMetadata* meta, FILE* image)
{
	IFDArray** ptr = (IFDArray*)meta;
	for (int currentTable = 0; currentTable < NUM_OF_IFD_TABLES; currentTable++)
	{
		if (ptr[currentTable] == NULL) continue;

		for (int currentTag = 0; currentTag < ptr[currentTable]->lenght; currentTag++)
		{
			int valueSize = ptr[currentTable]->tags[currentTag].count * 
				getTypeSize(ptr[currentTable]->tags[currentTag].type);

			int offset = (int)(ptr[currentTable]->tags[currentTag].ptr) + sizeof(APP1Header) - 
				sizeof(struct TIFF_header_s) + meta->App1StartOffset;

			ptr[currentTable]->tags[currentTag].ptr = 
				(BYTE*)calloc(1, (valueSize <= sizeof(LONG)) ? sizeof(LONG) : valueSize);

			size_t pos = ftell(image);
			fseek(image, offset, SEEK_SET);

			if (isBigEndian) swabBytes(1, TYPE_LONG, &offset);
		
			if (valueSize <= sizeof(LONG))
			{
				memcpy(ptr[currentTable]->tags[currentTag].ptr, &offset, sizeof(LONG));
			}
			else
			{
				// Read data into buffer
				fread(ptr[currentTable]->tags[currentTag].ptr, sizeof(char), valueSize, image);

				// Check if byte swapping is required
				if (isBigEndian) {
					BYTE* temp = (BYTE*)ptr[currentTable]->tags[currentTag].ptr;

					// Determine the type and size of the data
					int type = ptr[currentTable]->tags[currentTag].type;
					int size = getTypeSize(type);

					// Special handling for RATIONAL and SRATIONAL types
					if (type == TYPE_RATIONAL || type == TYPE_SRATIONAL) {
						size = sizeof(LONG);
						type = TYPE_LONG;
					}

					// Swap bytes for each block of data
					for (size_t k = 0; k < valueSize; k += size) {
						swabBytes(1, type, &temp[k]);
					}
				}
			}
			fseek(image, pos, SEEK_SET);
		}
	}
	setTagsNames(meta);

	return EXIF_SUCCESS;
}

int parseIFDs(ExifMetadata* meta)
{
	LONG numOfTags = 0;
	FILE* image = fopen(meta->fileName, "rb");
	
	fseek(image, SIZE_OF_DEFAULT_HEADER + meta->App1StartOffset, SEEK_SET);

	numOfTags = readTagsInfo(image, meta->IFD0thTags);

	meta->IFD0thTags->lenght = numOfTags;

	//Find offsets to next structures.
	LONG InterPointer = findTagInIFDTable(meta->IFD0thTags, InteroperabilityIFDPointer);
	LONG ExifPointer = findTagInIFDTable(meta->IFD0thTags, ExifIFDPointer);
	LONG GPSPointer = findTagInIFDTable(meta->IFD0thTags, GPSInfoIFDPointer);

	//Read offset to next IFD table if it's exist.
	LONG IFD1stPointer = 0;
	fread(&IFD1stPointer, sizeof(IFD1stPointer), 1, image);
	if (isBigEndian) swabBytes(1, TYPE_LONG, &IFD1stPointer);

	int beginPtr = sizeof(APP1Header) - sizeof(struct TIFF_header_s) + meta->App1StartOffset;

	if (InterPointer)
	{
		fseek(image, beginPtr + InterPointer, SEEK_SET);
		numOfTags = readTagsInfo(image, meta->InterTags);
	}

	if (ExifPointer)
	{
		fseek(image, beginPtr + ExifPointer, SEEK_SET);
		numOfTags = readTagsInfo(image, meta->ExifTags);
	}

	if (GPSPointer)
	{
		fseek(image,beginPtr + GPSPointer, SEEK_SET);
		numOfTags = readTagsInfo(image, meta->GPSTags);
	}

	if (IFD1stPointer)
	{
		fseek(image, beginPtr + IFD1stPointer, SEEK_SET);
		numOfTags = readTagsInfo(image, meta->IFD1stTags);
	}

	fillIFDTable(meta, image);

	fclose(image);

	return EXIF_SUCCESS;
}

int removeEXIFFromJPEG(ExifMetadata* metadata, const char* newImageName)
{
	SHORT SOImarker = 0xD8FF;

	char* newName = NULL;
	char noEXIF[FILENAME_MAX] = "NoEXIF_";

	if (newImageName == NULL)
	{
		strcat(noEXIF, getFileNameFromPath(metadata->fileName));
		newName = noEXIF;
	} 
	else newName = newImageName;

	FILE* inputImage = fopen(metadata->fileName, "rb");
	FILE* outputImage = fopen(newName, "wb");

	if (inputImage == NULL) return EXIF_FAILURE;

	fseek(inputImage, 0, SEEK_END);
	unsigned long long int fileSize = ftell(inputImage);
	rewind(inputImage);

	//write all data before app1 header
	fwrite(&SOImarker, sizeof(SHORT), 1, outputImage);

	LONG offset = metadata->app1_header->count + SIZE_OF_APP1_MARKER + metadata->App1StartOffset;

	fseek(inputImage, offset, SEEK_CUR);

	BYTE* buffer = (BYTE*)malloc(sizeof(BYTE) * fileSize - offset - SIZE_OF_SOI_MARKER);
	if (buffer != NULL)
	{
		fread(buffer, sizeof(BYTE), fileSize - offset - SIZE_OF_SOI_MARKER, inputImage);
		fwrite(buffer, sizeof(BYTE), fileSize - offset - SIZE_OF_SOI_MARKER, outputImage);
	}

	free(buffer);

	fclose(inputImage);
	fclose(outputImage);

	return EXIF_SUCCESS;
}

static int setTagsNames(ExifMetadata* metadata)
{
	char* tagName;

	IFDArray** ptr = (IFDArray*)metadata;
	for (int i = 0; i < NUM_OF_IFD_TABLES; i++)
	{
		if (ptr[i]->tags == NULL) continue;

		IFDArray* currentIFDTable = ptr[i];

		LONG numOfTags = currentIFDTable->lenght;
		currentIFDTable->tagsNames = (char**)calloc(numOfTags, sizeof(char*));

		for (size_t j = 0; j < numOfTags; j++)
		{
			LONG tag = currentIFDTable->tags[j].tag;
			LONG type = currentIFDTable->tags[j].type;

			//choosing a tag of a value
			switch (tag)
			{
			case(ImageWidth):					tagName = "ImageWigth";					break;
			case(Model):						tagName = "Model";						break;
			case(ImageLength):					tagName = "ImageLength";				break;
			case(BitsPerSample):				tagName = "BitsPerSample";				break;
			case(Compression):					tagName = "Compression";				break;
			case PhotometricInterpretation:		tagName = "PhotometricInterpretation";	break;
			case Orientation:					tagName = "Orientation";				break;
			case SamplesPerPixel:				tagName = "SamplesPerPixel";			break;
			case PlanarConfiguration:			tagName = "PlanarConfiguration";		break;
			case YCbCrSubSampling:				tagName = "YCbCrSubSampling";			break;
			case YCbCrPositioning:				tagName = "YCbCrPositioning";			break;
			case XResolution:					tagName = "XResolution";				break;
			case YResolution:					tagName = "YResolution";				break;
			case ResolutionUnit:				tagName = "ResolutionUnit";				break;	
			case StripOffsets:					tagName = "StripOffsets";				break;
			case RowsPerStrip:					tagName = "RowsPerStrip";				break;
			case StripByteCounts:				tagName = "StripByteCounts";			break;
			case JPEGInterchangeFormat:			tagName = "JPEGInterchangeFormat";		break;
			case JPEGInterchangeFormatLength:	tagName = "JPEGInterchangeFormatLength";break;
			case TransferFunction:				tagName = "TransferFunction";			break;
			case WhitePoint:					tagName = "WhitePoint";					break;
			case PrimaryChromaticities:			tagName = "PrimaryChromaticities";		break;
			case YCbCrCoefficients:				tagName = "YCbCrCoefficients";			break;
			case ReferenceBlackWhite:			tagName = "ReferenceBlackWhite";		break;
			case DateTime:						tagName = "DateTime";					break;
			case ImageDescription:				tagName = "ImageDescription";			break;
			case Make:							tagName = "Make";						break;
			case Software:						tagName = "Software";					break;
			case Artist:						tagName = "Artist";						break;
			case Copyright:						tagName = "Copyright";					break;
			case ExifIFDPointer:				tagName = "ExifIFDPointer";				break;
			case GPSInfoIFDPointer:				tagName = "GPSInfoIFDPointer";			break;
			case InteroperabilityIFDPointer:	tagName = "InteroperabilityIFDPointer"; break;
			case Rating:						tagName = "Rating";						break;
			case ExifVersion:					tagName = "ExifVersion";				break;
			case FlashPixVersion:				tagName = "FlashPixVersion";			break;
			case ColorSpace:					tagName = "ColorSpace";					break;
			case ComponentsConfiguration:		tagName = "ComponentsConfiguration";	break;
			case CompressedBitsPerPixel:		tagName = "CompressedBitsPerPixel";		break;
			case PixelXDimension:				tagName = "PixelXDimension";			break;
			case PixelYDimension:				tagName = "PixelYDimension";			break;
			case MakerNote:						tagName = "MakerNote";					break;
			case UserComment:					tagName = "UserComment";				break;
			case RelatedSoundFile:				tagName = "RelatedSoundFile";			break;
			case DateTimeOriginal:				tagName = "DateTimeOriginal";			break;
			case DateTimeDigitized:				tagName = "DateTimeDigitized";			break;
			case(SubSecTime):					tagName = "SubSecTime";					break;
			case(SubSecTimeOriginal):			tagName = "SubSecTimeOriginal";			break;
			case(SubSecTimeDigitized):			tagName = "SubSecTimeDigitized";		break;
			case(ExposureTime):					tagName = "ExposureTime";				break;
			case(FNumber):						tagName = "FNumber";					break;
			case(ExposureProgram):				tagName = "ExposureProgram";			break;
			case(SpectralSensitivity):			tagName = "SpectralSensitivity";		break;
			case(PhotographicSensitivity):		tagName = "PhotographicSensitivity";	break;
			case(OECF):							tagName = "OECF";						break;
			case(SensitivityType):				tagName = "SensitivityType";			break;
			case(StandardOutputSensitivity):	tagName = "StandardOutputSensitivity";	break;
			case(RecommendedExposureIndex):		tagName = "RecommendedExposureIndex";	break;
			case(ISOSpeed):						tagName = "ISOSpeed";					break;
			case(ISOSpeedLatitudeyyy):			tagName = "ISOSpeedLatitudeyyy";		break;
			case(ISOSpeedLatitudezzz):			tagName = "ISOSpeedLatitudezzz";		break;
			case(ShutterSpeedValue):			tagName = "ShutterSpeedValue";			break;
			case(ApertureValue):				tagName = "ApertureValue";				break;
			case(BrightnessValue):				tagName = "BrightnessValue";			break;
			case(ExposureBiasValue):			tagName = "ExposureBiasValue";			break;
			case(MaxApertureValue):				tagName = "MaxApertureValue";			break;
			case(SubjectDistance):				tagName = "SubjectDistance";			break;
			case(MeteringMode):					tagName = "MeteringMode";				break;
			case(LightSource):					tagName = "LightSource";				break;
			case(Flash):						tagName = "Flash";						break;		
			case(FocalLength):					tagName = "FocalLength";				break;
			case(SubjectArea):					tagName = "SubjectArea";				break;
			case(FlashEnergy):					tagName = "FlashEnergy";				break;
			case(SpatialFrequencyResponse):		tagName = "SpatialFrequencyResponse";	break;
			case(FocalPlaneXResolution):		tagName = "FocalPlaneXResolution";		break;
			case(FocalPlaneYResolution):		tagName = "FocalPlaneYResolution";		break;
			case(FocalPlaneResolutionUnit):		tagName = "FocalPlaneResolutionUnit";	break;
			case(SubjectLocation):				tagName = "SubjectLocation";			break;
			case(ExposureIndex):				tagName = "ExposureIndex";				break;
			case(SensingMethod):				tagName = "SensingMethod";				break;
			case(FileSource):					tagName = "FileSource";					break;
			case(SceneType):					tagName = "SceneType";					break;
			case(CFAPattern):					tagName = "CFAPattern";					break;	
			case(CustomRendered):				tagName = "CustomRendered";				break;
			case(ExposureMode):					tagName = "ExposureMode";				break;
			case(WhiteBalance):					tagName = "WhiteBalance";				break;
			case(DigitalZoomRatio):				tagName = "DigitalZoomRatio";			break;
			case(FocalLengthIn35mmFormat):		tagName = "FocalLengthIn35mmFormat";	break;
			case(SceneCaptureType):				tagName = "SceneCaptureType";			break;
			case(GainControl):					tagName = "GainControl";				break;
			case(Contrast):						tagName = "Contrast";					break;
			case(Saturation):					tagName = "Saturation";					break;
			case(Sharpness):					tagName = "Sharpness";					break;
			case(DeviceSettingDescription):		tagName = "DeviceSettingDescription";	break;
			case(SubjectDistanceRange):			tagName = "SubjectDistanceRange";		break;
			case(ImageUniqueID):				tagName = "ImageUniqueID";				break;
			case(CameraOwnerName):				tagName = "CameraOwnerName";			break;
			case(BodySerialNumber):				tagName = "BodySerialNumber";			break;
			case(LensSpecification):			tagName = "LensSpecification";			break;
			case(LensMake):						tagName = "LensMake";					break;
			case(LensModel):					tagName = "LensModel";					break;
			case(LensSerialNumber):				tagName = "LensSerialNumber";			break;
			case(Gamma):						tagName = "Gamma";						break;
			case(GPSVersionID):					tagName = "GPSVersionID";				break;
			case(GPSLatitudeRef):				tagName = "GPSLatitudeRef";				break;
			case(GPSLatitude):					tagName = "GPSLatitude";				break;
			case(GPSLongitudeRef):				tagName = "GPSLongitudeRef";			break;
			case(GPSLongitude):					tagName = "GPSLongitude";				break;
			case(GPSAltitudeRef):				tagName = "GPSAltitudeRef";				break;
			case(GPSAltitude):					tagName = "GPSAltitude";				break;
			case(GPSTimeStamp):					tagName = "GPSTimeStamp";				break;
			case(GPSSatellites):				tagName = "GPSSatellites";				break;
			case(GPSStatus):					tagName = "GPSStatus";					break;
			case(GPSMeasureMode):				tagName = "GPSMeasureMode";				break;
			case(GPSDOP):						tagName = "GPSDOP";						break;
			case(GPSSpeedRef):					tagName = "GPSSpeedRef";				break;
			case(GPSSpeed):						tagName = "GPSSpeed";					break;
			case(GPSTrackRef):					tagName = "GPSTrackRef";				break;
			case(GPSTrack):						tagName = "GPSTrack";					break;		
			case(GPSImgDirectionRef):			tagName = "GPSImgDirectionRef";			break;
			case(GPSImgDirection):				tagName = "GPSImgDirection";			break;
			case(GPSMapDatum):					tagName = "GPSMapDatum";				break;
			case(GPSDestLatitudeRef):			tagName = "GPSDestLatitudeRef";			break;
			case(GPSDestLatitude):				tagName = "GPSDestLatitude";			break;
			case(GPSDestLongitudeRef):			tagName = "GPSDestLongitudeRef";		break;
			case(GPSDestLongitude):				tagName = "GPSDestLongitude";			break;
			case(GPSBearingRef):				tagName = "GPSDestBearingRef";			break;
			case(GPSBearing):					tagName = "GPSDestBearing";				break;
			case(GPSDestDistanceRef):			tagName = "GPSDestDistanceRef";			break;
			case(GPSDestDistance):				tagName = "GPSDestDistance";			break;
			case(GPSProcessingMethod):			tagName = "GPSProcessingMethod";		break;
			case(GPSAreaInformation):			tagName = "GPSAreaInformation";			break;
			case(GPSDateStamp):					tagName = "GPSDateStamp";				break;
			case(GPSDifferential):				tagName = "GPSDifferential";			break;
			case(GPSHPositioningError):			tagName = "GPSHPositioningError";		break;
			default:							tagName = "Unknown tag";				break;
			}

			if (ptr[i]->tagsNames != NULL) currentIFDTable->tagsNames[j] = (char*)calloc(strlen(tagName) + 1, sizeof(char));
			if (ptr[i]->tagsNames != NULL) strcpy(currentIFDTable->tagsNames[j], tagName);
		}
	}
	return EXIF_SUCCESS;
}

int printMetadataIntoStream(ExifMetadata* metadata, const char* txtFileName, int mode)
{
	if (!metadata) return NO_DATA_ERROR;

	if (mode != TXT_STREAM_MODE && mode != STD_STREAM_MODE) mode = TXT_STREAM_MODE;

	FILE* stream;

	//Choose a stream
	if (mode) stream = stdout;
	else stream = fopen(txtFileName, "wt");

	RATIONAL	RationalValue = { 0 };
	SRATIONAL	SRationalValue = { 0 };

	BYTE* bytePtr;
	SBYTE* sbytePtr;
	SHORT* shortPtr;
	SSHORT* sshortPtr;
	LONG* longPtr;
	SLONG* slongPtr;

	IFDArray** ptr = (IFDArray*)metadata;
	for (int currTable = 0; currTable < NUM_OF_IFD_TABLES; currTable++)
	{
		if (ptr[currTable]->tags == NULL) continue;
		print(stream, "%s:\n", nameOfTables[currTable]);

		for (int currTag = 0; currTag < ptr[currTable]->lenght; currTag++)
		{
			print(stream, "%s: ", ptr[currTable]->tagsNames[currTag]);

			int type = ptr[currTable]->tags[currTag].type;
			int count = ptr[currTable]->tags[currTag].count;
			BYTE* curr_ptr = ptr[currTable]->tags[currTag].ptr;

			if (isBigEndian) swabBytes(1, type, curr_ptr);

			switch (type)
			{
			case(TYPE_ASCII):
				// In case current value has a '\n' symbol in the end, we just delete it to better output
				if (curr_ptr[count - 2] == '\n') curr_ptr[count - 2] = '\0';
				print(stream, "%s\n", curr_ptr);
				break;

			case(TYPE_BYTE):
				bytePtr = (BYTE*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					print(stream, "%u ", (BYTE)bytePtr[x]);
				print(stream, "%s", "\n");
				break;

			case(TYPE_SBYTE):
				sbytePtr = (SBYTE*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					print(stream, "%c ", sbytePtr[x]);
				print(stream, "%s", "\n");
				break;

			case(TYPE_SHORT):
				shortPtr = (SHORT*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					print(stream, "%d ", shortPtr[x]);
				print(stream, "%s", "\n");
				break;

			case(TYPE_SSHORT):
				sshortPtr = (SSHORT*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					print(stream, "%hd ", sshortPtr[x]);
				print(stream, "%s", "\n");
				break;

			case(TYPE_LONG):
				longPtr = (LONG*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					print(stream, "%I32d ", longPtr[x]);
				print(stream, "%s", "\n");
				break;

			case(TYPE_SLONG):
				slongPtr = (SLONG*)curr_ptr;
				for (size_t x = 0; x < count; x++)
					print(stream, "%d ", slongPtr[x]);
				print(stream, "%s", "\n");
				break;

			case(TYPE_RATIONAL):
				for (size_t x = 0; x < 1ULL * count; x += 1)
				{
					RATIONAL* fraction = (RATIONAL*)&(ptr[currTable]->tags[currTag].ptr[x * sizeof(RATIONAL)]);
					RationalValue.nominator = fraction->nominator;
					RationalValue.denominator = fraction->denominator;

					if (showAsFraction) print(stream, "%u/%u ", RationalValue.nominator, RationalValue.denominator);
					else print(stream, "%g ", (double)RationalValue.nominator / (double)RationalValue.denominator);
				}
				print(stream, "%s", "\n");
				break;

			case(TYPE_SRATIONAL):
				for (size_t x = 0; x < 1ULL * count; x += 1)
				{
					SRATIONAL* fraction = (SRATIONAL*)&(ptr[currTable]->tags[currTag].ptr[x * sizeof(SRATIONAL)]);
					SRationalValue.nominator = fraction->nominator;
					SRationalValue.denominator = fraction->denominator;

					if (showAsFraction) print(stream, "%d/%d ", SRationalValue.nominator, SRationalValue.denominator);
					else print(stream, "%g ", (double)SRationalValue.nominator / (double)SRationalValue.denominator);
				}
				print(stream, "%s", "\n");
				break;

			case(TYPE_UNDEFINED):
				for (size_t x = 0; x < count; x++)
				{
					if (isLongDataAreLimited && x == MAX_LENGTH_OF_STRING_DATA) break;

					unsigned char* temp = ptr[currTable]->tags[currTag].ptr;
					if (isgraph(temp[x]))
						print(stream, "%c ", ptr[currTable]->tags[currTag].ptr[x]);
					else print(stream, "0x%02x ", (char)ptr[currTable]->tags[currTag].ptr[x]);
				}
				print(stream, "%s", "\n");
				break;
			}
		}

		print(stream, "%s", "\n");
	}

	if (!mode) fclose(stream);

	return EXIT_SUCCESS;
}

void printMetadata(ExifMetadata* metadata)
{
	printMetadataIntoStream(metadata, NULL, STD_STREAM_MODE);
}

void saveMetadataInTXT(ExifMetadata* metadata, const char* txtFileName)
{
	printMetadataIntoStream(metadata, txtFileName, TXT_STREAM_MODE);
}
	