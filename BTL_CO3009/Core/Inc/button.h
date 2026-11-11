#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"
#include <stdint.h>

#define NUM_BUTTONS         4
#define BUTTON_PRESSED      GPIO_PIN_RESET
#define BUTTON_RELEASED     GPIO_PIN_SET

// 100 chu kỳ x 10ms = 1000ms (1 giây nhấn giữ)
#define DURATION_FOR_AUTO_INCREASING    100

typedef enum {
    BTN_INDEX_1 = 0,
    BTN_INDEX_2,
    BTN_INDEX_3,
    BTN_INDEX_4
} ButtonIndex_t;

void    Button_Init(void);
void    Button_Reading(void);
uint8_t Is_Button_Pressed(ButtonIndex_t index);
uint8_t Is_Button_Held(ButtonIndex_t index);

#endif /* INC_BUTTON_H_ */
