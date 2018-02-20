 /* 
  * This code is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef MPU_SERVICE_H__
#define MPU_SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "app_mpu.h"

#define BLE_UUID_BASE_UUID              {0x23, 0xD1, 0x13, 0xEF, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00} // 128-bit base UUID
#define BLE_UUID_MPU_SERVICE_UUID                0xF00D // Just a random, but recognizable value

#define BLE_UUID_ACCEL_CHARACTERISTC_UUID          0xACCE // Just a random, but recognizable value

typedef struct
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of ble Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    accel_char_handles;   /**< Handles related to the our new characteristic. */
    bool                        is_notification_enabled;
}ble_mpu_t;

/**@brief Function for handling BLE Stack events related to mpu service and characteristic.
 *
 * @details Handles all events from the BLE stack of interest to mpu Service.
 *
 * @param[in]   p_mpu       mpu structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_mpu_on_ble_evt(ble_mpu_t * p_mpu, ble_evt_t const * p_ble_evt);

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_mpu       Pointer to ble mpu structure.
 */
void ble_mpu_service_init(ble_mpu_t * p_mpu);

/**@brief Function for updating and sending new characteristic values
 *
 * @details The application calls this function whenever our timer_timeout_handler triggers
 *
 * @param[in]   p_mpu                     mpu structure.
 * @param[in]   characteristic_value     New characteristic value.
 */
uint32_t ble_mpu_update(ble_mpu_t *p_mpu, accel_values_t * accel_values);

#endif  /* _ MPU_SERVICE_H__ */
