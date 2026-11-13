#include "dht20.h"

extern I2C_HandleTypeDef hi2c1;

#define SLAVE_ADDRESS_DHT20 (0x38 << 1)

uint16_t value_x10[2] = {0, 0};

typedef enum {
    DHT20_STATE_TRIGGER = 0,
    DHT20_STATE_READ
} DHT20_State_t;

static DHT20_State_t dht20_state = DHT20_STATE_TRIGGER;
static uint8_t wait_ticks = 0;

void dht20_init(void) {
    uint8_t init[3];

    init[0] = 0xA8;
    init[1] = 0x00;
    init[2] = 0x00;
    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*)init, 3, 0xFF);
    HAL_Delay(20);

    init[0] = 0xBE;
    init[1] = 0x08;
    init[2] = 0x00;
    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*)init, 3, 0xFF);
    HAL_Delay(20);
}

void dht20_reset(uint8_t regist) {
    uint8_t reset[3], reply[3];

    reset[0] = regist;
    reset[1] = 0x00;
    reset[2] = 0x00;
    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*)reset, 3, 0xFF);
    HAL_Delay(10);

    HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDRESS_DHT20 | 0x01, (uint8_t*)reply, 3, 0xFF);
    reset[0] = 0xB0 | regist;
    reset[1] = reply[1];
    reset[2] = reply[2];
    HAL_Delay(10);

    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*)reset, 3, 0xFF);
}

static void dht20_trigger_measurement(void) {
    uint8_t status[1] = {0};
    HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDRESS_DHT20 | 0x01, (uint8_t*)status, 1, 0xFF);

    if ((status[0] & 0x18) != 0x18) {
        dht20_reset(0x1B);
        dht20_reset(0x1C);
        dht20_reset(0x1E);
    }

    uint8_t data[3] = {0xAC, 0x33, 0x00};
    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*)data, 3, 0xFF);
}

static void dht20_fetch_data(uint16_t* value) {
    uint8_t data[7] = {0};
    uint32_t Temper = 0, Humid = 0;

    if (HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDRESS_DHT20 | 0x01, (uint8_t*)data, 7, 0xFF) == HAL_OK) {
        // Humid (x10)
        Humid = (Humid | data[1]) << 8;
        Humid = (Humid | data[2]) << 8;
        Humid = Humid | data[3];
        Humid = Humid >> 4;
        Humid = (Humid * 100 * 10 / 1024 / 1024);
        value[0] = (uint16_t)Humid;

        // Temperature (x10)
        Temper = (Temper | data[3]) << 8;
        Temper = (Temper | data[4]) << 8;
        Temper = Temper | data[5];
        Temper = Temper & 0xfffff;
        Temper = Temper * 200 * 10 / 1024 / 1024 - 500;
        value[1] = (uint16_t)Temper;

        current_humidity    = value[0] / 10.0f;
        current_temperature = value[1] / 10.0f;
    }
}

// Hàm đọc trực tiếp (dùng 1 lần lúc khởi tạo)
void dht20_read(uint16_t* value) {
    dht20_trigger_measurement();
    HAL_Delay(80);
    dht20_fetch_data(value);
}

// Hàm chạy định kỳ mỗi 100ms (10 ticks) bởi Scheduler
void DHT20_Task_Run(void) {
    switch (dht20_state) {
        case DHT20_STATE_TRIGGER:
            dht20_trigger_measurement();
            wait_ticks = 0;
            dht20_state = DHT20_STATE_READ;
            break;

        case DHT20_STATE_READ:
            // Sau 100ms (1 lần gọi) thì đọc kết quả, sau đó đợi đủ 2 giây (20 lần x 100ms) mới đo lại
            if (wait_ticks == 0) {
                dht20_fetch_data(value_x10);
            }
            wait_ticks++;
            if (wait_ticks >= 20) {
                dht20_state = DHT20_STATE_TRIGGER;
            }
            break;
    }
}
