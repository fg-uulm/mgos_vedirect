#pragma once

#include "mgos.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <VEDirectFrameHandler.hpp>

#ifdef __cplusplus
extern "C" {
#endif

struct mgos_vedirect {
  uint8_t uart_no;
  bool connected;
};

struct mgos_vedirect_cb {
    void (*callback)(char*, void *);
    void *ud;
};

VeDirectFrameHandler fHandler;

void mgos_vedirect_create();
void mgos_vedirect_destroy();
char* mgos_vedirect_read();
void mgos_vedirect_print();

#ifdef __cplusplus
}
#endif
