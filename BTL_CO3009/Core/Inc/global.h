#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"
#include <stdint.h>

// Định nghĩa chu kỳ tick của Timer (ms)
#define TIMER_TICK_MS       10

// Các chế độ hoạt động chính của hệ thống
typedef enum {
    MODE_AUTOMATIC = 0,
    MODE_MANUAL,
    MODE_TUNING
} SystemMode_t;

// Các trạng thái của cụm đèn giao thông
typedef enum {
    TL_RED = 0,
    TL_GREEN,
    TL_YELLOW,
    TL_OFF
} LightState_t;

// Các trạng thái của FSM Automatic
typedef enum {
    AUTO_INIT = 0,
    AUTO_RED_GREEN,
    AUTO_RED_YELLOW,
    AUTO_GREEN_RED,
    AUTO_YELLOW_RED
} AutoState_t;

// Các trạng thái của FSM Tuning
typedef enum {
    TUNING_RED = 0,
    TUNING_YELLOW,
    TUNING_GREEN
} TuningState_t;

// Biến trạng thái toàn cục
extern SystemMode_t current_mode;
extern AutoState_t   auto_state;
extern TuningState_t tuning_state;

// Thời gian cài đặt cho các đèn (tính theo giây)
// Quy tắc giao thông: time_red = time_green + time_yellow
extern uint8_t time_red;
extern uint8_t time_yellow;
extern uint8_t time_green;

// Bộ đếm lùi thời gian hiển thị trên LCD (giây)
extern uint8_t countdown_way1;
extern uint8_t countdown_way2;

// Dữ liệu cảm biến DHT20
extern float current_temperature;
extern float current_humidity;

void Global_Init(void);

#endif /* INC_GLOBAL_H_ */
