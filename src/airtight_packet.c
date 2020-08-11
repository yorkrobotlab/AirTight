/**
 * @addtogroup Airtight_Packet
 * @{
 * @file
 * AirTight: packet definition implementation.
 */
#include "airtight_packet.h"

/**
 * Initialise a Airtight_InitialisePacket.
 *
 * All values set to zero including priority, except criticality which is set
 * to LOW.
 */
void Airtight_InitialisePacket(Airtight_Packet *packet)
{
    packet->data.fields.criticality = LOW_CRIT;
    packet->data.fields.priority = 0;
    packet->data.fields.destination = 0;
    packet->data.fields.flow_id = 0;
    packet->data.fields.hop_destination = 0;
    packet->data.fields.hop_source = AT_CONF_NODE_ID;
    packet->data.fields.sequence_number = 0;
    packet->data.fields.source = AT_CONF_NODE_ID;

    packet->meta.burst_number = 0;
    packet->meta.enqueue_slot = 0;
    packet->meta.failed_ack_status = 0;
    packet->meta.hop_send_slot = 0;
    packet->meta.inject_time = 0;
    packet->meta.local_retransmit_count = 0;
    packet->meta.send_time = 0;

#if (AT_CONF_ZERO_PACKET_DATA == 1)
    memset(packet->data.fields.data, 0x00, AIRTIGHT_DATA);
#endif
}
