/**
 * @addtogroup Airtight_Utilities
 * @{
 * @file
 * AirTight: utility functions and definitions header.
 */
#ifndef __AIRTIGHT_UTILITIES_H
#define __AIRTIGHT_UTILITIES_H

#include <stdio.h>

#include "airtight_packet.h"

#define AIRTIGHT_DEBUG

void Airtight_PrintPacket(Airtight_Packet *packet);

#ifdef AIRTIGHT_DEBUG
#define AT_ENTER(f) puts("-> " #f)
#define AT_DEBUG(s) puts(s)
#define AT_DEBUGF(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define AT_ENTER(f)
#define AT_DEBUG(s)
#define AT_DEBUGF(fmt, ...)
#endif

#endif
