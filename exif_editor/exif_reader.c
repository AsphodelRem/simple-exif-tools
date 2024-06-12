#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <exif_reader.h>
#include <linked_list.h>
#include <raw_exif.h>
#include <exif.h>


static char* getTagName(int tag)
{
    //Choosing tag name by tag's index
    switch (tag)
    {
        case(ImageWidth):					return "ImageWigth";				    break;
        case(Model):						return "Model";						    break;
        case(ImageLength):					return "ImageLength";				    break;
        case(BitsPerSample):				return "BitsPerSample";				    break;
        case(Compression):					return "Compression";				    break;
        case PhotometricInterpretation:		return "PhotometricInterpretation";	    break;
        case Orientation:					return "Orientation";				    break;
        case SamplesPerPixel:				return "SamplesPerPixel";			    break;
        case PlanarConfiguration:			return "PlanarConfiguration";		    break;
        case YCbCrSubSampling:				return "YCbCrSubSampling";			    break;
        case YCbCrPositioning:				return "YCbCrPositioning";			    break;
        case XResolution:					return "XResolution";				    break;
        case YResolution:					return "YResolution";				    break;
        case ResolutionUnit:				return "ResolutionUnit";			    break;
        case StripOffsets:					return "StripOffsets";			    	break;
        case RowsPerStrip:					return "RowsPerStrip";				    break;
        case StripByteCounts:				return "StripByteCounts";			    break;
        case JPEGInterchangeFormat:			return "JPEGInterchangeFormat";		    break;
        case JPEGInterchangeFormatLength:	return "JPEGInterchangeFormatLength";   break;
        case TransferFunction:				return "TransferFunction";			    break;
        case WhitePoint:					return "WhitePoint";				    break;
        case PrimaryChromaticities:			return "PrimaryChromaticities";		    break;
        case YCbCrCoefficients:				return "YCbCrCoefficients";			    break;
        case ReferenceBlackWhite:			return "ReferenceBlackWhite";		    break;
        case DateTime:						return "DateTime";					    break;
        case ImageDescription:				return "ImageDescription";			    break;
        case Make:							return "Make";						    break;
        case Software:						return "Software";					    break;
        case Artist:						return "Artist";						break;
        case Copyright:						return "Copyright";					    break;
        case ExifIFDPointer:				return "ExifIFDPointer";				break;
        case GPSInfoIFDPointer:				return "GPSInfoIFDPointer";			    break;
        case InteroperabilityIFDPointer:	return "InteroperabilityIFDPointer";    break;
        case Rating:						return "Rating";						break;
        case ExifVersion:					return "ExifVersion";				    break;
        case FlashPixVersion:				return "FlashPixVersion";		    	break;
        case ColorSpace:					return "ColorSpace";					break;
        case ComponentsConfiguration:		return "ComponentsConfiguration";	    break;
        case CompressedBitsPerPixel:		return "CompressedBitsPerPixel";		break;
        case PixelXDimension:				return "PixelXDimension";			    break;
        case PixelYDimension:				return "PixelYDimension";			    break;
        case MakerNote:						return "MakerNote";					    break;
        case UserComment:					return "UserComment";				    break;
        case RelatedSoundFile:				return "RelatedSoundFile";			    break;
        case DateTimeOriginal:				return "DateTimeOriginal";			    break;
        case DateTimeDigitized:				return "DateTimeDigitized";			    break;
        case(SubSecTime):					return "SubSecTime";					break;
        case(SubSecTimeOriginal):			return "SubSecTimeOriginal";			break;
        case(SubSecTimeDigitized):			return "SubSecTimeDigitized";		    break;
        case(ExposureTime):					return "ExposureTime";				    break;
        case(FNumber):						return "FNumber";					    break;
        case(ExposureProgram):				return "ExposureProgram";			    break;
        case(SpectralSensitivity):			return "SpectralSensitivity";		    break;
        case(PhotographicSensitivity):		return "PhotographicSensitivity";	    break;
        case(OECF):							return "OECF";						    break;
        case(SensitivityType):				return "SensitivityType";		    	break;
        case(StandardOutputSensitivity):	return "StandardOutputSensitivity";	    break;
        case(RecommendedExposureIndex):		return "RecommendedExposureIndex";	    break;
        case(ISOSpeed):						return "ISOSpeed";					    break;
        case(ISOSpeedLatitudeyyy):			return "ISOSpeedLatitudeyyy";		    break;
        case(ISOSpeedLatitudezzz):			return "ISOSpeedLatitudezzz";		    break;
        case(ShutterSpeedValue):			return "ShutterSpeedValue";			    break;
        case(ApertureValue):				return "ApertureValue";				    break;
        case(BrightnessValue):				return "BrightnessValue";			    break;
        case(ExposureBiasValue):			return "ExposureBiasValue";			    break;
        case(MaxApertureValue):				return "MaxApertureValue";			    break;
        case(SubjectDistance):				return "SubjectDistance";			    break;
        case(MeteringMode):					return "MeteringMode";				    break;
        case(LightSource):					return "LightSource";				    break;
        case(Flash):						return "Flash";						    break;
        case(FocalLength):					return "FocalLength";				    break;
        case(SubjectArea):					return "SubjectArea";				    break;
        case(FlashEnergy):					return "FlashEnergy";				    break;
        case(SpatialFrequencyResponse):		return "SpatialFrequencyResponse";	    break;
        case(FocalPlaneXResolution):		return "FocalPlaneXResolution";		    break;
        case(FocalPlaneYResolution):		return "FocalPlaneYResolution";		    break;
        case(FocalPlaneResolutionUnit):		return "FocalPlaneResolutionUnit";	    break;
        case(SubjectLocation):				return "SubjectLocation";			    break;
        case(ExposureIndex):				return "ExposureIndex";				    break;
        case(SensingMethod):				return "SensingMethod";				    break;
        case(FileSource):					return "FileSource";					break;
        case(SceneType):					return "SceneType";					    break;
        case(CFAPattern):					return "CFAPattern";					break;
        case(CustomRendered):				return "CustomRendered";				break;
        case(ExposureMode):					return "ExposureMode";				    break;
        case(WhiteBalance):					return "WhiteBalance";				    break;
        case(DigitalZoomRatio):				return "DigitalZoomRatio";			    break;
        case(FocalLengthIn35mmFormat):		return "FocalLengthIn35mmFormat";	    break;
        case(SceneCaptureType):				return "SceneCaptureType";			    break;
        case(GainControl):					return "GainControl";				    break;
        case(Contrast):						return "Contrast";					    break;
        case(Saturation):					return "Saturation";					break;
        case(Sharpness):					return "Sharpness";					    break;
        case(DeviceSettingDescription):		return "DeviceSettingDescription";  	break;
        case(SubjectDistanceRange):			return "SubjectDistanceRange";		    break;
        case(ImageUniqueID):				return "ImageUniqueID";				    break;
        case(CameraOwnerName):				return "CameraOwnerName";			    break;
        case(BodySerialNumber):				return "BodySerialNumber";			    break;
        case(LensSpecification):			return "LensSpecification";			    break;
        case(LensMake):						return "LensMake";					    break;
        case(LensModel):					return "LensModel";					    break;
        case(LensSerialNumber):				return "LensSerialNumber";			    break;
        case(Gamma):						return "Gamma";						    break;
        case(GPSVersionID):					return "GPSVersionID";				    break;
        case(GPSLatitudeRef):				return "GPSLatitudeRef";				break;
        case(GPSLatitude):					return "GPSLatitude";				    break;
        case(GPSLongitudeRef):				return "GPSLongitudeRef";			    break;
        case(GPSLongitude):					return "GPSLongitude";				    break;
        case(GPSAltitudeRef):				return "GPSAltitudeRef";				break;
        case(GPSAltitude):					return "GPSAltitude";			    	break;
        case(GPSTimeStamp):					return "GPSTimeStamp";				    break;
        case(GPSSatellites):				return "GPSSatellites";				    break;
        case(GPSStatus):					return "GPSStatus";					    break;
        case(GPSMeasureMode):				return "GPSMeasureMode";				break;
        case(GPSDOP):						return "GPSDOP";						break;
        case(GPSSpeedRef):					return "GPSSpeedRef";			    	break;
        case(GPSSpeed):						return "GPSSpeed";					    break;
        case(GPSTrackRef):					return "GPSTrackRef";			    	break;
        case(GPSTrack):						return "GPSTrack";				    	break;
        case(GPSImgDirectionRef):			return "GPSImgDirectionRef";			break;
        case(GPSImgDirection):				return "GPSImgDirection";			    break;
        case(GPSMapDatum):					return "GPSMapDatum";			    	break;
        case(GPSDestLatitudeRef):			return "GPSDestLatitudeRef";			break;
        case(GPSDestLatitude):				return "GPSDestLatitude";			    break;
        case(GPSDestLongitudeRef):			return "GPSDestLongitudeRef";		    break;
        case(GPSDestLongitude):				return "GPSDestLongitude";			    break;
        case(GPSBearingRef):				return "GPSDestBearingRef";		    	break;
        case(GPSBearing):					return "GPSDestBearing";				break;
        case(GPSDestDistanceRef):			return "GPSDestDistanceRef";			break;
        case(GPSDestDistance):				return "GPSDestDistance";		    	break;
        case(GPSProcessingMethod):			return "GPSProcessingMethod";	    	break;
        case(GPSAreaInformation):			return "GPSAreaInformation";			break;
        case(GPSDateStamp):					return "GPSDateStamp";				    break;
        case(GPSDifferential):				return "GPSDifferential";			    break;
        case(GPSHPositioningError):			return "GPSHPositioningError";	    	break;
        default:							return "Unknown tag";				    break;
    }
}

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

            // Calculate current tag's value actual size
            int value_size = raw_table[table_index]->tags[tag_index].count *
                            getTypeSize(raw_table[table_index]->tags[tag_index].type);

            Node *new_tag = createNode(
                NULL,
                value_size, 
                getTagName(raw_table[table_index]->tags[tag_index].tag));

            new_tag->_tag_type = raw_table[table_index]->tags[tag_index].type;

            // Value is stored in ptr filed
            if (value_size <= sizeof(LONG)) {
                memcpy(new_tag->value, &raw_table[table_index]->tags[tag_index].ptr, value_size);
            } 
            // Value is stored somewhere in a file...
            else {
                int offset = (int)(raw_table[table_index]->tags[tag_index].ptr) +
                    sizeof(APP1Header) - sizeof(struct TIFF_header_s) +
                    raw_exif_data->app1_start_offset;

                //if (is_big_endian) {
                //    swabBytes(1, TYPE_LONG, &offset);
                //}

                size_t pos = ftell(data);
                fseek(data, offset, SEEK_SET);

                // Read data into buffer
                fread(new_tag->value, sizeof(char), value_size, data);

                fseek(data, pos, SEEK_SET);

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
                    for (size_t k = 0; k < value_size; k += size) {
                        swabBytes(sizeof(BYTE), type, &temp[k]);
                    }
                }
            }

            addNode(ifd_array[table_index], new_tag);
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

static void printMetadataIntoStream(ExifMetadata* metadata, char* file_name) {
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

    if (file_name) {
        freopen(file_name, "wt", stdout);
    }

    List** table = (List*)(metadata);
    for (int i = 0; i < NUMBER_OF_IFD_TABLES; i++) {

        if (table[i]->length != 0) {
            printf("%s:\n", names_of_tables[i]);
        }
        else {
            continue;
        }
        
        Node* iter = table[i]->head;
        while (iter) {
            printf("%s: ", iter->tag_name);

            int type		= iter->_tag_type;
            int count		= iter->length / getTypeSize(type);
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
                    printf("%u ", longPtr[x]);
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

    if (file_name) {
        fclose(stdout);
    }
}

void printMetadata(ExifMetadata* metadata) {
    printMetadataIntoStream(metadata, NULL);
}

void saveMetadata(ExifMetadata* metadata, char* file_name) {
    printMetadataIntoStream(metadata, file_name);
}

