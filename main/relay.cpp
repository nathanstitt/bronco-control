#include "relay.hpp"

#include "../defines.h"
#include <driver/i2c.h>
#include <esp_log.h>

#define I2C_MASTER_SCL_IO 22       // GPIO number for I2C master clock
#define I2C_MASTER_SDA_IO 21       // GPIO number for I2C master data
#define I2C_MASTER_NUM I2C_NUM_0   // I2C port number for master dev
#define I2C_MASTER_FREQ_HZ 100000  // I2C master clock frequency
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define WRITE_BIT I2C_MASTER_WRITE
#define READ_BIT I2C_MASTER_READ
#define ACK_CHECK_EN 0x1           // Enable I2C ack check
#define ACK_VAL 0x0                // I2C ack value
#define NACK_VAL 0x1               // I2C nack value

Relay::Relay() {
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err == ESP_OK) {
        err = i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
        if (err != ESP_OK) {
            ESP_LOGE("Relay", "Failed to install I2C driver: %s", esp_err_to_name(err));
        }
    } else {
        ESP_LOGE("Relay", "Failed to configure I2C: %s", esp_err_to_name(err));
    }
}


esp_err_t
Relay::set_state(bool state) {
    uint8_t command = state ? 0x01 : 0x00; // Assuming 0x01 turns relay ON, 0x00 turns it OFF

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (RELAY_I2C_ADDRESS << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, command, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    if (err != ESP_OK) {
        ESP_LOGE("Relay", "Failed to set relay state: %s", esp_err_to_name(err));
    }

    return err;
}
