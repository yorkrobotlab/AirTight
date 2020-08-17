/**
 * @addtogroup Airtight_Packet
 * @{
 * @file
 * AirTight: packet definition header.
 */
#ifndef __AIRTIGHT_PACKET_QUEUE_H
#define __AIRTIGHT_PACKET_QUEUE_H

#include <string.h>

#include "airtight_types.h"
#include "airtight_mac_config.h"

/**
 * The size of the data segment of packets in bytes.
 */
#define AIRTIGHT_DATA 16

/**
 * The size of the non-transmitted packet metadata in bytes.
 */
#define AIRTIGHT_PACKET_META 9

/**
 * The size of a notification packet.
 */
#define AIRTIGHT_NOTIFICATION_PACKET 8

/**
 * The size of full Airtight packet.
 */
#define AIRTIGHT_PACKET_SIZE (AIRTIGHT_PACKET_META + AIRTIGHT_DATA)

// \cond DO_NOT_DOCUMENT
#ifndef __LINT
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif
// \endcond

/**
 * Transmissible data fields of an Airtight_Packet.
 */
typedef struct PACKED
{
    Airtight_Priority priority : 8;
    Airtight_Criticality criticality : 8;
    at_u8_t flow_id : 8;
    Airtight_NodeId source : 8;
    Airtight_NodeId destination : 8;
    Airtight_NodeId hop_source : 8;
    Airtight_NodeId hop_destination : 8;
    at_u8_t c_value : 8;
    at_u8_t sequence_number : 8;
    at_u8_t data[AIRTIGHT_DATA];
} Airtight_PacketDataInner;

/**
 * Non-transmissble metadata relating to an Airtight_Packet.
 */
typedef struct
{
    at_u8_t local_retransmit_count;
    at_u16_t enqueue_slot;
    at_u16_t hop_send_slot;
    at_u8_t burst_number;
    at_time_t inject_time;
    at_time_t send_time;
    at_u8_t failed_ack_status;
} Airtight_PacketMeta;

/**
 * Packet data as either fields via Airtight_PacketDataInner or raw bytes.
 */
typedef union {
    Airtight_PacketDataInner fields;
    at_u8_t raw[AIRTIGHT_PACKET_SIZE];
} Airtight_PacketData;

/**
 * An Airtight packet with data and metadata.
 */
typedef struct
{
    Airtight_PacketData data;
    Airtight_PacketMeta meta;
} Airtight_Packet;

/**
 * Notification packet fault types.
 */
typedef enum
{
    FAULT_SYNC = 0x00
} Airtight_Fault;

/**
 * Notification packet inner data fields.
 */
typedef struct PACKED
{
    Airtight_NodeId root_id : 8;
    Airtight_Fault fault_activity : 8;
    at_u16_t sync_slot : 16;
    at_time_t sync_time : 32;
} Airtight_NotificationInner;

/**
 * Notification packet data as either fields via Airtight_NotificationInner or
 * raw bytes.
 */
typedef union {
    Airtight_NotificationInner fields;
    at_u8_t raw[AIRTIGHT_NOTIFICATION_PACKET];
} Airtight_Notification;

void Airtight_InitialisePacket(Airtight_Packet *packet);

#endif
