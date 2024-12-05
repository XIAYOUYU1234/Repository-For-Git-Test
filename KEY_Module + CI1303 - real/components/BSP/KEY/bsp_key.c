#include "bsp_key.h"
#include "bsp_power.h"
#include "bsp_xl9555.h"
#include "esp_log.h"
#include "string.h"

uint8_t Flag_PowerStatus = 0;

/**
 * @brief       初始化按键
 * @param       无
 * @retval      无
 */
void Single_key_init(void)
{
    gpio_config_t gpio_init_struct;

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;                 /* 失能引脚中断 */
    gpio_init_struct.mode = GPIO_MODE_INPUT;                        /* 输入模式 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;               /* 使能上拉 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;          /* 失能下拉 */
    gpio_init_struct.pin_bit_mask = 1ull << KEY_Charge_GPIO_PIN;    /* PTT按键引脚 */
    gpio_config(&gpio_init_struct);                                 /* 配置使能 */
	gpio_init_struct.pin_bit_mask = 1ull << KEY_OK_GPIO_PIN;        /* OK按键引脚 */
    gpio_config(&gpio_init_struct);                                 /* 配置使能 */
}

static void Key_OK_KeyScan(uint8_t* Key_TimeBuf, uint8_t* Key_SelfLock)
{
    uint8_t times = 0;
    uint8_t Self_Lock = 0;

    Self_Lock = Key_SelfLock[4];
    times = Key_TimeBuf[4];

    if(OK_KEY == PIN_RESET)
    {
        vTaskDelay(10);
        if(OK_KEY == PIN_RESET)
        {
            if(Self_Lock == 0) times++;
        }
    } 
    else 
    {
#if DEBUG_KEY
        if(times)
            printf("Key_OK 按下时间：%d *100ms\r\n\r\n", times);
#endif
        Self_Lock = 0;
    }

    Key_SelfLock[4] = Self_Lock;
    Key_TimeBuf[4] = times;
}

static uint8_t Key_OK_KeyDownJudje(uint8_t* Key_TimeBuf, uint8_t* pKey_TimeBuf, StructDef_KEY* StructDef_KEY_Buf)
{
    uint8_t times = 0, ptimes = 0;
    uint8_t DownJudje_Ret = 0;

    times = Key_TimeBuf[4];
    ptimes = pKey_TimeBuf[4];

    if(ptimes != times)
    {
       ptimes = times;
       if(times >= 30)  times = 0;
    }
    else times = 0;

    Key_TimeBuf[4] = times;
    pKey_TimeBuf[4] = ptimes;

    return DownJudje_Ret;
}

static uint8_t Key_OK_KeyStatusCheck(uint8_t* Key_TimeBuf, uint8_t* pKey_TimeBuf, uint8_t* Key_SelfLock, StructDef_KEY* StructDef_KEY_Buf)
{
    uint8_t KeyStatus_Ret = 0;
    uint8_t times = 0, ptimes = 0;
    uint8_t Self_Lock = 0;

    times = Key_TimeBuf[4];
    ptimes = pKey_TimeBuf[4];
    Self_Lock = Key_SelfLock[4];

    if(times == 0)
    {
        if(ptimes != 0)
        {
#if DEBUG_KEY
            printf("Key_OK ptimes = %d\r\n", ptimes);
#endif
            if(ptimes >= 30)
             {
                 if(pKey_TimeBuf[2]) /* 判断 KEY_UP*/
                 {
                     pKey_TimeBuf[2] = 0;
                     Key_TimeBuf[2] = 0;
                     Key_SelfLock[2] = 1;
                     StructDef_KEY_Buf->KEY_OK.KEY_Status = KEY_COMBINATE0;
                 }
                 else if(pKey_TimeBuf[1]) /* 判断 KEY_DOWN*/
                 {
                     pKey_TimeBuf[1] = 0;
                     Key_TimeBuf[1] = 0;
                     Key_SelfLock[1] = 1;
                     StructDef_KEY_Buf->KEY_OK.KEY_Status = KEY_COMBINATE1;
                 }
                 else
                 {
                     StructDef_KEY_Buf->KEY_OK.KEY_Status = KEY_LONG_DOWN;
                 }
             }
             else
             {
                 StructDef_KEY_Buf->KEY_OK.KEY_Status = KEY_SHORT_DOWN; 
             }

            KeyStatus_Ret = 1;
            Self_Lock = 1;
            ptimes = 0; 
        }
    }

    pKey_TimeBuf[4] = ptimes;
    Key_SelfLock[4] = Self_Lock;

    return KeyStatus_Ret;
}

