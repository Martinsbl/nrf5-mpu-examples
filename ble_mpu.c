/* Copyright (c) Nordic Semiconductor ASA
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *   1. Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 *   2. Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of other
 *   contributors to this software may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 * 
 *   4. This software must only be used in a processor manufactured by Nordic
 *   Semiconductor ASA, or in a processor manufactured by a third party that
 *   is used in combination with a processor manufactured by Nordic Semiconductor.
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "ble_mpu.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "mpu.h"


void ble_mpu_on_ble_evt(ble_mpu_t * p_mpu, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            p_mpu->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            p_mpu->conn_handle = BLE_CONN_HANDLE_INVALID;
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for adding our new characterstic to "Our service" that we initiated in the previous tutorial. 
 *
 * @param[in]   p_mpu        mpu structure.
 *
 */
static uint32_t ble_char_accel_add(ble_mpu_t * p_mpu)
{
    uint32_t   err_code = 0; // Variable to hold return codes from library and softdevice functions
    
    ble_uuid_t          char_uuid;
    // WATING FOR BUG FIX IN SDK 11 PRODUCTION VERSION TO BE ABLE TO CALL sd_ble_uuid_vs_add TWICE
//    ble_uuid128_t       base_uuid = BLE_UUID_OUR_BASE_UUID;
//    char_uuid.uuid = BLE_UUID_OUR_CHARACTERISTC_UUID;
//    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
//    APP_ERROR_CHECK(err_code);   
    BLE_UUID_BLE_ASSIGN(char_uuid, BLE_UUID_ACCEL_CHARACTERISTC_UUID);
    
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;
    
    ble_gatts_attr_md_t cccd_md;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;
        
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;    
    
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;
    attr_char_value.max_len     = sizeof(accel_values_t);
    attr_char_value.init_len    = sizeof(accel_values_t);
    uint8_t value[6]            = {0};
    attr_char_value.p_value     = value;

    err_code = sd_ble_gatts_characteristic_add(p_mpu->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_mpu->accel_char_handles);
    APP_ERROR_CHECK(err_code);   

    return NRF_SUCCESS;
}


/**@brief Function for initiating our new service.
 *
 * @param[in]   p_mpu        Our Service structure.
 *
 */
void ble_mpu_service_init(ble_mpu_t * p_mpu)
{
    uint32_t   err_code; // Variable to hold return codes from library and softdevice functions

    ble_uuid_t        service_uuid;
    ble_uuid128_t     base_uuid = BLE_UUID_BASE_UUID;
    service_uuid.uuid = BLE_UUID_MPU_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);    

    p_mpu->conn_handle = BLE_CONN_HANDLE_INVALID;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &p_mpu->service_handle);
    
    APP_ERROR_CHECK(err_code);
    
    ble_char_accel_add(p_mpu);
}

// ALREADY_DONE_FOR_YOU: Function to be called when updating characteristic value
void ble_mpu_update(ble_mpu_t *p_mpu, accel_values_t * accel_values)
{
    // Send value if connected and notifying
    if (p_mpu->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t               len = sizeof(accel_values_t);
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_mpu->accel_char_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)accel_values;  

        sd_ble_gatts_hvx(p_mpu->conn_handle, &hvx_params);
    } 
    
}
