/*
 * Copyright 2020 d4rkmen <darkmen@i.ua>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mgos.h"
#include "mgos_vedirect.hpp"
#include "mgos_rpc.h"
#include "mgos_uart.h"

static struct mgos_vedirect *ved = NULL;
static constexpr char checksumTagName[] = "CHECKSUM";

void bin_to_hex(char *to, const unsigned char *p, size_t len) {
  static const char *hex = "0123456789abcdef";

  for (; len--; p++) {
    *to++ = hex[p[0] >> 4];
    *to++ = hex[p[0] & 0x0f];
    if (len) *to++ = ' ';
  }
  *to = '\0';
}

static uint8_t mgos_vedirect_packet_crc(uint8_t *data, uint8_t size) {
  uint8_t crc = 0;
  for (int i = 0; i < size; i++) crc += data[i];
  return (0xFC - crc);
}

void mgos_vedirect_packet_read() {
  // Handle packet read timeout
  int64_t now = mgos_uptime_micros();
  
  size_t rx_count = mgos_uart_read_avail(ved->uart_no);

  uint8_t data;

  while (rx_count) {
    size_t n = mgos_uart_read(ved->uart_no, &data, 1);
    if (n != 1)  // read error?
      return;
    fHandler.rxData(data);
    rx_count--;
  }
}

static void mgos_vedirect_uart_dispatcher(int uart_no, void *arg) {
  assert(uart_no == ved->uart_no);
  if (mgos_uart_read_avail(uart_no) == 0) return;
  mgos_vedirect_packet_read();
}

void mgos_vedirect_create() {
  LOG(LL_DEBUG, ("Creating VE.Direct object..."));
  struct mgos_uart_config ucfg;
  ved = (mgos_vedirect*)calloc(1, sizeof(struct mgos_vedirect));
  if (!ved) return;
  
  // Init all the structure members
  memset((void *) ved, 0, sizeof(struct mgos_vedirect));

  ved->uart_no = mgos_sys_config_get_vedirect_uart_no();

  // Initialize UART
  mgos_uart_config_set_defaults(ved->uart_no, &ucfg);
  ucfg.baud_rate = mgos_sys_config_get_vedirect_uart_baud_rate();
  ucfg.num_data_bits = 8;
  ucfg.parity = MGOS_UART_PARITY_EVEN;
  ucfg.stop_bits = MGOS_UART_STOP_BITS_1;
  ucfg.rx_buf_size = 256;
  ucfg.tx_buf_size = 32;
  if (!mgos_uart_configure(ved->uart_no, &ucfg)) goto err;
  mgos_uart_set_dispatcher(ved->uart_no, mgos_vedirect_uart_dispatcher,
                           (void *) ved);
  mgos_uart_set_rx_enabled(ved->uart_no, true);

  LOG(LL_INFO, ("UART%d initialized %u,%d%c%d", ved->uart_no, ucfg.baud_rate,
                ucfg.num_data_bits,
                ucfg.parity == MGOS_UART_PARITY_NONE ? 'N' : ucfg.parity + '0',
                ucfg.stop_bits));

  LOG(LL_INFO, ("VEDirect service running"));

  return;
err:
  LOG(LL_INFO, ("VEDirect service start fail"));
  free(ved);
  return;
}

void mgos_vedirect_destroy() {
  LOG(LL_DEBUG, ("Destroing Ve.Direct object...."));
  mgos_uart_set_rx_enabled(ved->uart_no, false);
  return;
}

void mgos_vedirect_get_params(struct mgos_mel_ac_params *params) {
  // if (!mel || !params) return;
  // *params = mel->params;
}

// RPC handlers
static void get_params_handler(struct mg_rpc_request_info *ri, void *cb_arg,
                               struct mg_rpc_frame_info *fi,
                               struct mg_str args) {
  // struct mbuf fb;
  // struct json_out out = JSON_OUT_MBUF(&fb);

  // mbuf_init(&fb, MGOS_MEL_AC_JSON_SIZE);

  // struct mgos_mel_ac_params params;
  // bool connected = mgos_mel_ac_get_connected();
  // bool operating = mgos_mel_ac_get_operating();
  // float room = mgos_mel_ac_get_room_temperature();
  // mgos_vedirect_get_params(&params);

  // // json_printf(
  // //     &out,
  // //     "{connected: %B, power: %d, mode: %d, setpoint: %.1f, fan: %d, "
  // //     "vane_vert: %d, vane_horiz: %d, isee: %B, operating: %B, room: %.1f}",
  // //     connected, params.power, params.mode, params.setpoint, params.fan,
  // //     params.vane_vert, params.vane_horiz, params.isee, operating, room);

  // mg_rpc_send_responsef(ri, "%.*s", fb.len, fb.buf);
  // ri = NULL;

  // mbuf_free(&fb);

  (void) cb_arg;
  (void) fi;
}

bool mgos_vedirect_init(void) {
  if (mgos_sys_config_get_vedirect_enable()) {
    mgos_vedirect_create();

    if (mgos_sys_config_get_vedirect_rpc_enable()) {
      struct mg_rpc *c = mgos_rpc_get_global();
      mg_rpc_add_handler(c, "VeDirect.GetParams", "{}", get_params_handler, NULL);     
    }
  }
  return true;
}

void mgos_vedirect_deinit(void) {
  mgos_vedirect_destroy();
}
