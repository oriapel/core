#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <inttypes.h> // for uint32_t

enum ErrorCode : uint32_t
{
    ERR_OK = 0,
    ERR_GENERAL_ERROR = 1,
    ERR_DATA_NOT_WRITTEN = 2,
    ERR_DATA_CORRUPTED = 3,
    ERR_FILE_PROBLEM = 4
};

#endif // ERROR_CODES_H