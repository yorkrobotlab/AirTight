/**
 * @addtogroup Airtight_Time
 * @{
 * @file
 * AirTight: time related functions including alarms and clock implementations.
 */
#include "airtight_time.h"

/**
 * Get the local clock in MS.
 */
static inline at_time_t Airtight_Time_ClockMS()
{
    // PORT: some platforms may require a more complex time source.
    return clock() / (CLOCKS_PER_SEC / 1000);
}

/**
 * Get a monotonic local clock in MS.
 *
 * This function's result is monotonically increasing.
 */
static inline at_time_t Airtight_Time_GetLocalMonotonicTime()
{
    static at_time_t last = 0;
    at_time_t current = Airtight_Time_ClockMS();
    if (current <= last)
    {
        current = last + 1;
    }
    last = current;
    return current;
}

/**
 * Initialise a Airtight_Time struct.
 */
void Airtight_Time_Init(Airtight_Time *time)
{
    at_time_t initial = Airtight_Time_GetLocalMonotonicTime();
    time->base_local = initial;
    time->offset = 0;
}

/**
 * Get the current synchronised time.
 *
 * This function's result is monotonically increasing.
 *
 * @return the current time.
 */
at_time_t Airtight_Time_GetSynchronisedTime(Airtight_Time *time)
{
    return Airtight_Time_GetLocalMonotonicTime() + time->offset - time->base_local;
}

/**
 * Synchronise time using a reference value.
 */
void Airtight_Time_SetSynchronisationPoint(Airtight_Time *time, at_time_t sync_time)
{
    at_time_t current = Airtight_Time_ClockMS() + time->offset - time->base_local;
    time->offset += sync_time - current;
}

/**
 * Wait for at least 1 millisecond.
 *
 * This is useful when used with any function which gets it's time from
 * Airtight_Time_GetLocalMonotonicTime as calling it at a faster rate than
 * 1ms will lead to artificial time inflation.
 */
void Airtight_Time_1ms()
{
    const at_time_t start = Airtight_Time_ClockMS();
    while (true)
    {
        const at_time_t current = Airtight_Time_ClockMS();
        if (current != start)
        {
            break;
        }
    }
}

// PORT: Alarms may be implemented in hardware on platforms which support this.

/**
 * Set an alarm on a Airtight_Alarm structure.
 */
void Airtight_Time_SetAlarm(Airtight_Alarm *alarm, at_time_t in)
{
    alarm->start = Airtight_Time_ClockMS();
    alarm->at = alarm->start + in;
    alarm->set = true;
}

/**
 * Clear an alarm on a Airtight_Alarm structure.
 */
void Airtight_Time_ClearAlarm(Airtight_Alarm *alarm)
{
    alarm->set = false;
}

/**
 * Check an alarm on a Airtight_Alarm structure.
 *
 * @return true if the alarm fired, false otherwise
 * @note After firing the alarm is unset so further calls will return false.
 */
at_bool_t Airtight_Time_CheckAlarm(Airtight_Alarm *alarm)
{
    if (!alarm->set)
    {
        return false;
    }
    const at_time_t now = Airtight_Time_ClockMS();
    if (now >= alarm->at && (alarm->at > alarm->start || now < alarm->start))
    {
        alarm->set = false;

        return true;
    }

    return false;
}