static void Key_CHARGE_KeyScan(uint8_t* Key_TimeBuf, uint8_t* Key_SelfLock)
{
    uint8_t times = 0;
    uint8_t Self_Lock = 0;

    Self_Lock = Key_SelfLock[5];
    times = Key_TimeBuf[5];

    if(CHARGE_KEY == PIN_RESET)
    {
        vTaskDelay(10);
        if(CHARGE_KEY == PIN_RESET)
        {
            if(Self_Lock == 0) times++;
        }
    } 
    else 
    {
#if DEBUG_KEY
        if(times)
            printf("Key_Charge 按下时间：%d *100ms\r\n\r\n", times);
#endif
        Self_Lock = 0;
    }

    Key_SelfLock[5] = Self_Lock;
    Key_TimeBuf[5] = times;
}

static uint8_t Key_CHARGE_KeyDownJudje(uint8_t* Key_TimeBuf, uint8_t* pKey_TimeBuf, StructDef_KEY* StructDef_KEY_Buf)
{
    uint8_t times = 0, ptimes = 0;
    uint8_t DownJudje_Ret = 0;

    times = Key_TimeBuf[5];
    ptimes = pKey_TimeBuf[5];

    if(ptimes != times)
    {
       ptimes = times;
       if(times >= 30)  times = 0;
    }
    else times = 0;

    Key_TimeBuf[5] = times;
    pKey_TimeBuf[5] = ptimes;

    return DownJudje_Ret;
}

static uint8_t Key_CHARGE_KeyStatusCheck(uint8_t* Key_TimeBuf, uint8_t* pKey_TimeBuf, uint8_t* Key_SelfLock, StructDef_KEY* StructDef_KEY_Buf)
{
    uint8_t KeyStatus_Ret = 0;
    uint8_t times = 0, ptimes = 0;
    uint8_t Self_Lock = 0;

    times = Key_TimeBuf[5];
    ptimes = pKey_TimeBuf[5];
    Self_Lock = Key_SelfLock[5];

    if(times == 0)
    {
        if(ptimes != 0)
        {
#if DEBUG_KEY
            printf("KEY_Charge ptimes = %d\r\n", ptimes);
#endif
            if(ptimes >= 30)
             {
                 StructDef_KEY_Buf->KEY_CHARGE.KEY_Status = KEY_LONG_DOWN;
             }
             else
             {
                 StructDef_KEY_Buf->KEY_CHARGE.KEY_Status = KEY_SHORT_DOWN; 
             }

            KeyStatus_Ret = 1;
            Self_Lock = 1;
            ptimes = 0; 
        }
    }

    pKey_TimeBuf[5] = ptimes;
    Key_SelfLock[5] = Self_Lock;

    return KeyStatus_Ret;
}

