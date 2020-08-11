/**
 * @addtogroup Airtight_Time
 * @{
 * @file
 * AirTight: time related functions including alarms and clock header.
 */
#ifndef __AIRTIGHT_TIME_H
#define __AIRTIGHT_TIME_H

#include "airtight_types.h"
#include <time.h>

/**
 * Synchronised time struct, used with Airtight_Time_GetSynchronisedTime.
 */
typedef struct
{
    at_time_t base_local;
    at_time_t offset;
} Airtight_Time;

/**
 * Alarm struct for storing an alarm.
 */
typedef struct
{
    at_time_t start;
    at_time_t at;
    at_bool_t set;
} Airtight_Alarm;

void Airtight_Time_Init(Airtight_Time *time);
at_time_t Airtight_Time_GetSynchronisedTime(Airtight_Time *time);
void Airtight_Time_1ms();

void Airtight_Time_SetAlarm(Airtight_Alarm *alarm, at_time_t in);
void Airtight_Time_ClearAlarm(Airtight_Alarm *alarm);
at_bool_t Airtight_Time_CheckAlarm(Airtight_Alarm *alarm);

/**
 * Initialise an Airtight_Alarm.
 *
 * @see Airtight_Time_ClearAlarm
 */
#define Airtight_Time_InitAlarm Airtight_Time_ClearAlarm

#endif
