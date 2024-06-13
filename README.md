# Simple ExifTool

Simple ExifTool is a library for working with EXIF metadata in images. The library allows you to extract, remove and save EXIF metadata from images.

## Features

- Extract EXIF metadata from images.
- Print EXIF metadata.
- Save EXIF metadata to a file.
- Remove EXIF metadata from images.

## Usage

### Including the Library

```c
#include "exif_tool.h"
```

### API Functions

#### `ExifMetadata* getExifMetadata(char* file_name);`

This function extracts EXIF metadata from the specified image file.

**Parameters:**
- `file_name`: The name of the image file.

**Returns:**
- A pointer to an `ExifMetadata` structure containing the metadata.

#### `void freeExifMetadata(ExifMetadata *metadata);`

This function frees the memory allocated for the `ExifMetadata` structure.

**Parameters:**
- `metadata`: A pointer to the `ExifMetadata` structure.

#### `void printMetadata(ExifMetadata* metadata);`

This function prints the EXIF metadata to the console.

**Parameters:**
- `metadata`: A pointer to the `ExifMetadata` structure.

#### `void saveMetadata(ExifMetadata* metadata, char* file_name);`

This function saves the EXIF metadata to the specified file.

**Parameters:**
- `metadata`: A pointer to the `ExifMetadata` structure.
- `file_name`: The name of the file to save the metadata to.

#### `int removeEXIFFromJPEG(char* file_name, char* new_file_name);`

This function removes EXIF metadata from a JPEG image and saves the result to a new file.

**Parameters:**
- `file_name`: The name of the original JPEG file.
- `new_file_name`: The name of the new JPEG file without EXIF metadata.

**Returns:**
- `0` on success, or a non-zero error code on failure.

### Example Usage

```c
#include <stdio.h>
#include "exif_tools.h"

int main() {
    char* file_name = "example.jpg";

    // Extract metadata from the image
    ExifMetadata* metadata = getExifMetadata(file_name);
    if (metadata == NULL) {
        printf("Failed to extract metadata.\n");
        return 1;
    }

    // Print metadata
    printMetadata(metadata);

    // Save metadata to a file
    saveMetadata(metadata, "metadata_output.txt");

    // Remove EXIF metadata from JPEG image
    if (removeEXIFFromJPEG(file_name, "example_no_exif.jpg") != 0) {
        printf("Failed to remove EXIF metadata.\n");
    }

    // Free all allocated memory
    freeExifMetadata(metadata);

    return 0;
}
```

## Requirements

- C Compiler (e.g., GCC)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
