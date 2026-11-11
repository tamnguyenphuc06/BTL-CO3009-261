#ifndef INC_FSM_MANUAL_H_
#define INC_FSM_MANUAL_H_

#include "global.h"
#include "button.h"
#include "traffic_light.h"

typedef enum {
    MANUAL_WAY1_PRIORITY = 0,
    MANUAL_WAY2_PRIORITY
} ManualState_t;

extern ManualState_t manual_state;

void FSM_Manual_Init(void);
void FSM_Manual_Run(void);

#endif /* INC_FSM_MANUAL_H_ */
