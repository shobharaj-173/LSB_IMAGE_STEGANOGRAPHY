/* Name : Shobharaj H L
   Date : 12/11/2025
   Description : LSB_IMAGE_STEGANOGRAPHY */
#ifndef DECODE_H
#define DECODE_H

#include "typesd.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 5

typedef struct EncodeInfod
{
    /* Source Image info */
    char *src_image_fname;//To store the beautiful.bmp
    FILE *fptr_src_image;//to hold the beautiful.bmp offset
    uint image_capacity;//to store the size
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];//to store the readed 8 bytes

    /* Secret File Info */
    
    char extn_secret_file[MAX_FILE_SUFFIX];//to store extension of secret file
    long size_secret_file;//to store secret file size

    /* Stego Image Info */
    char *stego_image_fname;//to store o/p file name
    FILE *fptr_stego_image;//to store o/p file offset(address)

    /*OUTPUT */
     char buf[256];//to store output filename
    FILE *out_fptr;// output file pointer
} EncodeInfod;


/* Encoding function prototype */

/* Check operation type */
OperationTyped check_operation_type_d(char *argv[],int argc);

/* Read and validate Encode args from argv */
Statusd read_and_validate_decode_args(int argc,char *argv[], EncodeInfod *encInfod);

/* Perform the encoding */
Statusd do_decoding(EncodeInfod *encInfod);


/* Store Magic String */
Statusd decode_magic_string(char *magic_string, EncodeInfod *encInfod);

/* Encode secret file extenstion */
Statusd decode_extn(int *size, EncodeInfod *encInfod);

/* Encode secret file size */
Statusd decode_secret_file_size(long *file_size, EncodeInfod *encInfod);

/* Encode secret file data*/
Statusd decode_secret_file_data(EncodeInfod *encInfod);

/* Encode function, which does the real encoding */
Statusd decode_data_from_image( char *data, int size, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Statusd decode_byte_from_lsb(char *data, char *image_buffer);

/*Encode the extension size of the secret file*/
Statusd decode_extn_size(int *size, EncodeInfod *encInfod);

/*encode the byte of size to lsb*/
Statusd decode_size_from_lsb(int *size,char *image_buffer);

#endif
