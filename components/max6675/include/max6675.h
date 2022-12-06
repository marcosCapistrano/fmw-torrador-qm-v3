#ifndef MAX6675_H
#define MAX6675_H

#include <stdint.h>
#include <stdbool.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX6675_MAX_CLOCK_SPEED_HZ (4300000) // 4.3 MHz

/**
 * Conversion mode
 */
typedef enum {
    MAX6675_MODE_SINGLE = 0, /**< Single consersion mode, default */
    MAX6675_MODE_AUTO        /**< Automatic conversion mode at 50/60Hz rate */
} max6675_mode_t;

/**
 * Notch frequencies for the noise rejection filter
 */
typedef enum {
    MAX6675_FILTER_60HZ = 0, /**< 60Hz */
    MAX6675_FILTER_50HZ      /**< 50Hz */
} max6675_filter_t;

/**
 * Connection type
 */
typedef enum {
    MAX6675_2WIRE = 0, /**< 2 wires */
    MAX6675_3WIRE,     /**< 3 wires */
    MAX6675_4WIRE      /**< 4 wires */
} max6675_connection_type_t;

/**
 * Device configuration
 */
typedef struct
{
    max6675_mode_t mode;
    max6675_connection_type_t connection;
    bool v_bias;
    max6675_filter_t filter;
} max6675_config_t;

/**
 * Temperature scale standard
 */
typedef enum {
    MAX6675_ITS90 = 0,    /**< ITS-90 */
    MAX6675_DIN43760,     /**< DIN43760 */
    MAX6675_US_INDUSTRIAL /**< US INDUSTRIAL */
} max6675_standard_t;

/**
 * Device descriptor
 */
typedef struct
{
    spi_device_interface_config_t spi_cfg;  /**< SPI device configuration */
    spi_device_handle_t spi_dev;            /**< SPI device handler */
    max6675_standard_t standard;           /**< Temperature scale standard */
    float r_ref;                            /**< Reference resistor value, Ohms */
    float rtd_nominal;                      /**< RTD nominal resistance at 0 deg. C, Ohms (PT100 - 100 Ohms, PT1000 - 1000 Ohms) */
} max6675_t;

/**
 * @brief Initialize device descriptor
 *
 * @param dev            Device descriptor
 * @param host           SPI host
 * @param clock_speed_hz SPI clock speed, Hz
 * @param cs_pin         CS GPIO number
 * @return `ESP_OK` on success
 */
esp_err_t max6675_init_desc(max6675_t *dev, spi_host_device_t host, uint32_t clock_speed_hz, gpio_num_t cs_pin);

/**
 * @brief Free device descriptor
 *
 * @param dev Device descriptor
 * @return `ESP_OK` on success
 */
esp_err_t max6675_free_desc(max6675_t *dev);

/**
 * @brief Configure device
 *
 * @param dev    Device descriptor
 * @param config Configuration
 * @return `ESP_OK` on success
 */
esp_err_t max6675_set_config(max6675_t *dev, const max6675_config_t *config);

/**
 * @brief Read device configuration
 *
 * @param dev         Device descriptor
 * @param[out] config Configuration
 * @return `ESP_OK` on success
 */
esp_err_t max6675_get_config(max6675_t *dev, max6675_config_t *config);

/**
 * @brief Trigger single-shot measurement
 *
 * @param dev Device descriptor
 * @return `ESP_OK` on success
 */
esp_err_t max6675_start_measurement(max6675_t *dev);

/**
 * @brief Read raw RTD value
 *
 * @param dev        Device descritptor
 * @param[out] raw   Raw 15 bits RTD value
 * @param[out] fault true when fault is detected
 * @return `ESP_OK` on success
 */
esp_err_t max6675_read_raw(max6675_t *dev, uint16_t *raw, bool *fault);

/**
 * @brief Read RTD value and convert it temperature
 *
 * @param dev       Device descritptor
 * @param[out] temp Temperature, deg. Celsius
 * @return `ESP_OK` on success
 */
esp_err_t max6675_read_temperature(max6675_t *dev, float *temp);

/**
 * @brief Measure temperature
 *
 * Run full cycle of single-shot measurement:
 *  - trigger measurement
 *  - wait for 70ms
 *  - read and convert RTD value
 *
 * @param dev       Device descritptor
 * @param[out] temp Temperature, deg. Celsius
 * @return `ESP_OK` on success
 */
esp_err_t max6675_measure(max6675_t *dev, float *temp);

/**
 * @brief Run automatical fault detection cycle
 *
 * After calling this function, device must be reconfigured
 *
 * @param dev Device descriptor
 * @return `ESP_OK` on success
 */
esp_err_t max6675_detect_fault_auto(max6675_t *dev);

/**
 * @brief Get bits of current fault status
 *
 * See datasheet for fault status interpretation
 *
 * @param dev               Device descriptor
 * @param[out] fault_status Fault status bits
 * @return `ESP_OK` on success
 */
esp_err_t max6675_get_fault_status(max6675_t *dev, uint8_t *fault_status);

/**
 * @brief Clear current fault status
 *
 * After calling this function, device must be reconfigured
 *
 * @param dev Device descriptor
 * @return `ESP_OK` on success
 */
esp_err_t max6675_clear_fault_status(max6675_t *dev);

#ifdef __cplusplus
}
#endif

#endif