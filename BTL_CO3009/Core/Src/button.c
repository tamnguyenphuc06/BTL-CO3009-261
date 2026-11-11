#include "button.h"

// Bộ đệm lọc dội 3 tầng
static GPIO_PinState KeyReg0[NUM_BUTTONS];
static GPIO_PinState KeyReg1[NUM_BUTTONS];
static GPIO_PinState KeyReg2[NUM_BUTTONS];
static GPIO_PinState KeyReg3[NUM_BUTTONS];

static uint16_t time_out_for_key_press[NUM_BUTTONS];
static uint8_t  flag_for_button_press[NUM_BUTTONS];
static uint8_t  flag_for_button_hold[NUM_BUTTONS];

// Bảng tra chân GPIO phần cứng đã cấu hình trong CubeMX
static GPIO_TypeDef* BTN_PORTS[NUM_BUTTONS] = {
    BTN_1_GPIO_Port,
    BTN_2_GPIO_Port,
    BTN_3_GPIO_Port,
    BTN_4_GPIO_Port
};

static const uint16_t BTN_PINS[NUM_BUTTONS] = {
    BTN_1_Pin,
    BTN_2_Pin,
    BTN_3_Pin,
    BTN_4_Pin
};

void Button_Init(void) {
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        KeyReg0[i] = BUTTON_RELEASED;
        KeyReg1[i] = BUTTON_RELEASED;
        KeyReg2[i] = BUTTON_RELEASED;
        KeyReg3[i] = BUTTON_RELEASED;
        time_out_for_key_press[i] = DURATION_FOR_AUTO_INCREASING;
        flag_for_button_press[i] = 0;
        flag_for_button_hold[i] = 0;
    }
}

// Hàm này được Scheduler gọi định kỳ mỗi 10ms
void Button_Reading(void) {
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        KeyReg0[i] = KeyReg1[i];
        KeyReg1[i] = KeyReg2[i];
        KeyReg2[i] = HAL_GPIO_ReadPin(BTN_PORTS[i], BTN_PINS[i]);

        if ((KeyReg0[i] == KeyReg1[i]) && (KeyReg1[i] == KeyReg2[i])) {
            // Phát hiện cạnh xuống (vừa bấm nút)
            if (KeyReg3[i] != KeyReg2[i]) {
                KeyReg3[i] = KeyReg2[i];
                if (KeyReg2[i] == BUTTON_PRESSED) {
                    time_out_for_key_press[i] = DURATION_FOR_AUTO_INCREASING;
                    flag_for_button_press[i] = 1;
                }
            } else {
                // Xử lý sự kiện nhấn giữ
                if (KeyReg2[i] == BUTTON_PRESSED) {
                    if (time_out_for_key_press[i] > 0) {
                        time_out_for_key_press[i]--;
                        if (time_out_for_key_press[i] == 0) {
                            flag_for_button_hold[i] = 1;
                            // Cứ mỗi 200ms nhấn giữ tiếp theo sẽ kích hoạt lại cờ
                            time_out_for_key_press[i] = 20;
                        }
                    }
                }
            }
        }
    }
}

uint8_t Is_Button_Pressed(ButtonIndex_t index) {
    if (index >= NUM_BUTTONS) return 0;
    if (flag_for_button_press[index] == 1) {
        flag_for_button_press[index] = 0;
        return 1;
    }
    return 0;
}

uint8_t Is_Button_Held(ButtonIndex_t index) {
    if (index >= NUM_BUTTONS) return 0;
    if (flag_for_button_hold[index] == 1) {
        flag_for_button_hold[index] = 0;
        return 1;
    }
    return 0;
}
