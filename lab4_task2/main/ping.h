#ifndef __ping_h__
#define __ping_h__

#include "ping/ping_sock.h"

void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args);
void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args);
void cmd_ping_on_ping_end(esp_ping_handle_t hdl, void *args);
int do_ping_cmd(char *target_host, uint32_t interval_ms);

#endif /* __ping_h__ */
