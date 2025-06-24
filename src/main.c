#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

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

    // Compose the command frame
    uint8_t cmd_buf[3] = { 0x00, 0x39, 0x00 };  // Address + control byte
    uint8_t rx_data = 0;

    spi_transaction_t t = {
        .length = 8 * 4,
        .tx_buffer = cmd_buf,
        .rx_buffer = &rx_data,
        .flags = SPI_TRANS_USE_RXDATA,
    };

    // Pad rx buffer with 1 dummy byte so we read the 4th byte correctly
    uint8_t tx_rx_buf[4] = { 0x00, 0x39, 0x00, 0x00 };
    memset(&t, 0, sizeof(t));
    t.length = 8 * 4;
    t.tx_buffer = tx_rx_buf;
    t.rx_buffer = tx_rx_buf;

    ESP_LOGI(TAG, "Reading version register...");
    ret = spi_device_transmit(spi, &t);
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "W5500 Version Register: 0x%02X", tx_rx_buf[3]);
}
