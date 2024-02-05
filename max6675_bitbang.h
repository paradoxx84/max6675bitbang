#include "stdint.h"
#include "gpio.h"

#define CONVERSION_INTERVAL 220 //ms

typedef enum
{
    max6675_Idle,
    max6675_ClockUp,
    max6675_ClockDn
}max6675_bitbang_state_t;

typedef struct
{
    float fTemp;
    uint8_t Sensor_OK;
    uint8_t Chip_OK;
    max6675_bitbang_state_t State;
    uint64_t NextProcess;
    GPIO_TypeDef* Clk_port;
    uint16_t Clk_pin;
    GPIO_TypeDef* DO_port;
    uint16_t DO_pin;
    GPIO_TypeDef* CS_port;
    uint16_t CS_pin;
    uint8_t Idx;
    uint16_t uTemp;
    uint64_t(*uptimeptr)(void);
}max6675_bitbang_t;

void Max6675_bitbang_Init(max6675_bitbang_t*, GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t, uint64_t(*)(void));
void Max6675_bitbang_Process(max6675_bitbang_t*);
float Max6675_bitbang_GetTemperature(max6675_bitbang_t*);
