#include "keymoduletask.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "bsp_key.h"
#include "bsp_power.h"


/* KEY_JudgeTask 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define KEY_Judge_PRIO      3                           /* 任务优先级 */
#define KEY_Judge_STK_SIZE  1024*2                      /* 任务堆栈大小 */
TaskHandle_t            KEY_JudgeTask_Handler;          /* 任务句柄 */
void KEY_JudgeTask(void *pvParameters);                 /* 任务函数 */

/* KEY_Queue 队列 配置
 * 包括: 队列句柄 队列长度 队列项目长度
 */
#define KEY_Queue_Length    160                         /* 队列长度 */
#define KEY_Queue_ItemSize  1                           /* 队列项目长度 */
QueueHandle_t KEY_Queue;                                /* 队列句柄 */

/* KEY_Timer 定时器 配置
 * 包括: 队列句柄 队列长度 队列项目长度
 */
#define KEY_Timer_ID        0                           /* 定时器ID */
#define KEY_Timer_Period    100/portTICK_PERIOD_MS      /* 定时器周期 */
TimerHandle_t KEY_Timer_Handler;                        /* 定时器句柄 */
void KEY_ScanTimer(void *pvParameters);


/**
 * @brief       KEY_ValGet
 * @param       StructDef_KEY_RecBuf : 按键状态结构体
 * @param       Ret_KeyScanNum : 应该产生的按键键值数量
 * @retval      无
 */
static void KEY_ValGet(StructDef_KEY* StructDef_KEY_RecBuf, uint8_t Ret_KeyScanNum)
{
    BaseType_t Queue_ret = pdPASS;  //检查数列是否成功写入
    Enum_RetKeyScan Ret_KeyScan = 0;        //按键键值返回变量
    static uint8_t PPT_Status = 0;  //记录PPT按键的按键状态，在按键松开时发送一次松开键值

    /* KEY_CHARGE 键值判断 */
    if (StructDef_KEY_RecBuf->KEY_CHARGE.KEY_Status == KEY_SHORT_DOWN)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_CHARGE 短按\r\n");
#endif
        Ret_KeyScan = KEY_CHARGE_SHORTDOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif
        }
    }
    else if (StructDef_KEY_RecBuf->KEY_CHARGE.KEY_Status == KEY_LONG_DOWN)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_CHARGE 长按\r\n");
#endif
        Ret_KeyScan = KEY_CHARGE_LONG;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif
        }
    }

     /* KEY_SOS 键值判断 */
    if (StructDef_KEY_RecBuf->KEY_SOS.KEY_Status == KEY_LONG_LONG_DOWN)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_SOS 长按10s\r\n");
#endif
        Ret_KeyScan = KEY_SOS_LONG_LONGDOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif
        }
    }
    else if (StructDef_KEY_RecBuf->KEY_SOS.KEY_Status == KEY_LONG_DOWN)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_SOS 长按2s\r\n");
#endif 
        Ret_KeyScan = KEY_SOS_LONGDOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    
     /* KEY_OK 键值判断 */
    if (StructDef_KEY_RecBuf->KEY_OK.KEY_Status == KEY_SHORT_DOWN)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_OK 短按\r\n");
#endif 
        Ret_KeyScan = KEY_OK_SHORTDOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    else if (StructDef_KEY_RecBuf->KEY_OK.KEY_Status == KEY_LONG_DOWN)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_OK 长按\r\n");
#endif 
        Ret_KeyScan = KEY_OK_LONGDOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    else if (StructDef_KEY_RecBuf->KEY_OK.KEY_Status == KEY_COMBINATE0)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_OK+KEY_UP 组合按键\r\n");
#endif 
        Ret_KeyScan = KEY_OK_ADD_KEY_UP;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    else if (StructDef_KEY_RecBuf->KEY_OK.KEY_Status == KEY_COMBINATE1)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_OK+KEY_DOWN 组合按键\r\n");
