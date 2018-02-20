/**
 * Copyright (c) 2009 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/** @file
* @brief Example template project.
* @defgroup nrf_templates_example Example Template
*
*/

#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nordic_common.h"
#include "boards.h"
#include "nrf_delay.h"
#include "app_mpu.h"
#include "nrf_drv_gpiote.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define MPU_MPU_INT_PIN     30

volatile bool mpu_data_ready = false;


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


void mpu_init(void)
{
    uint32_t err_code;
    // Initiate MPU driver
    err_code = app_mpu_init();
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    // Setup and configure the MPU with intial values
    app_mpu_config_t p_mpu_config = MPU_DEFAULT_CONFIG(); // Load default values
    p_mpu_config.smplrt_div = 199;   // Change sampelrate. Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV). 199 gives a sample rate of 5Hz
    p_mpu_config.accel_config.afs_sel = AFS_2G; // Set accelerometer full scale range to 2G
    err_code = app_mpu_config(&p_mpu_config); // Configure the MPU with above values
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    
    // This is a way to configure the interrupt pin behaviour
    app_mpu_int_pin_cfg_t p_int_pin_cfg = MPU_DEFAULT_INT_PIN_CONFIG(); // Default configurations
    p_int_pin_cfg.int_rd_clear = 1; // When this bit is equal to 1, interrupt status bits are cleared on any read operation
    err_code = app_mpu_int_cfg_pin(&p_int_pin_cfg); // Configure pin behaviour
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    // Enable the MPU interrupts
    app_mpu_int_enable_t p_int_enable = MPU_DEFAULT_INT_ENABLE_CONFIG();
    p_int_enable.data_rdy_en = 1; // Trigger interrupt everytime new sensor values are available
    err_code = app_mpu_int_enable(&p_int_enable); // Configure interrupts
    APP_ERROR_CHECK(err_code); // Check for errors in return value    
}

/**
 * @brief Simple interrupt handler setting a flag indicating that data is ready
 *
 */
void int_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    mpu_data_ready = true;
}

/**
 * @brief Function for initiating the GPIOTE module and enable the 
 * nRF5 to trigger an interrupt on a Low-To-High event on pin MPU_MPU_INT_PIN
 *
 */
static void gpiote_init(void)
{
    uint32_t err_code;
    if (!nrf_drv_gpiote_is_init())
    {
        err_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }
    
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

    err_code = nrf_drv_gpiote_in_init(MPU_MPU_INT_PIN, &in_config, int_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(MPU_MPU_INT_PIN, true);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;
    
    // Initialize.
    log_init();
	NRF_LOG_INFO("\033[2J\033[;H"); // Clear screen
    
    gpiote_init();
    mpu_init();
    
    // Start execution.
    NRF_LOG_INFO("MPU Data Ready example.");
    
    accel_values_t acc_values;
    uint32_t sample_number = 0;
        
    while(1)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            while(mpu_data_ready != true)
            {
                // Make sure any pending events are cleared
                __SEV();
                __WFE();
                // Enter System ON sleep mode
                __WFE();          
            }
         
            err_code = app_mpu_read_accel(&acc_values);
            APP_ERROR_CHECK(err_code);
            // Clear terminal and print values
            NRF_LOG_INFO("\033[2J\033[;HSample # %d\r\nX: %06d\r\nY: %06d\r\nZ: %06d", ++sample_number, acc_values.x, acc_values.y, acc_values.z);
            mpu_data_ready = false;
        }
    }
}
/** @} */
