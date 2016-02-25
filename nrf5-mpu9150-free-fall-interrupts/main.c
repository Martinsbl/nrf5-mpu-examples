/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_uart.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "mpu9150.h"
#include "mpu9150_register_map.h"
#include "nrf_drv_gpiote.h"

/*Pins to connect MPU. */
#define MPU9150_TWI_SCL_PIN     1
#define MPU9150_TWI_SDA_PIN     2
#define MPU9150_MPU_INT_PIN     3

/*UART buffer size. */
#define UART_TX_BUF_SIZE 256
#define UART_RX_BUF_SIZE 1

static const nrf_drv_twi_t m_twi_instance = NRF_DRV_TWI_INSTANCE(0);
volatile bool mpu_free_fall_event = false;

/**
 * @brief UART events handler.
 */
static void uart_events_handler(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type)
    {
        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}


/**
 * @brief UART initialization.
 * Just the usual way. Nothing special here
 */
static void uart_config(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_events_handler,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    APP_ERROR_CHECK(err_code);
}


/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{   
    // Pass TWI events down to the MPU driver.
    mpu9150_twi_event_handler(p_event);
}

/**
 * @brief TWI initialization.
 * Just the usual way. Nothing special here
 */
void twi_init(void)
{
    uint32_t err_code;
    
    const nrf_drv_twi_config_t twi_mpu_9150_config = {
       .scl                = MPU9150_TWI_SCL_PIN,
       .sda                = MPU9150_TWI_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH
    };
    
    err_code = nrf_drv_twi_init(&m_twi_instance, &twi_mpu_9150_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_twi_enable(&m_twi_instance);
}

void mpu_init(void)
{
    uint32_t err_code;
    // Initiate MPU9150 driver with TWI instance handler
    err_code = mpu9150_init(&m_twi_instance);
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    // Setup and configure the MPU9150 with intial values
    mpu9150_config_t p_mpu_config = MPU9150_DEFAULT_CONFIG(); // Load default values
    p_mpu_config.smplrt_div = 9;   // Change sampelrate. Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV). 19 gives a sample rate of 50Hz
    p_mpu_config.accel_config.afs_sel = AFS_2G; // Set accelerometer full scale range to 2G
    err_code = mpu9150_config(&p_mpu_config); // Configure the MPU9150 with above values
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    
    // This is a way to configure the interrupt pin behaviour
    mpu9150_int_pin_cfg_t p_int_pin_cfg = MPU9150_DEFAULT_INT_PIN_CONFIG(); // Default configurations
    p_int_pin_cfg.int_rd_clear = 1; // When this bit is equal to 1, interrupt status bits are cleared on any read operation
    err_code = mpu9150_int_cfg_pin(&p_int_pin_cfg); // Configure pin behaviour
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    // Enable the MPU interrupts
    mpu9150_int_enable_t p_int_enable = MPU9150_DEFAULT_INT_ENABLE_CONFIG();
    p_int_enable.ff_en = 1; // Trigger interrupt on free fall
    err_code = mpu9150_int_enable(&p_int_enable); // Configure interrupts
    APP_ERROR_CHECK(err_code); // Check for errors in return value    
}

/**
 * @brief Simple interrupt handler setting a flag indicating a free fall
 *
 */
void int_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    mpu_free_fall_event = true;
}

/**
 * @brief Function for initiating the GPIOTE module and enable the 
 * nRF5 to trigger an interrupt on a Low-To-High event on pin MPU9150_MPU_INT_PIN
 *
 */
static void gpiote_init(void)
{
    uint32_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

    err_code = nrf_drv_gpiote_in_init(MPU9150_MPU_INT_PIN, &in_config, int_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(MPU9150_MPU_INT_PIN, true);
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    uint32_t err_code;
    uart_config();
    printf("\033[2J\033[;HMPU9150 example with MPU generated data ready interrupts. Compiled @ %s\r\n", __TIME__);
    twi_init();
    gpiote_init();
    mpu_init();
    
    
    
    uint8_t value;
    
    mpu9150_write_register(MPU9150_REG_FF_DUR, 0x01);
    mpu9150_write_register(MPU9150_REG_FF_THR, 0xFF);
    
    mpu9150_read_registers(MPU9150_REG_FF_DUR, &value, 1);
    printf("MPU9150_REG_FF_DUR: 0x%x\n\r", value);
    
    mpu9150_read_registers(MPU9150_REG_FF_THR, &value, 1);
    printf("MPU9150_REG_FF_THR: 0x%x\r\n", value);
    
    accel_values_t acc_values;
    uint32_t sample_number = 0;
        
    while(1)
    {
        if(mpu_free_fall_event == true)
        {

            err_code = mpu9150_read_accel(&acc_values);
            APP_ERROR_CHECK(err_code);
            // Clear terminal and print values
            printf("\033[2J\033[;HFree fall # %d\r\nX: %06d\r\nY: %06d\r\nZ: %06d", ++sample_number, acc_values.x, acc_values.y, acc_values.z);
            mpu_free_fall_event = false;
        }
    }
}

/** @} */
