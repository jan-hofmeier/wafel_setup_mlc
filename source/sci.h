#pragma once

#include <wafel/types.h>

enum {
    UC_DATA_TYPE_U8      = 1,
    UC_DATA_TYPE_U16     = 2,
    UC_DATA_TYPE_U32     = 3,
    UC_DATA_TYPE_I32     = 4,
    UC_DATA_TYPE_F32     = 5,
    UC_DATA_TYPE_STRING  = 6,
    UC_DATA_TYPE_BINARY  = 7,
    UC_DATA_TYPE_COMPLEX = 8,
};

typedef struct __attribute__((__packed__)) {
    char name[64];
    uint32_t access;
    uint32_t data_type;
    int error;
    uint32_t data_size;
    void* data;
} UCSysConfig_t;


int SCISetParentalEnable(uint8_t enable);

int SCIGetParentalEnable(uint8_t* outEnable);

int SCIGetParentalPinCode(char* pin, uint32_t pin_size);

int SCIGetParentalCustomSecQuestion(char* buf, uint32_t buf_size);

int SCIGetParentalSecAnswer(char* buf, uint32_t buf_size);

int SCISetInitialLaunch(uint8_t param_1);
