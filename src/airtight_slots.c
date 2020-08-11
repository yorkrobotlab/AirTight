/**
 * @addtogroup Airtight_Slots
 * @{
 * @file
 * AirTight: slot table and slot table configuration implementation.
 */
#include "airtight_slots.h"

// \cond DO_NOT_DOCUMENT
#define SLOT_TABLE const Airtight_SlotTable slot_table[AT_CONF_SLOT_TABLE_COLUMNS]
#define IDLE ACTION_IDLE
#define LISTEN ACTION_LISTEN
#define TRANSMIT ACTION_TRANSMIT
// \endcond

#include "airtight_slot_table.txt"

at_bool_t Airtight_SlotShouldReceive(at_u8_t slot)
{
    return slot_table[AT_CONF_NODE_ID].slots[slot] == LISTEN;
}

Airtight_SlotAction Airtight_GetSlotAction(at_u8_t slot)
{
    return slot_table[AT_CONF_NODE_ID].slots[slot];
}
