/**
 * @addtogroup AirTight_Logging
 * @{
 * @file
 * AirTight: data logging functionality header.
 */
#ifndef __AIRTIGHT_LOGGING_H
#define __AIRTIGHT_LOGGING_H

#include <stdio.h>

#include "airtight_mac_config.h"
#include "airtight_packet.h"

#define AIRTIGHT_LOGGING

#define AIRTIGHT_LOGGING_PREFIX "LOG "

#ifdef AIRTIGHT_LOGGING

/**
 * Log data for analysis about the behaviour of the protocol.
 */
#define AT_LOG(time, event, slot, packet)                                                   \
    do                                                                                      \
    {                                                                                       \
        printf(AIRTIGHT_LOGGING_PREFIX "%u %s %u %u ", time, event, AT_CONF_NODE_ID, slot); \
        for (size_t _i = 0; _i < AIRTIGHT_PACKET_SIZE; _i++)                                \
        {                                                                                   \
            printf("%02x", (packet).data.raw[_i]);                                            \
        }                                                                                   \
        puts("");                                                                           \
    } while (0)

/**
 * Simplified AT_LOG for use where an Airtight_MACState is available.
 */
#define AT_LOG_MAC(mac_state, event, packet) AT_LOG(Airtight_Time_GetSynchronisedTime(&mac_state->time), event, mac_state->current_slot, packet)

#else

/**
 * Log data for analysis about the behaviour of the protocol.
 * @note Disabled.
 */
#define AT_LOG(time, event, slot, packet)

/**
 * Simplified AT_LOG for use where an Airtight_MACState is available.
 * @note Disabled.
 */
#define AT_LOG_MAC(mac_state, event, packet)

#endif

#endif
