#ifndef __BLE_HRM_H
#define __BLE_HRM_H

#include "esp_event_base.h"
#include "esp_types.h"
#include "esp_event.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "esp_gattc_api.h"

#include "event_common.h"

#define HEART_RATE_MEASUREMENT_CHARACTERISTIC 0x2A37
#define HR_SENSOR_LOCATION_CHARACTERISTIC_UUID 0x2A38

extern esp_event_loop_handle_t event_loop_handle;

ESP_EVENT_DECLARE_BASE(BIKE_BLE_HRM_SENSOR_EVENT);

static struct csc_measure_sensor {
    uint16_t heart_rate;
} hr_measure_sensor_t;

void ble_parse_hrm_data(char *device_name, esp_ble_gattc_cb_param_t *p_data);

#endif