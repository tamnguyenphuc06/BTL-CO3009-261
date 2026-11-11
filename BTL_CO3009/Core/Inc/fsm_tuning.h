#ifndef INC_FSM_TUNING_H_
#define INC_FSM_TUNING_H_

#include "global.h"
#include "button.h"
#include "traffic_light.h"

extern uint8_t temp_tuning_value;

void FSM_Tuning_Init(void);
void FSM_Tuning_Run(void);

#endif /* INC_FSM_TUNING_H_ */