#endif 
        Ret_KeyScan = KEY_OK_ADD_KEY_DOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }

     /* KEY_UP 键值判断 */
    if(StructDef_KEY_RecBuf->KEY_UP.KEY_Status == KEY_SHORT_DOWN)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_UP 短按\r\n");
#endif 
        Ret_KeyScan = KEY_UP_SHORTDOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    else if(StructDef_KEY_RecBuf->KEY_UP.KEY_Status == KEY_LONG_DOWN)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_UP 长按\r\n");
#endif 
        Ret_KeyScan = KEY_UP_LONGDOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    else if(StructDef_KEY_RecBuf->KEY_UP.KEY_Status == KEY_COMBINATE0)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_UP+KEY_DOWN 组合按键\r\n");
#endif 
        Ret_KeyScan = KEY_UP_ADD_KEY_DOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    else if(StructDef_KEY_RecBuf->KEY_UP.KEY_Status == KEY_COMBINATE1)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_UP+KEY_OK 组合按键\r\n");
#endif 
        Ret_KeyScan = KEY_OK_ADD_KEY_UP;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    
     /* KEY_DOWN 键值判断 */
    if(StructDef_KEY_RecBuf->KEY_DOWN.KEY_Status == KEY_SHORT_DOWN)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_DOWN 短按\r\n");
#endif 
        Ret_KeyScan = KEY_DOWN_SHORTDOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    else if(StructDef_KEY_RecBuf->KEY_DOWN.KEY_Status == KEY_LONG_DOWN)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_DOWN 长按\r\n");
#endif 
        Ret_KeyScan = KEY_DOWN_LONGDOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    else if(StructDef_KEY_RecBuf->KEY_DOWN.KEY_Status == KEY_COMBINATE0)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_DOWN+KEY_UP 组合按键\r\n");
#endif 
        Ret_KeyScan = KEY_UP_ADD_KEY_DOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    else if(StructDef_KEY_RecBuf->KEY_DOWN.KEY_Status == KEY_COMBINATE1)
    {
        Ret_KeyScanNum--;
#if DEBUG_KEY
        printf("KEY_DOWN+KEY_OK 组合按键\r\n");
#endif 
        Ret_KeyScan = KEY_OK_ADD_KEY_DOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }

     /* KEY_PTT 键值判断 */
    if(StructDef_KEY_RecBuf->KEY_PTT.KEY_Status == KEY_SHORT_DOWN)
    {
        Ret_KeyScanNum--;
        PPT_Status = 1;
#if DEBUG_KEY
        printf("KEY_PTT 按下\r\n");
#endif 
        Ret_KeyScan = KEY_PTT_SHORTDOWN;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    else if(StructDef_KEY_RecBuf->KEY_PTT.KEY_Status == KEY_TIMEOUT)
    {
        Ret_KeyScanNum--;
        PPT_Status = 0;
#if DEBUG_KEY
        printf("KEY_PTT 按下超过15s 按键超时\r\n");
#endif 
        Ret_KeyScan = KEY_PTT_TIMEOUT;
        if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
        }
    }
    else if(StructDef_KEY_RecBuf->KEY_PTT.KEY_Status == KEY_RELEASE)
    {
        if(Ret_KeyScanNum)
        {
            if(PPT_Status)
            {   
                PPT_Status = 0;
#if DEBUG_KEY
                printf("KEY_PTT 松开\r\n");
#endif 
                Ret_KeyScan = KEY_PTT_RELEASE;
                if((Queue_ret = xQueueSendToFrontFromISR(KEY_Queue, &Ret_KeyScan, 0)) != pdPASS)
                {
#if DEBUG_KEY
                    printf("KEY_Queue 写队列错误 ErrorCode: %d\r\n", Queue_ret);
#endif 
                }
            }
        }
    }
}

