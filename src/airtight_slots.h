/**
 * @addtogroup Airtight_Slots
 * @{
 * @file
 * AirTight: slot table and slot table configuration header.
 */
#ifndef __AIRTIGHT_SLOTS_H
#define __AIRTIGHT_SLOTS_H

#include "airtight_types.h"
#include "airtight_mac_config.h"
#include "airtight_slots_config.h"
#include "airtight_utilities.h"

typedef enum
{
    ACTION_IDLE,
    ACTION_LISTEN,
    ACTION_TRANSMIT
} Airtight_SlotAction;

typedef struct
{
    Airtight_SlotAction slots[AT_CONF_SLOT_TABLE_ROWS];
} Airtight_SlotTable;

at_bool_t Airtight_SlotShouldReceive(at_u8_t slot);
Airtight_SlotAction Airtight_GetSlotAction(at_u8_t slot);


#endif
