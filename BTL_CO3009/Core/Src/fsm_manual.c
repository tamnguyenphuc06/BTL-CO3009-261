#include "fsm_manual.h"

ManualState_t manual_state = MANUAL_WAY1_PRIORITY;

void FSM_Manual_Init(void) {
    manual_state = MANUAL_WAY1_PRIORITY;
    TrafficLight_Set(WAY_1, TL_GREEN);
    TrafficLight_Set(WAY_2, TL_RED);
}

void FSM_Manual_Run(void) {
    if (Is_Button_Pressed(BTN_INDEX_2)) {
        if (manual_state == MANUAL_WAY1_PRIORITY) {
            manual_state = MANUAL_WAY2_PRIORITY;
            TrafficLight_Set(WAY_1, TL_RED);
            TrafficLight_Set(WAY_2, TL_GREEN);
        } else {
            manual_state = MANUAL_WAY1_PRIORITY;
            TrafficLight_Set(WAY_1, TL_GREEN);
            TrafficLight_Set(WAY_2, TL_RED);
        }
    }
}
