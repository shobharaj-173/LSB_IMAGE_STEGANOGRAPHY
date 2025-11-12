# LSB_Image_Steganography
## Overview
A command-line tool for hiding secret messages within BMP images using Least Significant Bit (LSB) steganography technique.
## Features
* Embed secret files of any type into BMP images
* Extract hidden messages from steganographic images
* Supports magic string detection
* Validates image capacity before embedding
* Simple command-line interface
## Usage
### Encoding
```sh
./lsb_steg -e <source.bmp> <secret.txt> [output.bmp]
```
### Decoding
```sh
./lsb_steg -d <steged_image.bmp> [output_file]
```
## Requirements
* C compiler (GCC recommended)
* BMP image files
* Any file type to hide (e.g., .txt, .mp3, .mp4, .pdf)
## Build Instructions
```sh
gcc MAIN_test_encode.c encode.c decode.c -o lsb_steg
```
## How It Works
- Modifies least significant bits of image pixels
- Embeds file extension, size, and data
- Uses a magic string for verification
- Preserves image visual integrity
## Technical Details
### Encoding Process
1. Check Image Capacity
2. Copy BMP Header
3. Embed Magic String
4. Encode File Extension Size
5. Encode File Extension
6. Encode File Size
7. Encode File Data
### Decoding Process
1. Open Steganographic Image
2. Verify Magic String
3. Extract File Extension Size
4. Extract File Extension
5. Create secret_file using decoded extension
6. Extract File Size
7. Extract File Data
## Security Note
Not cryptographically secure. Use for educational purposes only.
## Example
```sh
# Encode a secret message
./lsb_steg -e image.bmp secret_file steged_image.bmp

# Decode the hidden message
./lsb_steg -d steged_image.bmp decoded_secret_file
```
## License
Open-source project
## Contributions
Contributions are welcome! Please submit pull requests or open issues.
