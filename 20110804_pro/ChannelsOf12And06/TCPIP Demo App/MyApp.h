#ifndef __MyApp_H
#define __MyApp_H

#define BOARDVER 		"CCT"	
//#define POWER_SUPPLY_12
//#define POWER_SUPPLY_12_1602
#define POWER_SUPPLY_6
#define STACK_USE_DS1302
//#define STACK_USE_RF315
//#define STACK_USE_IICEEPROM
//#define STACK_USE_DS18B20
//#define STACK_USE_BUZZER     //定时器3中断使用开关
#define STACK_USE_RS485

void PerDevice_Init(void);

#if defined(STACK_USE_DS1302)
	#include "DS1302.h"
#endif

  #if defined(STACK_USE_RF315)
	#include "RF315.h"
#endif

#if defined(STACK_USE_IICEEPROM)
	#include "IICEEPROM.h"
#endif

#if defined(STACK_USE_DS18B20)
	#include "DS18B20.h"
#endif

#if defined(STACK_USE_BUZZER)
//	#include "Buzzer.h"
	#include "ClockUse.h"
#endif
#if defined(STACK_USE_RS485)
	#include "RS485.h"
#endif
//***************************************
#if defined(STACK_USE_DS1302)
	void DisplayTime(void);	
#endif

#if defined(STACK_USE_DS18B20)
	void DisplayTemperature(void);
#endif

#if defined(STACK_USE_IICEEPROM)
	void BuildDateSave(void);
	void UploadBuildDate(void);
#endif



#endif
