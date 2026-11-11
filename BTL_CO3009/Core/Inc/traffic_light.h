#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

#include "main.h"
#include "global.h"

typedef enum {
    WAY_1 = 0,
    WAY_2
} WayIndex_t;

void TrafficLight_Init(void);
void TrafficLight_Set(WayIndex_t way, LightState_t state);
void TrafficLight_Toggle(WayIndex_t way, LightState_t color);

#endif /* INC_TRAFFIC_LIGHT_H_ */
