#include "relay.hpp"

#include "../defines.h"

#include "driver/i2c.h"
//#include <driver/i2c_master.h>

#include "esp_log.h"


static const char *TAG = "RELAY";

#define I2C_MASTER_FREQ_HZ          100000
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000


// #define I2C_MASTER_SCL_IO 22       // GPIO number for I2C master clock
// #define I2C_MASTER_SDA_IO 21       // GPIO number for I2C master data
// #define I2C_MASTER_NUM I2C_NUM_0   // I2C port number for master dev
// #define I2C_MASTER_FREQ_HZ 100000  // I2C master clock frequency
// #define I2C_MASTER_TX_BUF_DISABLE 0
// #define I2C_MASTER_RX_BUF_DISABLE 0
// #define WRITE_BIT I2C_MASTER_WRITE
// #define READ_BIT I2C_MASTER_READ
// #define ACK_CHECK_EN 0x1           // Enable I2C ack check
// #define ACK_VAL 0x0                // I2C ack value
// #define NACK_VAL 0x1               // I2C nack value

// #define I2C_MASTER_SCL_IO           idf::SCL_GPIO(CONFIG_I2C_MASTER_SCL)      /*!< GPIO number used for I2C master clock */
// #define I2C_MASTER_SDA_IO           idf::SDA_GPIO(CONFIG_I2C_MASTER_SDA)      /*!< GPIO number used for I2C master data  */

Relay::Relay() {
    // i2c_port_t i2c_master_port = I2C_NUM_0;
    // i2c_config_t conf = {
    //     .mode = I2C_MODE_MASTER,
    //     .sda_io_num = 13,
    //     .scl_io_num = 15,
    //     .sda_pullup_en = GPIO_PULLUP_ENABLE,
    //     .scl_pullup_en = GPIO_PULLUP_ENABLE,
    //     .master = {
    //         .clk_speed = I2C_MASTER_FREQ_HZ,
    //     },
    //     .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL,
    // };
    // //conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    // i2c_param_config(i2c_master_port, &conf);

    // ESP_ERROR_CHECK( i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0) );



    // uint8_t write_buf[1] = {0b10111110};
    // auto err = i2c_master_write_to_device(i2c_master_port, RELAY_I2C_ADDRESS, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    // if (err != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to write to device: %s", esp_err_to_name(err));
    // }



    i2c = new idf::I2CMaster(idf::I2CNumber::I2C0(),
                             idf::SCL_GPIO(RELAY_SCL_GPIO_PIN),
                             idf::SDA_GPIO(RELAY_SDA_GPIO_PIN),
                             idf::Frequency(100000));

    // i2c->release_bus();

     // auto devAddr = idf::I2CAddress(RELAY_I2C_ADDRESS);
     // std::vector<uint8_t> writeData = {0b10111110};
     // i2c->sync_write(devAddr, writeData);



    // ESP_LOGI(TAG, "WHO_AM_I = %X", data[0]);



        //
    // if ( == 0) {
    //     ESP_LOGD(TAG, "Found I2C device at address 0x20\n");
    // } else {
    //     ESP_LOGD(TAG, "No device found at address 0x20\n");
    // }
    //

    // i2c_master_bus_handle_t bus_handle;
    // i2c_master_dev_handle_t dev_handle;

    // i2c_master_bus_config_t bus_config = {};
    // bus_config.i2c_port = I2C_NUM_0;
    // bus_config.sda_io_num = RELAY_SDA_GPIO_PIN;
    // bus_config.scl_io_num = RELAY_SCL_GPIO_PIN;
    // ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    // i2c_device_config_t dev_config = {
    //     .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    //     .device_address = RELAY_I2C_ADDRESS,
    //     .scl_speed_hz = 100000,
    //     .scl_wait_us = 0, // 0 == use default
    // };
    // ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));


    // conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    // conf.
    // conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    // conf.master.clk_speed = 400000;

    // esp_err_t err = i2c_param_config(RELAY_I2C_PORT_NUM, &conf);
    // if (err == ESP_OK) {
    //     err = i2c_driver_install(RELAY_I2C_PORT_NUM, conf.mode, 0, 0, 0);
    //     if (err != ESP_OK) {
    //         ESP_LOGE("Relay", "Failed to install I2C driver: %s", esp_err_to_name(err));
    //     }
    // } else {
    //     ESP_LOGE("Relay", "Failed to configure I2C: %s", esp_err_to_name(err));
    // }

}


