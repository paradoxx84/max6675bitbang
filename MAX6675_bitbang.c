#include "max6675_bitbang.h"

void Max6675_bitbang_Init(max6675_bitbang_t* arg, 
                          GPIO_TypeDef* clk_port, uint16_t clk_pin, 
                          GPIO_TypeDef* do_port, uint16_t do_pin, 
                          GPIO_TypeDef* cs_port, uint16_t cs_pin, 
                          uint64_t(*uptimeptr)(void))
{
    arg->uptimeptr = uptimeptr;
    arg->NextProcess = arg->uptimeptr() + CONVERSION_INTERVAL;
    
    arg->Clk_port = clk_port;
    arg->Clk_pin = clk_pin;
    
    arg->DO_port = do_port;
    arg->DO_pin = do_pin;
    
    arg->CS_port = cs_port;
    arg->CS_pin = cs_pin;
    
    arg->State = max6675_Idle;
}

void Max6675_bitbang_Process(max6675_bitbang_t* arg)
{
    uint64_t currenttime = arg->uptimeptr();
    if(currenttime >= arg->NextProcess)
    {
        switch(arg->State)
        {
            case max6675_Idle:
            {                               
                arg->NextProcess += 1;
                arg->Idx = 0;
                arg->uTemp = 0;
                HAL_GPIO_WritePin(arg->CS_port, arg->CS_pin, GPIO_PIN_RESET);
                arg->State = max6675_ClockUp;               
                break;
            }
            
            case max6675_ClockUp:
            {
                arg->NextProcess += 1;
                HAL_GPIO_WritePin(arg->Clk_port, arg->Clk_pin, GPIO_PIN_SET);
                arg->State = max6675_ClockDn;                
                break;
            }
            
            case max6675_ClockDn:
            {                  
                uint8_t dat = HAL_GPIO_ReadPin(arg->DO_port, arg->DO_pin);
                HAL_GPIO_WritePin(arg->Clk_port, arg->Clk_pin, GPIO_PIN_RESET);
                arg->uTemp = (arg->uTemp << 1) + dat;                
                arg->Idx++;
                if(arg->Idx == 16)
                {
                    arg->NextProcess += CONVERSION_INTERVAL;
                    if(arg->uTemp & 0x0002)
                    {
                        arg->Chip_OK = 0;
                    }else{
                        arg->Chip_OK = 1;
                    }
                    
                    if(arg->uTemp & 0x0004)
                    {
                        arg->Sensor_OK = 0;
                    }else{
                        arg->Sensor_OK = 1;
                    }
                    
                    arg->uTemp = (arg->uTemp >> 3) & 0x0FFF;
                    arg->fTemp = arg->uTemp * 1023.75f / 0x0FFF;
                    
                    HAL_GPIO_WritePin(arg->CS_port, arg->CS_pin, GPIO_PIN_SET);
                    arg->State = max6675_Idle;
                }else{
                    arg->NextProcess += 1;
                    arg->State = max6675_ClockUp;
                }                
                break;
            }
        }
    }
}

float Max6675_bitbang_GetTemperature(max6675_bitbang_t*)
{
    return arg->fTemp;
}
