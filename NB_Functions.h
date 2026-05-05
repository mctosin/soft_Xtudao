#pragma once
#ifndef NB_FUNCTIONS_H
#define NB_FUNCTIONS_H

//#include "main.h"

#include "stm32f4xx_hal.h"


#define BOT_B_Pin GPIO_PIN_0
#define BOT_B_GPIO_Port GPIOA
#define LED_G_Pin GPIO_PIN_12
#define LED_G_GPIO_Port GPIOD
#define LED_O_Pin GPIO_PIN_13
#define LED_O_GPIO_Port GPIOD
#define LED_R_Pin GPIO_PIN_14
#define LED_R_GPIO_Port GPIOD
#define LED_B_Pin GPIO_PIN_15
#define LED_B_GPIO_Port GPIOD

typedef enum
{
	off = 0,
	on  = 1
}type_on_off;

typedef enum
{
	False = 0,
	True  = 1
}type_bool;

typedef struct
{
	uint32_t initial_time;
	uint32_t elapsed_time;
	uint32_t delay_time;
}type_ST; // tipo soft timer

typedef enum
{
	Inactive = 0,
	Active   = 1
}type_bool_state;

typedef struct
{
	type_bool_state state;
	uint32_t T;
	uint32_t t_act;
	uint32_t t_ina;
	float duty_cycle;
	GPIO_TypeDef *Port;
	uint16_t Pin;
	type_ST timer;
	uint32_t T_shadow;
	float duty_cicle_shadow;
	type_bool_state shadow;

}type_PWM; // tipo PWM

typedef enum
{
	Detecting           = 0,
	Possible_transition = 1,
	Detected            = 2
} type_transition_state;
typedef enum
{
	fall = 0,
	rise = 1
}type_transition_type;

typedef struct
{
	type_bool_state atu;
	type_bool_state ant;
	type_transition_state state;
	type_ST timer_db;
	int time_debounce;
	GPIO_TypeDef *Port;
	uint16_t Pin;
	type_transition_type edge;

} type_transition;

void ST_Init(type_ST *pST, uint32_t time_lapse);
type_bool ST(type_ST *pST);
void ST_Lapse(type_ST *pST);

void PWM_Run(type_PWM *pPWM);
void PWM_Init(type_PWM *pPWM,
	GPIO_TypeDef* GPIO_Port,
	uint16_t GPIO_Pin, 
	uint32_t Period,
	float Duty); 
void PWM_Update(type_PWM *pPWM,
	uint32_t Period, 
	float Duty,
	type_bool_state shadow);
type_bool transition_detection(type_transition *trans);
void transition_detection_init(type_transition *trans, 
	type_transition_type edge,
	GPIO_TypeDef *Port,
	uint16_t Pin,
	int time_debounce);


#endif
