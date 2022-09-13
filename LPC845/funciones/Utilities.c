/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/

#include "Rgb.h"
#include "Timer.h"
#include "Utilities.h"
#include "Config.h"


/***********************************************************************************************************************************
 *** IMPLEMENTACION DE FUNCIONES PUBLICAS
 **********************************************************************************************************************************/

void AlertPending(void){
	Rgb_SetState(RGB_RED_LED, 0);
	Rgb_SetState(RGB_BLUE_LED, 1);
	Rgb_SetState(RGB_GREEN_LED, 0);
}

void AlertOK(void){
	Rgb_SetState(RGB_RED_LED, 0);
	Rgb_SetState(RGB_BLUE_LED, 0);
	Rgb_SetState(RGB_GREEN_LED, 1);
	TIMER_Start(TIMER_ALERT, ALERT_TIMEOUT, SINGLE_SHOT, &AlertIdleState);
}

void AlertFAIL(void){
	Rgb_SetState(RGB_RED_LED, 1);
	Rgb_SetState(RGB_BLUE_LED, 0);
	Rgb_SetState(RGB_GREEN_LED, 0);
	TIMER_Start(TIMER_ALERT, ALERT_TIMEOUT, SINGLE_SHOT, &AlertIdleState);
}

void AlertIdleState(void){
	Rgb_SetState(RGB_RED_LED, 0);
	Rgb_SetState(RGB_BLUE_LED, 0);
	Rgb_SetState(RGB_GREEN_LED, 0);
}