static void XL9555_KeyScan(uint8_t* Key_TimeBuf, uint8_t* Key_SelfLock)
{
    uint8_t i = 0;
    uint8_t XL9555_ScanData = 0xFF;
    static uint16_t Times = 0;
    static uint8_t printfTimes = 0;

    printfTimes++;
    if(printfTimes == 10)
    {
        printfTimes = 0;
        printf("Input_Reg_0 输入为 0x%02X\r\n", XL9555_Read_IO(Input_Reg_0));
        printf("Input_Reg_1 输入为 0x%02X\r\n", XL9555_Read_IO(Input_Reg_1));
        printf("Output_Reg_0 输出为 0x%02X\r\n", XL9555_Read_IO(Output_Reg_0));
        printf("Output_Reg_1 输出为 0x%02X\r\n", XL9555_Read_IO(Output_Reg_1));
    }

    XL9555_ScanData = XL9555_Read_IO(Input_Reg_1);
    if(XL9555_ScanData != 0xFF)
    {
        /* 消抖 */
        vTaskDelay(10);
        XL9555_ScanData = XL9555_Read_IO(Input_Reg_1);

        if(XL9555_ScanData != 0xFF)
        {
            for(i=0; i<4; i++)
            {
                if((0x08 & XL9555_ScanData) == 0)
                {
                    if(Key_SelfLock[i] == 0)
                    {          
                        Times++;
                        Key_TimeBuf[i]++;
                    }
                }
                else
                {
                    Key_SelfLock[i] = 0;
                }
                XL9555_ScanData <<= 1;
            }
        }
    }
    else
    {
        memset(Key_SelfLock, 0, 4);
        if(Times)
        {
#if DEBUG_KEY
            printf("按下时间：%d *100ms\r\n\r\n", Times);
#endif
            Times = 0;
        }
    }
}

static uint8_t XL9555_KeyDownJudje(uint8_t* Key_TimeBuf, uint8_t* pKey_TimeBuf, StructDef_KEY* StructDef_KEY_Buf)
{
    uint8_t i = 0;
    uint8_t DownJudje_Ret = 0;
    static uint8_t SOS_Status = 0, PTT_Status = 0, UP_Status = 0, Down_Status = 0;
    
    for(i=0; i<4; i++)
    {
        if(pKey_TimeBuf[i] != Key_TimeBuf[i])
        {
            pKey_TimeBuf[i] = Key_TimeBuf[i];
            switch(i)
            {
                case 0: /*KEY_SOS*/
                        if(Key_TimeBuf[i] >= 20)
                        {
                            if(SOS_Status == 0)
                            {
                                SOS_Status = 1;
                                StructDef_KEY_Buf->KEY_SOS.KEY_Status = KEY_LONG_DOWN; 
                                DownJudje_Ret++;
                            }
                            if(Key_TimeBuf[i] >= 100)
                            {
                                Key_TimeBuf[i] = 0;
                            }
                        }  
                                                                                        break;
                case 1: /*KEY_DOWN*/
                        if(Down_Status == 0)
                        {
                            if(Key_TimeBuf[i] >= 30)  
                            {
                                Key_TimeBuf[i] = 0; 
                                Down_Status = 1;
                            }
                        }
                        else
                        {
                            if(Key_TimeBuf[i] >= 10)
                            {
                                Key_TimeBuf[i] = 0; 
                                pKey_TimeBuf[i] = 30;
                            }
                        } 
                                                                                        break;
                case 2: /*KEY_UP*/
                        if(UP_Status == 0)
                        {
                            if(Key_TimeBuf[i] >= 30)
                            {   
                                Key_TimeBuf[i] = 0; 
                                UP_Status = 1;
                            }
                        }
                        else
                        {
                            if(Key_TimeBuf[i] >= 10)
                            {
                                Key_TimeBuf[i] = 0; 
                                pKey_TimeBuf[i] = 30;
                            }
                        }
                                                                                         break;
                case 3: /*KEY_PTT*/
                        if(Key_TimeBuf[i] >= 5)                                                
                        {
                            if(PTT_Status == 0)
                            {
                                PTT_Status = 1;
                                StructDef_KEY_Buf->KEY_PTT.KEY_Status = KEY_SHORT_DOWN; 
                                DownJudje_Ret++;
                            }
                            if(Key_TimeBuf[i] > 150)
                            {
                                Key_TimeBuf[i] = 0; 
                                PTT_Status = 0;
                            }
                        }
                                                                                         break;
                default: break;
            }
        }
        else
        {
            Key_TimeBuf[i] = 0;

            if(i == 0)
                SOS_Status = 0;

            if((i == 2) && (UP_Status))
            {
                UP_Status = 0;
                pKey_TimeBuf[i] = 0;
            }

            if((i == 1) && (Down_Status))
            {
                Down_Status = 0;
                pKey_TimeBuf[i] = 0;
            }

            if(i == 3)
                PTT_Status = 0;
        }
    }

    return DownJudje_Ret;
}

