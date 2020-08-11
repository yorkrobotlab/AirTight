/**
 * @addtogroup AirTight_History
 * @{
 * @file
 * AirTight: Fixed length history header.
 */
#ifndef __AIRTIGHT_HISTORY_H
#define __AIRTIGHT_HISTORY_H

#include "airtight_types.h"
#include "airtight_packet.h"
#include "airtight_mac_config.h"
#include "string.h"

/**
 * Record of a reception event.
 */
typedef struct
{
    at_u8_t sequence_number;
    at_u8_t flow_id;
} Airtight_ReceiveRecord;

/**
 * Record of a transmission event.
 */
typedef struct
{
    at_u8_t sequence_number;
    at_u8_t flow_id;
    at_u32_t inject_time;
    at_u32_t send_time;
} Airtight_SendRecord;

/**
 * A generic record, can contain either an Airtight_SendRecord or an Airtight_ReceiveRecord.
 */
typedef union {
    Airtight_ReceiveRecord receive;
    Airtight_SendRecord send;
} Airtight_Record;

/**
 * A structure for storing a history of Airtight_Record structures.
 */
typedef struct
{
    Airtight_Record queue[AT_CONF_HISTORY_SIZE];
    at_u8_t head;
    at_u8_t size;
} Airtight_History;

void Airtight_History_Init(Airtight_History *history);
void Airtight_History_Push(Airtight_History *history, Airtight_Record *record);
at_bool_t Airtight_History_Pop(Airtight_History *history, Airtight_Record *record_out);
at_u8_t Airtight_History_Size(Airtight_History *history);
at_bool_t Airtight_History_Get(Airtight_History *history, at_u8_t index, Airtight_Record *record_out);


#endif
