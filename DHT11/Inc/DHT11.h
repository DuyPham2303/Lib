#ifndef INC_DHT11_H_
#define INC_DHT11_H_
#include"stm32f1xx_hal.h"
typedef struct{
	GPIO_TypeDef* DHT_PORT;
	uint16_t DHT_PIN;
	TIM_HandleTypeDef* DHT_TIMER;
	uint8_t RHI, RHD, TCI, TCD, SUM;
	uint32_t pMillis, cMillis;
}DHT11_Name;
void DHT11_Init(DHT11_Name* DHT,GPIO_TypeDef* DHT_PORT,uint16_t DHT_PIN,TIM_HandleTypeDef* DHT_TIMER);
void DHT11_output(DHT11_Name* DHT);
uint8_t DHT11_Start(DHT11_Name* DHT);
#endif /* INC_DHT11_H_ */