static uint8_t XL9555_KeyStatusCheck(uint8_t* Key_TimeBuf, uint8_t* pKey_TimeBuf, uint8_t* Key_SelfLock, StructDef_KEY* StructDef_KEY_Buf)
{
    uint8_t i = 0;
    uint8_t KeyStatus_Ret = 0;

    for(i=0; i<4; i++)
    {
        if(Key_TimeBuf[i] == 0)
        {
            if(pKey_TimeBuf[i] != 0)
            {
                switch(i)
                {
                    case 0: /* KEY_SOS */
                            if(pKey_TimeBuf[i] >= 20)
                            {
                                if (pKey_TimeBuf[i] >= 100)
                                {
                                    StructDef_KEY_Buf->KEY_SOS.KEY_Status = KEY_LONG_LONG_DOWN;
                                    KeyStatus_Ret++;
                                }
                            }

                    break;

                    case 1: /* KEY_DOWN */
                            if(pKey_TimeBuf[i] >= 30)
                            {
                                if(pKey_TimeBuf[2]) /* 判断 KEY_UP*/
                                {
                                    pKey_TimeBuf[2] = 0;
                                    Key_TimeBuf[2] = 0;
                                    Key_SelfLock[2] = 1;
                                    StructDef_KEY_Buf->KEY_DOWN.KEY_Status = KEY_COMBINATE0;
                                }
                                else if(pKey_TimeBuf[4]) /* 判断 KEY_OK*/
                                {
                                    pKey_TimeBuf[4] = 0;
                                    Key_TimeBuf[4] = 0;
                                    Key_SelfLock[4] = 1;
                                    StructDef_KEY_Buf->KEY_DOWN.KEY_Status = KEY_COMBINATE1;
                                }
                                else
                                {
                                    StructDef_KEY_Buf->KEY_DOWN.KEY_Status = KEY_LONG_DOWN;
                                }
                            }
                            else
                            {
                                StructDef_KEY_Buf->KEY_DOWN.KEY_Status = KEY_SHORT_DOWN; 
                            }

                            KeyStatus_Ret++;
                    break;

                    case 2: /* KEY_UP */
                            if(pKey_TimeBuf[i] >= 30)
                            {
                                if(pKey_TimeBuf[1]) /* 判断 KEY_DOWN*/
                                {
                                    pKey_TimeBuf[1] = 0;
                                    Key_TimeBuf[1] = 0;
                                    Key_SelfLock[1] = 1;
                                    StructDef_KEY_Buf->KEY_UP.KEY_Status = KEY_COMBINATE0;
                                }
                                else if(pKey_TimeBuf[4]) /* 判断 KEY_OK*/
                                {
                                    pKey_TimeBuf[4] = 0;
                                    Key_TimeBuf[4] = 0;
                                    Key_SelfLock[4] = 1;
                                    StructDef_KEY_Buf->KEY_UP.KEY_Status = KEY_COMBINATE1;
                                }
                                else
                                {
                                    StructDef_KEY_Buf->KEY_UP.KEY_Status = KEY_LONG_DOWN;
                                }
                            }
                            else
                            {
                                StructDef_KEY_Buf->KEY_UP.KEY_Status = KEY_SHORT_DOWN; 
                            }

                            KeyStatus_Ret++;
                    break;

                    case 3: /* KEY_PTT */
                            if(pKey_TimeBuf[i] >= 5)  
                            {
                                if(pKey_TimeBuf[i] >= 150)
                                    StructDef_KEY_Buf->KEY_PTT.KEY_Status = KEY_TIMEOUT;

                                KeyStatus_Ret++;
                            }
                            else
                            {
                                /* 按键时间低于0.5s 默认为抖动 */
                                if(KeyStatus_Ret) KeyStatus_Ret--;
                            }
                    break;
                    default: break;
                }

                pKey_TimeBuf[i] = 0;
                Key_SelfLock[i] = 1;

                if(i == 1)
                {
                    if(StructDef_KEY_Buf->KEY_DOWN.KEY_Status == KEY_LONG_DOWN)
                    {
                        Key_SelfLock[i] = 0;
                    }
                }
                if(i == 2)
                {
                    if(StructDef_KEY_Buf->KEY_UP.KEY_Status == KEY_LONG_DOWN)
                    {
                        Key_SelfLock[i] = 0;
                    }
                }
            }
        }
    }

    return KeyStatus_Ret;
}

