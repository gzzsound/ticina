#include <stdio.h>
#include "mgos.h"
#include "common/platform.h"
#include "mgos_app.h"
#include "mgos_gpio.h"
#include "mgos_sys_config.h"
#include "mgos_wifi.h"
#include "mgos_rpc.h"
#include "mgos_mqtt.h"

#define LED_STATUS_PIN 15

static void mgos_ping(struct mg_rpc_request_info *ri,
                                    void *cb_arg, struct mg_rpc_frame_info *fi,
                                    struct mg_str args) {
  
  int id;
  int err_code = 0;
  const char *err_msg = NULL;
  
  if (json_scanf(args.p, args.len, ri->args_fmt, &id) != 1) {
    err_code = 400;
    err_msg = "id is required";
    goto out;
  }

out:
  if (err_code != 0) {
    mg_rpc_send_errorf(ri, err_code, "%s", err_msg);
  } else {
    mg_rpc_send_responsef(ri, "{pong: %d}", id);
  }

  (void)cb_arg;
  (void)fi;
}

static void net_cb(int ev, void *evd, void *arg) {
  switch (ev) {
    case MGOS_NET_EV_DISCONNECTED:
      mgos_gpio_write(LED_STATUS_PIN, false);
    break;
    case MGOS_NET_EV_IP_ACQUIRED:
      mgos_gpio_write(LED_STATUS_PIN, true);
    break;
  } 
  (void) evd;
  (void) arg;
}

enum mgos_app_init_result mgos_app_init(void) {
  struct mg_rpc *c = mgos_rpc_get_global();
  mg_rpc_add_handler(c, "Ping", "{id: %i}", mgos_ping,NULL);
  mgos_gpio_set_mode(LED_STATUS_PIN, MGOS_GPIO_MODE_OUTPUT);
  mgos_event_add_group_handler(MGOS_EVENT_GRP_NET, net_cb, NULL);
  return MGOS_APP_INIT_SUCCESS;
}
