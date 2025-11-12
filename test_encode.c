/* Name : Shobharaj H L
   Date : 12/11/2025
   Description : LSB_IMAGE_STEGANOGRAPHY */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "encode.h"
#include "decode.h"
#include "typese.h"
#include "typesd.h"
#include "common.h"


 // Function: check_operation_type
 // Purpose: To check whether user wants encode, decode or invalid
 
OperationType check_operation_type(char* argv[], int argc)
{
    if (argc < 2 || argv[1] == NULL)
        return e_unsupported;

    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}


 // Function: read_and_validate_encode_args
 // Purpose: Validate all arguments for encoding mode
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    // Need at least: ./a.out -e <source.bmp> <secret.txt>
    if (argc < 4)
        return e_failure;

    // Check .bmp source image
    if (strstr(argv[2], ".bmp") == NULL)
        return e_failure;
    encInfo->src_image_fname = argv[2];

    // Check .txt secret file
    if (strstr(argv[3], ".txt") == NULL)
        return e_failure;
    encInfo->secret_fname = argv[3];
    strcpy(encInfo->extn_secret_file, ".txt");

    // Optional output image
    if (argc == 5)
    {
        if (strstr(argv[4], ".bmp") == NULL)
            return e_failure;
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";  // default name
    }

    return e_success;
}

 //Function: read_and_validate_decode_args
 //Purpose: Validate all arguments for decoding mode
Statusd read_and_validate_decode_args(int argc, char *argv[], EncodeInfod *encInfod)
{
    // Need at least: ./a.out -d <stego.bmp>
    if (argc < 3)
        return d_failure;

    // Validate stego image
    if (strstr(argv[2], ".bmp") == NULL)
        return d_failure;
    encInfod->stego_image_fname = argv[2];

    // Try opening file
    encInfod->fptr_stego_image = fopen(encInfod->stego_image_fname, "rb");
    if (encInfod->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfod->stego_image_fname);
        return d_failure;
    }

    // Optional output filename
    if (argc == 4)
    {
        strcpy(encInfod->buf, strtok(argv[3], ".")); 
        if (!(encInfod->buf))
            return d_failure;
    }
    else
    {
        strcpy(encInfod->buf, "output");
    }

    return d_success;
}

int main(int argc, char *argv[])
{
    printf("--------------------------------------------------------------------\n");
    printf("\n\t\t✨🖼️  LSB IMAGE STEGANOGRAPHY 🔐✨\n");
    printf("\n--------------------------------------------------------------------\n");
    // Basic usage message
    if (argc < 2)
    {
        printf("❌ Invalid input.\n");
        printf("Usage:\n");
        printf("./a.out -e <source.bmp> <secret.txt> [output.bmp]\n");
        printf("./a.out -d <encoded.bmp> [output.txt]\n");
        return 1;
    }

    EncodeInfo encInfo;
    EncodeInfod encInfod;


    if (check_operation_type(argv, argc) == e_encode)
    {
        if (read_and_validate_encode_args(argc, argv, &encInfo) != e_success)
        {
            printf("❌ Invalid input for encoding.\n");
            printf("Usage: ./a.out -e <source.bmp> <secret.txt> [output.bmp]\n");
            return e_failure;
        }

        if (do_encoding(&encInfo) == e_success)
            printf("✅ Encoded the given data into image successfully!\n");
        else
            printf("❌ Encoding failed.\n");
    }
    else if (check_operation_type(argv, argc) == e_decode)
    {
        if (read_and_validate_decode_args(argc, argv, &encInfod) != d_success)
        {
            printf("❌ Invalid input for decoding.\n");
            printf("Usage: ./a.out -d <encodedfile.bmp> [output.txt]\n");
            return d_failure;
        }

        if (do_decoding(&encInfod) == d_success)
            printf("✅ Decoded the given data from image successfully!\n");
        else
            printf("❌ Decoding failed.\n");
    }
    else
    {
        printf("❌ Invalid operation type !!!\n");
        printf("Usage:\n");
        printf("./a.out -e <source.bmp> <secret.txt> [output.bmp]\n");
        printf("./a.out -d <encodedfile.bmp> [output.txt]\n");
        return 1;
    }

    return 0;
}
