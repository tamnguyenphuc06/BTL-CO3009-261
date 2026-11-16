#include "dht20.h"
#include <stddef.h>

extern I2C_HandleTypeDef hi2c1;

#define SLAVE_ADDRESS_DHT20 (0x38 << 1)
#define DHT20_I2C_TIMEOUT_MS 5U
#define DHT20_INTERVAL_TICKS 20U  // Task được gọi mỗi 100 ms

uint16_t value_x10[2] = {0, 0};

typedef enum {
    DHT20_STATE_TRIGGER = 0,
    DHT20_STATE_READ,
    DHT20_STATE_WAIT_NEXT,
    DHT20_STATE_RESET_SEND,
    DHT20_STATE_RESET_READ,
    DHT20_STATE_RESET_WRITE
} DHT20_State_t;

static DHT20_State_t dht20_state = DHT20_STATE_TRIGGER;
static uint8_t wait_ticks = 0;
static uint8_t reset_index = 0;
static uint8_t reset_register = 0;
static uint8_t reset_reply[3];
static const uint8_t reset_registers[3] = {0x1B, 0x1C, 0x1E};

static void dht20_wait_next(void) {
    wait_ticks = 0;
    dht20_state = DHT20_STATE_WAIT_NEXT;
}

void dht20_init(void) {
    uint8_t init[3];

    dht20_state = DHT20_STATE_TRIGGER;
    wait_ticks = 0;
    reset_index = 0;

    init[0] = 0xA8;
    init[1] = 0x00;
    init[2] = 0x00;
    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, init, 3, DHT20_I2C_TIMEOUT_MS);
    HAL_Delay(20);

    init[0] = 0xBE;
    init[1] = 0x08;
    init[2] = 0x00;
    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, init, 3, DHT20_I2C_TIMEOUT_MS);
    HAL_Delay(20);
}

void dht20_reset(uint8_t regist) {
    // Đặt yêu cầu reset;
    reset_register = regist;
    dht20_state = DHT20_STATE_RESET_SEND;
}

static uint8_t dht20_trigger_measurement(void) {
    uint8_t status = 0;
    uint8_t data[3] = {0xAC, 0x33, 0x00};

    if (HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDRESS_DHT20, &status, 1,
                               DHT20_I2C_TIMEOUT_MS) != HAL_OK) return 0;

    if ((status & 0x18U) != 0x18U) {
        reset_index = 0;
        dht20_reset(reset_registers[reset_index]);
        return 0;
    }

    return HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, data, 3,
                                   DHT20_I2C_TIMEOUT_MS) == HAL_OK;
}

static void dht20_fetch_data(uint16_t* value) {
    uint8_t data[7] = {0};
    uint32_t Temper = 0, Humid = 0;
    int32_t temperature_x10;

    if (HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDRESS_DHT20, data, 7,
                               DHT20_I2C_TIMEOUT_MS) == HAL_OK && !(data[0] & 0x80U)) {
        // Humid (x10)
        Humid = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
        Humid = (Humid * 1000U) / 1048576U;
        if (Humid > 1000U) Humid = 1000U;
        value[0] = (uint16_t)Humid;

        // Temperature (x10)
        Temper = (((uint32_t)data[3] & 0x0FU) << 16) |
                 ((uint32_t)data[4] << 8) | data[5];
        temperature_x10 = (int32_t)((Temper * 2000U) / 1048576U) - 500;

        value[1] = (temperature_x10 < 0) ? 0U : (uint16_t)temperature_x10;

        current_humidity    = value[0] / 10.0f;
        current_temperature = temperature_x10 / 10.0f;
    }
}

// Hàm đọc trực tiếp (dùng 1 lần lúc khởi tạo)
void dht20_read(uint16_t* value) {
    if (value == NULL) return;
    if (!dht20_trigger_measurement()) return;
    HAL_Delay(80);
    dht20_fetch_data(value);
}

// Hàm chạy định kỳ mỗi 100ms bởi Scheduler
void DHT20_Task_Run(void) {
    uint8_t request[3];

    switch (dht20_state) {
        case DHT20_STATE_TRIGGER:
            if (dht20_trigger_measurement()) {
                dht20_state = DHT20_STATE_READ;
            } else if (dht20_state == DHT20_STATE_TRIGGER) {
                dht20_wait_next();
            }
            break;

        case DHT20_STATE_READ:
            // Sau 100ms thì đọc kết quả, sau đó đợi đủ 2 giây mới đo lại.
            dht20_fetch_data(value_x10);
            dht20_wait_next();
            break;

        case DHT20_STATE_WAIT_NEXT:
            if (++wait_ticks >= DHT20_INTERVAL_TICKS) {
                dht20_state = DHT20_STATE_TRIGGER;
            }
            break;

        case DHT20_STATE_RESET_SEND:
            request[0] = reset_register;
            request[1] = 0;
            request[2] = 0;
            if (HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, request, 3,
                                        DHT20_I2C_TIMEOUT_MS) == HAL_OK) {
                dht20_state = DHT20_STATE_RESET_READ;
            } else {
                dht20_wait_next();
            }
            break;

        case DHT20_STATE_RESET_READ:
            if (HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDRESS_DHT20, reset_reply, 3,
                                       DHT20_I2C_TIMEOUT_MS) == HAL_OK) {
                dht20_state = DHT20_STATE_RESET_WRITE;
            } else {
                dht20_wait_next();
            }
            break;

        case DHT20_STATE_RESET_WRITE:
            request[0] = 0xB0U | reset_register;
            request[1] = reset_reply[1];
            request[2] = reset_reply[2];
            if (HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, request, 3,
                                        DHT20_I2C_TIMEOUT_MS) != HAL_OK) {
                dht20_wait_next();
                break;
            }
            if (++reset_index < 3U) {
                dht20_reset(reset_registers[reset_index]);
            } else {
                dht20_wait_next();
            }
            break;

        default:
            dht20_wait_next();
            break;
    }
}
