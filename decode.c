/* Name : Shobharaj H L
   Date : 12/11/2025
   Description : LSB_IMAGE_STEGANOGRAPHY */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "decode.h"
#include "typesd.h"
#include "common.h"

//Function: decode_byte_from_lsb
 //Purpose : Decode a single character from 8 LSBs in image buffer
 
Statusd decode_byte_from_lsb(char *d_data, char *image_buffer)
{
    unsigned char ch = 0;
    for (int i = 0; i < 8; i++)
    {
        ch = (ch << 1) | (image_buffer[i] & 1); // extract LSB
    }

    *d_data = ch;
    return d_success;
}

 //Function: decode_data_from_image
 //Purpose : Decode a string of given size from image
 
Statusd decode_data_from_image(char *data, int size, FILE *fptr_stego_image)
{
    char image_buffer[8];

    for (int i = 0; i < size; i++)
    {
        if (fread(image_buffer, 1, 8, fptr_stego_image) != 8)
        {
            fprintf(stderr, "❌ ERROR: Unable to read 8 bytes from image\n");
            return d_failure;
        }

        if (decode_byte_from_lsb(&data[i], image_buffer) != d_success)
            return d_failure;
    }

    data[size] = '\0'; // Null terminate
    return d_success;
}

//Function: decode_magic_string
 //Purpose : Validate if hidden magic string is present

Statusd decode_magic_string(char *magic_string, EncodeInfod *encInfod)
{
    if (decode_data_from_image(magic_string, strlen(MAGIC_STRING), encInfod->fptr_stego_image) != d_success)
        return d_failure;

    if (strcmp(MAGIC_STRING, magic_string) != 0)
        return d_failure;

    printf("🪄 Magic string verified: %s\n", magic_string);
    return d_success;
}

// Function: decode_size_from_lsb
 // Purpose : Decode integer size (32 bits) from 32 bytes
Statusd decode_size_from_lsb(int *size, char *image_buffer)
{
    *size = 0;
    for (int i = 0; i < 32; i++)
        *size = (*size << 1) | (image_buffer[i] & 1);

    return d_success;
}

 //Function: decode_extn_size
 //Purpose : Decode size of file extension
Statusd decode_extn_size(int *size, EncodeInfod *encInfod)
{
    char image_buffer[32];

    if (fread(image_buffer, 1, 32, encInfod->fptr_stego_image) != 32)
    {
        fprintf(stderr, "❌ ERROR: Unable to read 32 bytes for extension size\n");
        return d_failure;
    }

    if (decode_size_from_lsb(size, image_buffer) != d_success)
        return d_failure;

    printf("📏 Decoded extension size: %d bytes\n", *size);
    return d_success;
}


 //Function: decode_extn
 // Purpose : Decode and prepare output filename
Statusd decode_extn(int *size, EncodeInfod *encInfod)
{
    char extn[*size + 1];

    if (decode_data_from_image(extn, *size, encInfod->fptr_stego_image) != d_success)
        return d_failure;

    extn[*size] = '\0';
    strcpy(encInfod->extn_secret_file, extn);

    if (encInfod->buf != NULL)
        strcat(encInfod->buf, extn);

    encInfod->out_fptr = fopen(encInfod->buf, "wb");
    if (encInfod->out_fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "❌ ERROR: Unable to open file %s\n", encInfod->buf);
        return d_failure;
    }

    printf("📂 Decoded file extension: %s\n", extn);
    printf("💾 Output file created: %s\n", encInfod->buf);
    return d_success;
}


 //Function: decode_secret_file_size
 // Purpose : Decode size of hidden secret file
Statusd decode_secret_file_size(long *file_size, EncodeInfod *encInfod)
{
    char image_buffer[32];

    if (fread(image_buffer, 1, 32, encInfod->fptr_stego_image) != 32)
    {
        fprintf(stderr, "❌ ERROR: Unable to read 32 bytes for secret file size\n");
        return d_failure;
    }

    int temp_size = 0;
    if (decode_size_from_lsb(&temp_size, image_buffer) != d_success)
        return d_failure;

    *file_size = temp_size;
    printf("📦 Decoded secret file size: %ld bytes\n", *file_size);
    return d_success;
}

//Function: decode_secret_file_data
 // Purpose : Decode actual hidden content into output file
 
Statusd decode_secret_file_data(EncodeInfod *encInfod)
{
    long size = encInfod->size_secret_file;
    char *secret_data = malloc(size);
    if (!secret_data)
    {
        fprintf(stderr, "❌ ERROR: Memory allocation failed\n");
        return d_failure;
    }

    char image_buffer[8];

    for (long i = 0; i < size; i++)
    {
        if (fread(image_buffer, 1, 8, encInfod->fptr_stego_image) != 8)
        {
            fprintf(stderr, "❌ ERROR: Unable to read 8 bytes from image\n");
            free(secret_data);
            return d_failure;
        }

        if (decode_byte_from_lsb(&secret_data[i], image_buffer) != d_success)
        {
            free(secret_data);
            return d_failure;
        }
    }

    if (fwrite(secret_data, 1, size, encInfod->out_fptr) != size)
    {
        fprintf(stderr, "❌ ERROR: Unable to write all decoded data to output file\n");
        free(secret_data);
        return d_failure;
    }

    fclose(encInfod->out_fptr);
    free(secret_data);

    printf("✅ Secret file data successfully decoded and written!\n");
    return d_success;
}


Statusd do_decoding(EncodeInfod *encInfod)
{
    printf("\n🔍 Starting decoding process...\n");
    fseek(encInfod->fptr_stego_image, 54, SEEK_SET); // Skip BMP header

    char magic_string[strlen(MAGIC_STRING) + 1];

    if (decode_magic_string(magic_string, encInfod) != d_success)
    {
        printf("❌ Magic string verification failed! Not a valid stego image.\n");
        return d_failure;
    }

    int extn_size;
    if (decode_extn_size(&extn_size, encInfod) != d_success)
        return d_failure;

    if (decode_extn(&extn_size, encInfod) != d_success)
        return d_failure;

    if (decode_secret_file_size(&encInfod->size_secret_file, encInfod) != d_success)
        return d_failure;

    if (decode_secret_file_data(encInfod) != d_success)
        return d_failure;

    printf("🎉 Decoding completed successfully!\n");
    printf("------------------------------------------\n");
    printf("📁 Output File: %s\n", encInfod->buf);
    printf("📄 File Size  : %ld bytes\n", encInfod->size_secret_file);
    printf("------------------------------------------\n");

    return d_success;
}
