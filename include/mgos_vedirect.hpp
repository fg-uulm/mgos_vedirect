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

VeDirectFrameHandler fHandler;

void mgos_vedirect_create();
void mgos_vedirect_destroy();

#ifdef __cplusplus
}
#endif
