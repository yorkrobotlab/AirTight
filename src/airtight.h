/**
 * @addtogroup Airtight_ExampleIntegration
 * @{
 * @file
 * AirTight: example integration of AirTight header.
 */
#ifndef __AIRTIGHT_MAIN_H
#define __AIRTIGHT_MAIN_H

#include "airtight_types.h"
#include "airtight_utilities.h"
#include "airtight_packet.h"
#include "airtight_mac.h"
#include "airtight_radio.h"
#include "airtight_slotter.h"
#include "airtight_time.h"

#include <stdio.h>

typedef struct
{
    at_u8_t length : 8;
    at_u8_t data[AIRTIGHT_DATA - 1];
} Application_PacketInner;

typedef union {
    Application_PacketInner fields;
    at_u8_t raw[AIRTIGHT_DATA];
} Application_Packet;

#endif
