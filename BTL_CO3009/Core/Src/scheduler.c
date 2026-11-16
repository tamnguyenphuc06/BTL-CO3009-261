#include "scheduler.h"
#include <stddef.h>
#include "main.h"

static sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint8_t current_task_count = 0;

/* Chỉ gọi khi đã khóa ngắt: giữ đúng độ trễ tương đối khi xóa. */
static void SCH_Delete_At_Locked(uint8_t index) {
    if (index + 1U < current_task_count) {
        SCH_tasks_G[index + 1U].Delay += SCH_tasks_G[index].Delay;
    }
    for (uint8_t i = index; i + 1U < current_task_count; i++) {
        SCH_tasks_G[i] = SCH_tasks_G[i + 1U];
        SCH_tasks_G[i].TaskID = i; // TaskID trong cấu trúc này là vị trí hiện tại.
    }
    current_task_count--;
    SCH_tasks_G[current_task_count] = (sTask){0};
    if (current_task_count > 0U && SCH_tasks_G[0].Delay == 0U) {
        SCH_tasks_G[0].RunMe = 1U;
    }
}

void SCH_Init(void) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i] = (sTask){0};
    }
    current_task_count = 0;
    __set_PRIMASK(primask);
}

// O(1)
void SCH_Update(void) {
    if (current_task_count > 0U && SCH_tasks_G[0].pTask != NULL) {
        if (SCH_tasks_G[0].Delay > 0U) {
            SCH_tasks_G[0].Delay--;
        }
        if (SCH_tasks_G[0].Delay == 0U) {
            SCH_tasks_G[0].RunMe = 1U;
        }
    }
}

// Thêm tác vụ vào hàng đợi
uint8_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
    uint32_t primask = __get_PRIMASK(); // luu trang thai ngat
    __disable_irq();                    //tat ngat, tranh xung dot

    if (current_task_count >= SCH_MAX_TASKS || pFunction == NULL) {
        __set_PRIMASK(primask);
        return SCH_MAX_TASKS;
    }

    uint8_t index = 0;
    // Tìm vị trí chèn theo độ trễ tương đối, giữ thứ tự các tác vụ cùng hạn.
    while (index < current_task_count && DELAY >= SCH_tasks_G[index].Delay) {
        DELAY -= SCH_tasks_G[index].Delay;
        index++;
    }

    // Dịch chuyển các tác vụ phía sau sang phải
    for (uint8_t i = current_task_count; i > index; i--) {
        SCH_tasks_G[i] = SCH_tasks_G[i - 1U];
        SCH_tasks_G[i].TaskID = i;
    }

    // Gán thông tin tác vụ mới
    SCH_tasks_G[index].pTask = pFunction;
    SCH_tasks_G[index].Delay = DELAY;
    SCH_tasks_G[index].Period = PERIOD;
    SCH_tasks_G[index].RunMe = (DELAY == 0U) ? 1U : 0U;
    SCH_tasks_G[index].TaskID = index;

    // Trừ độ trễ của tác vụ ngay sau
    if (index < current_task_count) {
        SCH_tasks_G[index + 1U].Delay -= DELAY;
    }

    current_task_count++;
    __set_PRIMASK(primask);
    return index;
}

uint8_t SCH_Delete_Task(uint32_t task_index) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    if (task_index >= current_task_count) {
        __set_PRIMASK(primask);
        return 0;
    }
    SCH_Delete_At_Locked((uint8_t)task_index);
    __set_PRIMASK(primask);
    return 1;
}

// Thực thi tác vụ tại vòng lặp while(1)
void SCH_Dispatch_Tasks(void) {
    for (;;) {
        uint32_t primask = __get_PRIMASK();
        __disable_irq();
        if (current_task_count == 0U || SCH_tasks_G[0].RunMe == 0U) {
            __set_PRIMASK(primask);
            break;
        }

        void (*temp_pTask)(void) = SCH_tasks_G[0].pTask;
        uint32_t temp_period = SCH_tasks_G[0].Period;
        // Xóa tác vụ đầu
        SCH_Delete_At_Locked(0);
        __set_PRIMASK(primask);

        // Thực thi hàm tác vụ ngoài vùng khóa ngắt
        (*temp_pTask)();

        // Nếu là tác vụ lặp lại, thêm lại vào hàng đợi
        if (temp_period > 0U) {
            SCH_Add_Task(temp_pTask, temp_period, temp_period);
        }
    }
}
