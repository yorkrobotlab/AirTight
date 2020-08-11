/**
 * @addtogroup Airtight_Slotter
 * @{
 * @file
 * AirTight: core loop functions firing scheduled slot actions header.
 */
#ifndef __AIRTIGHT_SLOTTER_H
#define __AIRTIGHT_SLOTTER_H

#include "airtight_types.h"
#include "airtight_mac.h"
#include "airtight_time.h"

void Airtight_Slotter_SingleThreadedSlotterTick(Airtight_MACState *mac_state,
                                                at_u8_t *slot,
                                                at_time_t *counter);

#endif
