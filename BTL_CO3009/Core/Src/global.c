#include "global.h"

SystemMode_t current_mode = MODE_AUTOMATIC;
AutoState_t   auto_state   = AUTO_INIT;
TuningState_t tuning_state = TUNING_RED;

// Giá trị khởi tạo mặc định: Đỏ 5s, Xanh 3s, Vàng 2s
uint8_t time_red    = 5;
uint8_t time_yellow = 2;
uint8_t time_green  = 3;

uint8_t countdown_way1 = 0;
uint8_t countdown_way2 = 0;

float current_temperature = 0.0f;
float current_humidity    = 0.0f;

void Global_Init(void) {
    current_mode    = MODE_AUTOMATIC;
    auto_state      = AUTO_INIT;
    countdown_way1  = time_red;
    countdown_way2  = time_green;
}
