#include "fsm_automatic.h"

static uint16_t sub_timer_1s = 0;

void FSM_Automatic_Init(void) {
    auto_state = AUTO_RED_GREEN;
    countdown_way1 = time_red;
    countdown_way2 = time_green;
    sub_timer_1s = 100; // 100 ticks x 10ms = 1000ms (1s)

    TrafficLight_Set(WAY_1, TL_RED);
    TrafficLight_Set(WAY_2, TL_GREEN);
}

// Hàm này được Scheduler gọi mỗi 10ms
void FSM_Automatic_Run(void) {
    if (sub_timer_1s > 0) {
        sub_timer_1s--;
    }

    // Đủ 1 giây thì giảm bộ đếm lùi
    if (sub_timer_1s == 0) {
        sub_timer_1s = 100;

        if (countdown_way1 > 0) countdown_way1--;
        if (countdown_way2 > 0) countdown_way2--;

        switch (auto_state) {
            case AUTO_RED_GREEN:
                if (countdown_way2 == 0) {
                    auto_state = AUTO_RED_YELLOW;
                    countdown_way2 = time_yellow;
                    TrafficLight_Set(WAY_1, TL_RED);
                    TrafficLight_Set(WAY_2, TL_YELLOW);
                }
                break;

            case AUTO_RED_YELLOW:
                if (countdown_way2 == 0) {
                    auto_state = AUTO_GREEN_RED;
                    countdown_way1 = time_green;
                    countdown_way2 = time_red;
                    TrafficLight_Set(WAY_1, TL_GREEN);
                    TrafficLight_Set(WAY_2, TL_RED);
                }
                break;

            case AUTO_GREEN_RED:
                if (countdown_way1 == 0) {
                    auto_state = AUTO_YELLOW_RED;
                    countdown_way1 = time_yellow;
                    TrafficLight_Set(WAY_1, TL_YELLOW);
                    TrafficLight_Set(WAY_2, TL_RED);
                }
                break;

            case AUTO_YELLOW_RED:
                if (countdown_way1 == 0) {
                    auto_state = AUTO_RED_GREEN;
                    countdown_way1 = time_red;
                    countdown_way2 = time_green;
                    TrafficLight_Set(WAY_1, TL_RED);
                    TrafficLight_Set(WAY_2, TL_GREEN);
                }
                break;

            default:
                FSM_Automatic_Init();
                break;
        }
    }
}
