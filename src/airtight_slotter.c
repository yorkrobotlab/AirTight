/**
 * @addtogroup Airtight_Slotter
 * @{
 * @file
 * AirTight: core loop functions firing scheduled slot actions implementation.
 */
#include "airtight_slotter.h"

/**
 * Perform core event loop of AirTight.
 *
 * In order:
 *  - Checks alarms,
 *  - Checks for slot and triggers slot if reached.
 *  - Ensures at least 1ms has passed.
 */
void Airtight_Slotter_SingleThreadedSlotterTick(Airtight_MACState *mac_state,
                                                at_u8_t *slot,
                                                at_time_t *counter)
{
    if (Airtight_Time_CheckAlarm(&mac_state->fault_alarm))
    {
        Airtight_ClearFault(mac_state);
    }

    const at_time_t current_time = Airtight_Time_GetSynchronisedTime(&mac_state->time);
    if ((current_time / AT_CONF_SLOT_LENGTH_MS) > *counter)
    {
        AT_DEBUG("Slotter: doing slot.");
        (*slot)++;
        if (*slot >= AT_CONF_SLOT_TABLE_ROWS)
            *slot = 0;
        Airtight_DoSlot(mac_state, *slot);
        *counter = current_time / AT_CONF_SLOT_LENGTH_MS;
    }

    // We make sure a full ms has passed as our time is monotonic and could be artificially
    // inflated if the slotter loop runs too quickly!
    Airtight_Time_1ms();
}
