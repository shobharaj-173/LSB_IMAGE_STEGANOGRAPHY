#ifndef TYPESD_H
#define TYPESD_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    d_success,//0
    d_failure//1
} Statusd;

typedef enum
{
    d_encode,//0
    d_decode,//1
    d_unsupported//2
} OperationTyped;

#endif