static void KeyScan(uint8_t* Key_TimeBuf, uint8_t* Key_SelfLock)
{
    Key_OK_KeyScan(Key_TimeBuf, Key_SelfLock);
    Key_CHARGE_KeyScan(Key_TimeBuf, Key_SelfLock);
    XL9555_KeyScan(Key_TimeBuf, Key_SelfLock);
}

static uint8_t KeyDownJudje(uint8_t* Key_TimeBuf, uint8_t* pKey_TimeBuf, StructDef_KEY* StructDef_KEY_Buf)
{
    uint8_t DownJudje_Ret = 0;

    DownJudje_Ret += Key_OK_KeyDownJudje(Key_TimeBuf, pKey_TimeBuf, StructDef_KEY_Buf);
    DownJudje_Ret += Key_CHARGE_KeyDownJudje(Key_TimeBuf, pKey_TimeBuf, StructDef_KEY_Buf);
    DownJudje_Ret += XL9555_KeyDownJudje(Key_TimeBuf, pKey_TimeBuf, StructDef_KEY_Buf);

    return DownJudje_Ret;
}

static uint8_t KeyStatusCheck(uint8_t* Key_TimeBuf, uint8_t* pKey_TimeBuf, uint8_t* Key_SelfLock, StructDef_KEY* StructDef_KEY_Buf)
{
    uint8_t KeyStatus_Ret = 0;

    KeyStatus_Ret += Key_OK_KeyStatusCheck(Key_TimeBuf, pKey_TimeBuf, Key_SelfLock, StructDef_KEY_Buf);
    KeyStatus_Ret += Key_CHARGE_KeyStatusCheck(Key_TimeBuf, pKey_TimeBuf, Key_SelfLock, StructDef_KEY_Buf);
    KeyStatus_Ret += XL9555_KeyStatusCheck(Key_TimeBuf, pKey_TimeBuf, Key_SelfLock, StructDef_KEY_Buf);

    return KeyStatus_Ret;
}

uint8_t Total_KeyScan(StructDef_KEY* StructDef_KEY_Buf)
{
    uint8_t KeyStatus_Ret = 0;
    static uint8_t Key_Down_Times[6] = {0}, pKey_Down_Times[6] = {0};
    static uint8_t Key_SelfLock[6] = {0};

    KeyScan(Key_Down_Times, Key_SelfLock);
    KeyStatus_Ret = KeyDownJudje(Key_Down_Times, pKey_Down_Times, StructDef_KEY_Buf);
    KeyStatus_Ret += KeyStatusCheck(Key_Down_Times, pKey_Down_Times, Key_SelfLock, StructDef_KEY_Buf);

    return KeyStatus_Ret;
}

void Key_Init(void)
{
    Single_key_init();
}