/**
 * @brief       KEY_ScanTimer
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void KEY_ScanTimer(void *pvParameters)
{
    pvParameters = pvParameters;
    StructDef_KEY KEY_Buf_Structure = {KEY_RELEASE};
    uint8_t Ret_KeyScanNum = 0;

    if((Ret_KeyScanNum = Total_KeyScan(&KEY_Buf_Structure)) != 0)
    {

        KEY_ValGet(&KEY_Buf_Structure, Ret_KeyScanNum);
    }
}

void KEYScan_Start(void)
{
    BaseType_t Ret = pdPASS;

 /* 创建按键队列 */
    KEY_Queue = xQueueCreate(KEY_Queue_Length,      /* 队列项目长度 */
                            KEY_Queue_ItemSize);    /* 队列项目长度 */

 /* 创建定时器 */
    KEY_Timer_Handler = xTimerCreate((const char*   )"KEY_ScanTimer",           /* 定时器名称 */
                            (const TickType_t       )KEY_Timer_Period,          /* 定时器定时周期 */
                            (const BaseType_t       )Timer_AutoReloadEnable,    /* 定时器自动重装载 */
                            (void * const           )KEY_Timer_ID,              /* 定时器ID */
                            (TimerCallbackFunction_t) KEY_ScanTimer);           /* 定时器任务 */
    
    if(KEY_Timer_Handler != NULL)
    {
#if DEBUG_KEY
        printf("KEY_Timer_Handler 创建成功\r\n");
#endif 
        Ret = xTimerStart(KEY_Timer_Handler, 0);
        if(Ret != pdPASS)
        {
#if DEBUG_KEY
            printf("KEY_Timer_Handler 启动失败\r\n");
#endif
        }
        else
        {
#if DEBUG_KEY
            printf("KEY_Timer_Handler 启动成功\r\n");
#endif
        }
    }
    else
    {
#if DEBUG_KEY
        printf("KEY_Timer_Handler 创建失败\r\n");
#endif
    }
}

/**
 * @brief       KEY_JudgeTask
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void KEY_JudgeTask(void *pvParameters)
{
    pvParameters = pvParameters;
    TickType_t xLastWakeTime;
    BaseType_t Queue_ret = pdPASS;
    Enum_RetKeyScan Queue_KeyVal = 0;
    
#if DEBUG_KEY
    printf("KEY_JudgeTask Start\r\n");
#endif
    xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        if((Queue_ret = xQueueReceive(KEY_Queue, &Queue_KeyVal, portMAX_DELAY)) == pdPASS)
        {
#if DEBUG_KEY
            printf("Queue_KeyVal = %d\r\n", Queue_KeyVal);
#endif
            if(Queue_KeyVal == KEY_CHARGE_LONG)
            {
                if(Flag_PowerStatus)
				{
                    Flag_PowerStatus = 0;
					Sys_Power(PowerOFF);
				}
                else
				{
                    Flag_PowerStatus = 1;
					Sys_Power(PowerON);
				}
            }
        }

        vTaskDelayUntil(&xLastWakeTime, 100);
    }
}

void KEYScan_Action_Start(void)
{
    BaseType_t Ret = pdPASS;

 /* 创建任务 */
    Ret = xTaskCreatePinnedToCore((TaskFunction_t )KEY_JudgeTask,           /* 任务函数 */
                            (const char*    )"KEY_JudgeTask",               /* 任务名称 */
                            (uint16_t       )KEY_Judge_STK_SIZE,            /* 任务堆栈大小 */
                            (void*          )NULL,                          /* 传入给任务函数的参数 */
                            (UBaseType_t    )KEY_Judge_PRIO,                /* 任务优先级 */
                            (TaskHandle_t*  )&KEY_JudgeTask_Handler,        /* 任务句柄 */
                            (BaseType_t     ) 0);                           /* 该任务哪个内核运行 */
    
    if(Ret != pdPASS)
    {
#if DEBUG_KEY
        printf("KEY_JudgeTask 创建失败\r\n");
#endif
        vTaskDeleteWithCaps(KEY_JudgeTask_Handler);
    }
}