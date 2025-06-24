#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "W5500_TEST"

#define W5500_MISO 19
#define W5500_MOSI 23
#define W5500_SCLK 18
#define W5500_CS    5

void app_main(void)
{
    esp_err_t ret;

    ESP_LOGI(TAG, "Starting SPI bus...");
    spi_bus_config_t buscfg = {
        .miso_io_num = W5500_MISO,
        .mosi_io_num = W5500_MOSI,
        .sclk_io_num = W5500_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64
    };
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_DISABLED);
    ESP_ERROR_CHECK(ret);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 8 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = W5500_CS,
        .queue_size = 1,
        .command_bits = 0,
        .address_bits = 0,
    };

    spi_device_handle_t spi;
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    uint8_t cmd[3] = { 0x00, 0x39, 0x00 };
    uint8_t rx_data[1] = {0};
    spi_transaction_t t = {
        .length = 8 * (sizeof(cmd) + sizeof(rx_data)),
        .tx_buffer = cmd,
        .rxlength = 8 * sizeof(rx_data),
        .rx_buffer = rx_data
    };

    ESP_LOGI(TAG, "Sending version register read...");
    ret = spi_device_transmit(spi, &t);
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "W5500 Version Register: 0x%02X", rx_data[0]);
}
