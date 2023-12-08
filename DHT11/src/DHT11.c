#include"DHT11.h"
float tCelsius = 0;
float RH = 0;
void DHT11_Init(DHT11_Name* DHT,GPIO_TypeDef* DHT_PORT,uint16_t DHT_PIN,TIM_HandleTypeDef* DHT_TIMER){
	DHT->DHT_PORT = DHT_PORT;
	DHT->DHT_PIN = DHT_PIN;
	DHT->DHT_TIMER = DHT_TIMER;
	HAL_TIM_Base_Start(DHT->DHT_TIMER);
}
static void microDelay (DHT11_Name* DHT,uint16_t delay)
{
  __HAL_TIM_SET_COUNTER(DHT->DHT_TIMER, 0);
  while (__HAL_TIM_GET_COUNTER(DHT->DHT_TIMER) < delay);
}
static uint8_t DHT11_Read (DHT11_Name* DHT)
{
  uint8_t a,b;
  for (a=0;a<8;a++)
  {
    DHT->pMillis = HAL_GetTick();
    DHT->cMillis = HAL_GetTick();
    while (!(HAL_GPIO_ReadPin (DHT->DHT_PORT, DHT->DHT_PIN)) && DHT->pMillis + 2 > DHT->cMillis)
    {  // wait for the pin to go high
      DHT->cMillis = HAL_GetTick();
    }
    microDelay(DHT,40);   // wait for 40 us
    if (!(HAL_GPIO_ReadPin (DHT->DHT_PORT,DHT->DHT_PIN)))   // if the pin is low
      b&= ~(1<<(7-a));
    else
      b|= (1<<(7-a));
    DHT->pMillis = HAL_GetTick();
    DHT->cMillis = HAL_GetTick();
    while ((HAL_GPIO_ReadPin (DHT->DHT_PORT,DHT->DHT_PIN)) && DHT->pMillis + 2 > DHT->cMillis)
    {  // wait for the pin to go low
      DHT->cMillis = HAL_GetTick();
    }
  }
  return b;
}
void DHT11_output(DHT11_Name* DHT){
	DHT->RHI = DHT11_Read(DHT); // Relative humidity integral
	DHT->RHD = DHT11_Read(DHT); // Relative humidity decimal
	DHT->TCI = DHT11_Read(DHT); // Celsius integral
	DHT->TCD = DHT11_Read(DHT); // Celsius decimal
	DHT->SUM = DHT11_Read(DHT); // Check sum
	 if (DHT->RHI + DHT->RHD + DHT->TCI + DHT->TCD == DHT->SUM)
	 {
	        // Can use RHI and TCI for any purposes if whole number only needed
	        tCelsius = (float)DHT->TCI + (float)(DHT->TCD/10.0);
	        //tFahrenheit = tCelsius * 9/5 + 32;
	        RH = (float)DHT->RHI + (float)(DHT->RHD/10.0);
	        // Can use tCelsius, tFahrenheit and RH for any purposes
	 }
}
uint8_t DHT11_Start(DHT11_Name* DHT)
{
  uint8_t Response = 0;
  GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
  GPIO_InitStructPrivate.Pin = DHT->DHT_PIN;
  GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT->DHT_PORT, &GPIO_InitStructPrivate); // set the pin as output
  HAL_GPIO_WritePin (DHT->DHT_PORT,DHT->DHT_PIN, 0);   // pull the pin low
  HAL_Delay(20);   // wait for 20ms
  HAL_GPIO_WritePin(DHT->DHT_PORT,DHT->DHT_PIN,1);   // pull the pin high
  microDelay(DHT,30);   // wait for 30us
  GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT->DHT_PORT, &GPIO_InitStructPrivate); // set the pin as input
  microDelay(DHT,40);
  if (!(HAL_GPIO_ReadPin (DHT->DHT_PORT, DHT->DHT_PIN)))
  {
    microDelay(DHT,80);
    if ((HAL_GPIO_ReadPin (DHT->DHT_PORT, DHT->DHT_PIN))) Response = 1;
  }
  DHT->pMillis = HAL_GetTick();
  DHT->cMillis = HAL_GetTick();
  while ((HAL_GPIO_ReadPin (DHT->DHT_PORT, DHT->DHT_PIN)) && DHT->pMillis + 2 > DHT->cMillis)
  {
    DHT->cMillis = HAL_GetTick();
  }
  return Response;
}
