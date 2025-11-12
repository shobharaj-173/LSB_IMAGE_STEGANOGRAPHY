/* Name : Shobharaj H L
   Date : 12/11/2025
   Description : LSB_IMAGE_STEGANOGRAPHY */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "encode.h"
#include "typese.h"
#include "common.h"

/* ---------------------------------------------------------------
 * 🧠 Function: Get image size (for BMP)
 * --------------------------------------------------------------- */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    // Seek to 18th byte where width starts
    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, sizeof(int), 1, fptr_image);
    fread(&height, sizeof(int), 1, fptr_image);

    printf("🖼️  Image Dimensions: width = %u, height = %u\n", width, height);

    rewind(fptr_image); // Reset file pointer
    return width * height * 3; // 3 bytes per pixel (RGB)
}

/* ---------------------------------------------------------------
 * 📂 Open source, secret, and stego files
 * --------------------------------------------------------------- */
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "❌ ERROR: Unable to open image file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "❌ ERROR: Unable to open secret file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "❌ ERROR: Unable to open output file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    printf("📂 Files opened successfully!\n");
    return e_success;
}

/* ---------------------------------------------------------------
 * 📏 Get size of any file
 * --------------------------------------------------------------- */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    rewind(fptr);
    printf("📄 Secret file size: %ld bytes\n", size);
    return size;
}

/* ---------------------------------------------------------------
 * 🧮 Check if image has enough capacity
 * --------------------------------------------------------------- */
Status check_capacity(EncodeInfo* encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if (encInfo->image_capacity >= 54 + (2 + 4 + 4 + 4 + (encInfo->size_secret_file)) * 8)
    {
        printf("✅ Image has sufficient capacity to store secret data.\n");
        return e_success;
    }
    else
    {
        printf("⚠️  ERROR: Image does not have enough capacity!\n");
        return e_failure;
    }
}

/* ---------------------------------------------------------------
 * 🧾 Copy BMP Header (first 54 bytes)
 * --------------------------------------------------------------- */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];
    if (fread(header, 1, 54, fptr_src_image) != 54)
        return e_failure;

    if (fwrite(header, 1, 54, fptr_dest_image) != 54)
        return e_failure;

    printf("📑 BMP header copied successfully!\n");
    return e_success;
}

/* ---------------------------------------------------------------
 * ✨ Encode Magic String
 * --------------------------------------------------------------- */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if (encode_data_to_image(MAGIC_STRING, strlen(MAGIC_STRING), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("🔮 Magic string encoded successfully!\n");
        return e_success;
    }
    return e_failure;
}

/* ---------------------------------------------------------------
 * 🧩 Encode generic data into image
 * --------------------------------------------------------------- */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for (int i = 0; i < size; i++)
    {
        if (fread(image_buffer, 1, 8, fptr_src_image) != 8) return e_failure;
        encode_byte_to_lsb(data[i], image_buffer);
        if (fwrite(image_buffer, 1, 8, fptr_stego_image) != 8) return e_failure;
    }
    return e_success;
}

/* ---------------------------------------------------------------
 * 🔢 Encode a single byte to image’s LSB
 * --------------------------------------------------------------- */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
        image_buffer[i] = ((image_buffer[i] & ~1) | ((data >> (7 - i)) & 1));
    return e_success;
}

/* ---------------------------------------------------------------
 * 🧮 Encode Secret File Extension Size
 * --------------------------------------------------------------- */
Status encode_secret_file_extn_size(const char *file_extn, EncodeInfo *encInfo)
{
    char image_buffer[32];
    if (fread(image_buffer, 1, 32, encInfo->fptr_src_image))
    {
        encode_size_to_lsb(strlen(file_extn), image_buffer);
        fwrite(image_buffer, 1, 32, encInfo->fptr_stego_image);
        printf("📏 Encoded extension size successfully!\n");
        return e_success;
    }
    return e_failure;
}

/* ---------------------------------------------------------------
 * 🔠 Encode integer size into 32 LSBs
 * --------------------------------------------------------------- */
Status encode_size_to_lsb(int size, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
        image_buffer[i] = ((image_buffer[i] & ~1) | ((size >> (31 - i)) & 1));
    return e_success;
}

/* ---------------------------------------------------------------
 * 🧩 Encode Secret File Extension (.txt, .c, etc.)
 * --------------------------------------------------------------- */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if (encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("📎 Encoded file extension successfully!\n");
        return e_success;
    }
    return e_failure;
}

/* ---------------------------------------------------------------
 * 🧮 Encode Secret File Size
 * --------------------------------------------------------------- */ 
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buffer[32];
    if (fread(image_buffer, 1, 32, encInfo->fptr_src_image) != 32) return e_failure;
    encode_size_to_lsb(file_size, image_buffer);
    fwrite(image_buffer, 1, 32, encInfo->fptr_stego_image);
    printf("📦 Encoded secret file size successfully!\n");
    return e_success;
}

/* ---------------------------------------------------------------
 * 🧠 Encode Secret File Data
 * --------------------------------------------------------------- */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char *buffer = malloc(encInfo->size_secret_file);
    if (!buffer) return e_failure;

    size_t bytes_read = fread(buffer, 1, encInfo->size_secret_file, encInfo->fptr_secret);
    if (bytes_read != encInfo->size_secret_file)
    {
        free(buffer);
        return e_failure;
    }

    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        char image_bytes[8];
        fread(image_bytes, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(buffer[i], image_bytes);
        fwrite(image_bytes, 1, 8, encInfo->fptr_stego_image);
    }

    free(buffer);
    printf("🗝️  Encoded secret file data successfully!\n");
    return e_success;
}

/* ---------------------------------------------------------------
 * 🖋️ Copy remaining data from source to output image
 * --------------------------------------------------------------- */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src) == 1)
        fwrite(&ch, 1, 1, fptr_dest);

    printf("🖋️  Remaining image data copied successfully!\n");
    return e_success;
}

/* ---------------------------------------------------------------
 * 🚀 Main Encoding Process
 * --------------------------------------------------------------- */
Status do_encoding(EncodeInfo *encInfo)
{
    printf("\n🚀 Starting Encoding Process...\n");

    if (open_files(encInfo) != e_success) return e_failure;
    if (check_capacity(encInfo) != e_success) return e_failure;
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success) return e_failure;
    if (encode_magic_string(MAGIC_STRING, encInfo) != e_success) return e_failure;
    if (encode_secret_file_extn_size(encInfo->extn_secret_file, encInfo) != e_success) return e_failure;
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) != e_success) return e_failure;
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) != e_success) return e_failure;
    if (encode_secret_file_data(encInfo) != e_success) return e_failure;
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success) return e_failure;

    printf("\n✅ Encoding completed successfully! Secret data embedded into image.\n");
    return e_success;
}
