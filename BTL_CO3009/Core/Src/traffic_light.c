#include "traffic_light.h"

void TrafficLight_Init(void) {
    TrafficLight_Set(WAY_1, TL_OFF);
    TrafficLight_Set(WAY_2, TL_OFF);
}

void TrafficLight_Set(WayIndex_t way, LightState_t state) {
    GPIO_TypeDef* port_a;
    uint16_t pin_a;
    GPIO_TypeDef* port_b;
    uint16_t pin_b;

    if (way == WAY_1) {
        port_a = TL1_A_GPIO_Port; pin_a = TL1_A_Pin;
        port_b = TL1_B_GPIO_Port; pin_b = TL1_B_Pin;
    } else {
        port_a = TL2_A_GPIO_Port; pin_a = TL2_A_Pin;
        port_b = TL2_B_GPIO_Port; pin_b = TL2_B_Pin;
    }

    switch (state) {
        case TL_RED:
            HAL_GPIO_WritePin(port_a, pin_a, GPIO_PIN_SET);
            HAL_GPIO_WritePin(port_b, pin_b, GPIO_PIN_RESET);
            break;
        case TL_GREEN:
            HAL_GPIO_WritePin(port_a, pin_a, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(port_b, pin_b, GPIO_PIN_SET);
            break;
        case TL_YELLOW:
            HAL_GPIO_WritePin(port_a, pin_a, GPIO_PIN_SET);
            HAL_GPIO_WritePin(port_b, pin_b, GPIO_PIN_SET);
            break;
        case TL_OFF:
        default:
            HAL_GPIO_WritePin(port_a, pin_a, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(port_b, pin_b, GPIO_PIN_RESET);
            break;
    }
}

void TrafficLight_Toggle(WayIndex_t way, LightState_t color) {
    static uint8_t toggle_state_way1 = 0;
    static uint8_t toggle_state_way2 = 0;

    if (way == WAY_1) {
        toggle_state_way1 = !toggle_state_way1;
        TrafficLight_Set(WAY_1, toggle_state_way1 ? color : TL_OFF);
    } else {
        toggle_state_way2 = !toggle_state_way2;
        TrafficLight_Set(WAY_2, toggle_state_way2 ? color : TL_OFF);
    }
}