esp_err_t
Relay::set_state(bool state) {

     auto devAddr = idf::I2CAddress(RELAY_I2C_ADDRESS);
     std::vector<uint8_t> writeData ;
     writeData.push_back(state ? 0b10111110 : 0);

     ESP_LOGI(TAG, "Turning relay %s", state ? "ON" : "OFF");

     auto writeTx = std::make_shared<idf::I2CWrite>(writeData);

     auto future = i2c->transfer(devAddr, writeTx);
     future.get();

     //i2c->sync_write(devAddr, writeData);

    // uint8_t write_buf[1];//  = {0b10111110};

    // if (state) {
    //     write_buf[0] = 0b10111111;
    //     ESP_LOGI(TAG, "Turning relay ON");
    // } else {
    //     write_buf[0] = 0;
    //     ESP_LOGI(TAG, "Turning relay OFF");
    // }

    // auto err = i2c_master_write_to_device(I2C_NUM_0, RELAY_I2C_ADDRESS, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    // if (err != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to write to device: %s", esp_err_to_name(err));
    // }



     // auto devAddr = idf::I2CAddress(RELAY_I2C_ADDRESS);
     // std::vector<uint8_t> writeData = {0b10111110};
     // auto writeTx = std::make_shared<idf::I2CWrite>(writeData);

      // auto writeFuture = i2c->transfer(devAddr, writeTx);
      // writeFuture.get();

     //i2c->sync_write(devAddr, writeData);


//    esp_err_t writeResult =
   // writeFuture.get();

    // esp_err_t writeResult = writeTx->status();
    // if (writeResult != ESP_OK) {
    //     ESP_LOGE(TAG, "I2C Write failed: %s", esp_err_to_name(writeResult));
    // } else {
    //     ESP_LOGI(TAG, "I2C Write succeeded");
    // }
  // size_t readLength = 2;

  //   // 5. Create a WriteReadTransaction:
  //   //    - first argument: the buffer to write
  //   //    - second argument: how many bytes to write
  //   //    - third argument: how many bytes to read
  //   auto transaction = std::make_shared<idf::I2CMaster::I2CWrite>(
  //                                                                             writeData,
  //                                                                             sizeof(writeData),  // 2 bytes to write
  //                                                                             readLength          // read 2 bytes back
  //                                                                             );

  //   auto future   = i2c.transfer<idf::I2CMaster::WriteReadTransaction>(devAddr, transaction);
  //   auto response = future.get();

    // uint8_t writeData[2] = {0x01, 0x02};
    // uint8_t readData[2]  = {0x00, 0x00};
    // esp_err_t err = i2c->transfer(devAddr, writeData, sizeof(writeData), readData, sizeof(readData));

    // std::shared_ptr ptr = new I2CWrite(std::vector<uint8_t>{0x01, state ? 0x01 : 0x00});
    //
    // i2c->transfer(addr, {0x01});

    // idf::I2CMaster i2c0(idf::I2CNumber::I2C0(), idf::SCL_GPIO(RELAY_SCL_GPIO_PIN), idf::SDA_GPIO(RELAY_SDA_GPIO_PIN), idf::Frequency(100000));


    // reading back the value of WHO_AM_I register which should be 71
    //auto data = i2c0.sync_read(addr, 2);

    // uint8_t command = state ? 0x01 : 0x00; // Assuming 0x01 turns relay ON, 0x00 turns it OFF

    // i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    // i2c_master_start(cmd);
    // i2c_master_write_byte(cmd, (RELAY_I2C_ADDRESS << 1) | WRITE_BIT, ACK_CHECK_EN);
    // i2c_master_write_byte(cmd, command, ACK_CHECK_EN);
    // i2c_master_stop(cmd);

    // esp_err_t err = i2c_master_cmd_begin(RELAY_I2C_BUS_NUM, cmd, pdMS_TO_TICKS(1000));
    // i2c_cmd_link_delete(cmd);

    // if (err != ESP_OK) {
    //     ESP_LOGE("Relay", "Failed to set relay state: %s", esp_err_to_name(err));
    // }

    return 0;
    // return err;
}
