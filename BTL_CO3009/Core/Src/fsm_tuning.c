#include "fsm_tuning.h"

uint8_t temp_tuning_value = 0;
static uint8_t blink_counter = 0;
static LightState_t current_blink_color = TL_RED;

void FSM_Tuning_Init(void) {
    tuning_state = TUNING_RED;
    current_blink_color = TL_RED;
    temp_tuning_value = time_red;
    blink_counter = 0;
    TrafficLight_Set(WAY_1, TL_OFF);
    TrafficLight_Set(WAY_2, TL_OFF);
}

void FSM_Tuning_Run(void) {
    // 1. Nhấp nháy đèn tương ứng ở tần số 2Hz (toggle mỗi 250ms = 25 ticks)
    blink_counter++;
    if (blink_counter >= 25) {
        blink_counter = 0;
        TrafficLight_Toggle(WAY_1, current_blink_color);
        TrafficLight_Toggle(WAY_2, current_blink_color);
    }

    // 2. BTN_2: Đổi màu đèn cần cài đặt
    if (Is_Button_Pressed(BTN_INDEX_2)) {
        TrafficLight_Set(WAY_1, TL_OFF);
        TrafficLight_Set(WAY_2, TL_OFF);

        switch (tuning_state) {
            case TUNING_RED:
                tuning_state = TUNING_YELLOW;
                current_blink_color = TL_YELLOW;
                temp_tuning_value = time_yellow;
                break;
            case TUNING_YELLOW:
                tuning_state = TUNING_GREEN;
                current_blink_color = TL_GREEN;
                temp_tuning_value = time_green;
                break;
            case TUNING_GREEN:
                tuning_state = TUNING_RED;
                current_blink_color = TL_RED;
                temp_tuning_value = time_red;
                break;
        }
    }

    // 3. BTN_3: Tăng thời gian (+1s hoặc nhấn giữ tự động tăng)
    if (Is_Button_Pressed(BTN_INDEX_3) || Is_Button_Held(BTN_INDEX_3)) {
        temp_tuning_value++;
        if (temp_tuning_value > 99) {
            temp_tuning_value = 1;
        }
    }

    // 4. BTN_4: Xác nhận và lưu cài đặt
    if (Is_Button_Pressed(BTN_INDEX_4)) {
        switch (tuning_state) {
            case TUNING_RED:
                time_red = temp_tuning_value;
                // Đảm bảo ràng buộc: Red = Green + Yellow
                if (time_red > time_yellow) {
                    time_green = time_red - time_yellow;
                }
                break;
            case TUNING_YELLOW:
                time_yellow = temp_tuning_value;
                time_red = time_green + time_yellow;
                break;
            case TUNING_GREEN:
                time_green = temp_tuning_value;
                time_red = time_green + time_yellow;
                break;
        }
    }
}
