#include "NB_Functions.h"

void ST_Init(type_ST *pST, uint32_t time_lapse)
{
	pST->initial_time = HAL_GetTick();
	pST->elapsed_time = 0;
	pST->delay_time = time_lapse;
}
type_bool ST(type_ST *pST)
{
	pST->elapsed_time = HAL_GetTick() - pST->initial_time;
	if (pST->elapsed_time >= pST->delay_time)
	{
		return True;
	}
	else return False;
}
void ST_Lapse(type_ST *pST)
{
	pST->initial_time = pST->initial_time + pST->delay_time;
	pST->elapsed_time = 0;
}

void PWM_Init(type_PWM *pPWM,
	GPIO_TypeDef* GPIO_Port,
	uint16_t GPIO_Pin, 
	uint32_t Period,
	float Duty)
{
	pPWM->T = Period;
	if (Duty <= 0.0f)
		pPWM->duty_cycle = 0;
	else if (Duty >= 1.0f)
		pPWM->duty_cycle = 1.0;
	else
		pPWM->duty_cycle = Duty;
	pPWM->t_act = pPWM->T * pPWM->duty_cycle;
	pPWM->t_ina = pPWM->T - pPWM->t_act;
	
	pPWM->Port = GPIO_Port;
	pPWM->Pin = GPIO_Pin;
	
	pPWM->T_shadow = pPWM->T;
	pPWM->duty_cicle_shadow = pPWM->duty_cycle;
	pPWM->shadow = Inactive;
	
	pPWM->state = Active;
	ST_Init(&pPWM->timer, pPWM->t_act);
	HAL_GPIO_WritePin(pPWM->Port, pPWM->Pin, GPIO_PIN_SET);
}

void PWM_Run(type_PWM *pPWM)
{
	if (ST(&pPWM->timer))
	{
		ST_Lapse(&pPWM->timer);
		if (pPWM->state == Active)
		{
			pPWM->state = Inactive;
			HAL_GPIO_WritePin(pPWM->Port, pPWM->Pin, GPIO_PIN_RESET);
			pPWM->timer.delay_time = pPWM->t_ina;
		}
		else
		{
			pPWM->state = Active;
			if (pPWM->shadow)
			{
				pPWM->duty_cycle = pPWM->duty_cicle_shadow;
				pPWM->T = pPWM->T_shadow;
				pPWM->t_act = pPWM->T * pPWM->duty_cycle;
				pPWM->t_ina = pPWM->T - pPWM->t_act;
			}
			HAL_GPIO_WritePin(pPWM->Port, pPWM->Pin, GPIO_PIN_SET);
			pPWM->timer.delay_time = pPWM->t_act;

		}
	}
}

void PWM_Update(type_PWM *pPWM,
	uint32_t Period, 
	float Duty,
	type_bool_state shadow)
{
#if (0)
	pPWM->T = Period;
	if (Duty <= 0.0f)
		pPWM->duty_cycle = 0;
	else if (Duty >= 1.0f)
		pPWM->duty_cycle = 1.0;
	else
		pPWM->duty_cycle = Duty;
	pPWM->t_act = pPWM->T * pPWM->duty_cycle;
	pPWM->t_ina = pPWM->T - pPWM->t_act;
	
	pPWM->T_shadow = pPWM->T;
	pPWM->duty_cicle_shadow = pPWM->duty_cycle;
#endif
	
	pPWM->shadow = shadow;
	pPWM->T_shadow = Period;
	if (Duty <= 0.0f)
		pPWM->duty_cicle_shadow = 0;
	else if (Duty >= 1.0f)
		pPWM->duty_cicle_shadow = 1.0;
	else
		pPWM->duty_cicle_shadow = Duty;	
	if (!shadow)
	{
		pPWM->duty_cycle = pPWM->duty_cicle_shadow;
		pPWM->T = pPWM->T_shadow;
		pPWM->t_act = pPWM->T * pPWM->duty_cycle;
		pPWM->t_ina = pPWM->T - pPWM->t_act;
	}
}

type_bool transition_detection(type_transition *trans)
{
	trans->atu = (type_bool_state)HAL_GPIO_ReadPin(trans->Port, trans->Pin);
	if (trans->state == Detecting)
	{
		if (trans->edge == rise)
		{
			if (trans->atu == Active)
			{
				if (trans->ant == Inactive)
				{
					// Saboooor transição
					ST_Init(&(trans->timer_db), trans->time_debounce);
					trans->state = Possible_transition;
				}			
			}
		}
		else // fall
		{
			if (trans->atu == Inactive)
			{
				if (trans->ant == Active)
				{
					// Saboooor transição
					ST_Init(&(trans->timer_db), trans->time_debounce);
					trans->state = Possible_transition;
				}			
			}
		}
		
		trans->ant = trans->atu;
	} 
	else if (trans->state == Possible_transition)
	{
		if (ST(&(trans->timer_db)))
		{
			if (trans->edge == rise)
			{
				if (trans->atu == Active)
				{
					trans->state = Detected;
					return True;
				}
				else
				{
					trans->state = Detecting;
				}
			}
			else //fall
			{
				if (trans->atu == Inactive)
				{
					trans->state = Detected;
					return True;
				}
				else
				{
					trans->state = Detecting;
				}
			}
		}
			
	}
	else // Detected
	{
		trans->state = Detecting;
	}
	
	return False;
}
void transition_detection_init(type_transition *trans, 
	type_transition_type edge,
	GPIO_TypeDef *Port,
	uint16_t Pin,
	int time_debounce)
{
	trans->Port = Port;
	trans->Pin = Pin;
	trans->time_debounce = time_debounce;
	trans->state = Detecting;
	trans->edge = edge;
	if (trans->edge == rise)
	{
		trans->ant = Active;
		trans->atu = Active;
	}
	else // fall
	{
		trans->ant = Inactive;
		trans->atu = Inactive;
	}
	
}