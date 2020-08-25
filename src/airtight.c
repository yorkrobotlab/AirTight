/**
 * @addtogroup Airtight_ExampleIntegration
 * @{
 * @file
 * AirTight: example integration of AirTight implementation.
 */
#include "airtight.h"

/**
 * Transmission record allows transmit statuses to be associated with packets.
 *
 * We could simply store the priority and criticality and look them up in the
 * PCQ, but that would mean we would get the stored copy and not the sent copy
 * which has changed hop specific values.
 */
typedef struct
{
    Airtight_Packet packet;
    at_bool_t notification;
    at_bool_t active;
} Integration_TransmitRecord;

/**
 * Overall MAC state for the application.
 */
Airtight_MACState mac_state;

/**
 * Application level receive handler.
 */
void App_HandleReceive(Airtight_Packet *packet)
{
    AT_DEBUG("Received packet at callback");
    Airtight_PrintPacket(packet);
}

/**
 * This table associates packets with Xbee IDs to allow status messages to be
 * associated with packets.
 */
Integration_TransmitRecord packet_id_table[256] = {{.active = false}};

/**
 * Transmission status handler.
 *
 * Takes transmit statuses from the radio and sends them to AirTight.
 */
void Integration_TransmitStatusHandler(at_u8_t packet_id, at_u8_t status)
{
    AT_ENTER(Integration_TransmitStatusHandler);
    if (status == 0x00 || status == 0x01)
    {
        if (packet_id_table[packet_id].active)
        {
            AT_DEBUG("Integration_TransmitStatusHandler: identified packet");
            if (packet_id_table[packet_id].notification)
            {
                AT_DEBUG("Integration_TransmitStatusHandler: notification status, ignoring");
            }
            else
            {
                Airtight_RegisterSendComplete(&mac_state, &packet_id_table[packet_id].packet, status == 0x00);
            }
            packet_id_table[packet_id].active = false;
        }
        else
        {
            AT_DEBUG("Integration_TransmitStatusHandler: status error.");
        }
    }
}

/**
 * Transmission handler.
 *
 * Takes packets from AirTight and transforms them for the radio.
 */
void Integration_TransmitHandler(Airtight_Packet *packet)
{
    AT_ENTER(Integration_TransmitHandler);
    xbee_header_transmit_t transmit_header;
    transmit_header.frame_type = XBEE_FRAME_TRANSMIT;
    transmit_header.frame_id = xbee_next_frame_id(&mac_state.radio->device);
    transmit_header.ieee_address = _WPAN_IEEE_ADDR_UNDEFINED;
    const at_u16_t address_endienness_swapped = AT_CONF_ADDRESS_HIGH_BYTE + (packet->data.fields.hop_destination << 8);
    transmit_header.network_address_be = address_endienness_swapped;
    transmit_header.broadcast_radius = 0;
    transmit_header.options = 0;

    memcpy(&packet_id_table[transmit_header.frame_id].packet, packet, sizeof(Airtight_Packet));
    packet_id_table[transmit_header.frame_id].notification = false;
    packet_id_table[transmit_header.frame_id].active = true;

    AT_DEBUG("Integration_TransmitHandler: Transmitting!");
    xbee_frame_write(&mac_state.radio->device, &transmit_header, sizeof(transmit_header), &packet->data.raw, AIRTIGHT_PACKET_META + AIRTIGHT_DATA, 0);
}

/**
 * Notification transmission handler.
 *
 * Takes notifications from AirTight and transforms them for the radio.
 */
void Integration_NotificationHandler(Airtight_Notification *notification)
{
    AT_ENTER(Integration_NotificationHandler);
    xbee_header_transmit_t transmit_header;
    transmit_header.frame_type = XBEE_FRAME_TRANSMIT;
    transmit_header.frame_id = xbee_next_frame_id(&mac_state.radio->device);
    transmit_header.ieee_address = _WPAN_IEEE_ADDR_UNDEFINED;
    transmit_header.network_address_be = WPAN_NET_ADDR_BCAST_ALL_NODES;
    transmit_header.broadcast_radius = 0;
    transmit_header.options = 0;

    packet_id_table[transmit_header.frame_id].notification = true;
    packet_id_table[transmit_header.frame_id].active = true;

    AT_DEBUG("Integration_NotificationHandler: Transmitting Notification!");
    xbee_frame_write(&mac_state.radio->device, &transmit_header, sizeof(transmit_header), &notification->raw, AIRTIGHT_NOTIFICATION_PACKET, 0);
}

/**
 * Reception handler.
 *
 * Takes packets from the radio and sends them to AirTight.
 */
void Integration_ReceiveHandler(at_u8_t *raw_packet, at_u16_t length)
{
    AT_ENTER(Integration_ReceiveHandler);

    if (length >= AIRTIGHT_PACKET_META)
    {
        AT_DEBUG("Integration_ReceiveHandler: Received packet");
        Airtight_Packet packet;

        memset(&packet.data.raw, 0, sizeof(packet.data.raw));
        memcpy(&packet.data.raw, raw_packet, length < sizeof(packet.data.raw) ? length : sizeof(packet.data.raw));

#ifdef AT_DEBUG
        Airtight_PrintPacket(&packet);
#endif

        Airtight_HandleReceive(&mac_state, &packet);
    }
    else if (length == AIRTIGHT_NOTIFICATION_PACKET)
    {
        AT_DEBUG("Integration_ReceiveHandler: Received notification");
        Airtight_Notification notification;

        memcpy(&notification.raw, raw_packet, length < sizeof(notification.raw) ? length : sizeof(notification.raw));

        Airtight_HandleNotificationReceive(&mac_state, &notification);
    }
}

/**
 * Basic traffic generation for example application.
 */
void App_Tick(at_u8_t slot)
{
    static at_u8_t slot_counter = 0;
    static at_u8_t last_slot = 0;
    at_bool_t packet_to_send = false;
    at_bool_t incremented = false;

    if (slot != last_slot)
    {
        last_slot = slot;
        slot_counter++;
        incremented = true;
    }

#if (AT_CONF_NODE_ID == 0)
    if (slot_counter % 16 == 0 && incremented)
    {
        packet_to_send = true;
    }
#elif (AT_CONF_NODE_ID == 1)
    if (slot_counter % 32 == 0 && incremented)
    {
        packet_to_send = true;
    }
#endif
    if (packet_to_send)
    {
        Airtight_Packet packet_to_send;
        Airtight_InitialisePacket(&packet_to_send);
        Application_Packet packet_content;

#if (AT_CONF_NODE_ID == 0)
        packet_content.fields.length = 3;
        packet_content.fields.data[0] = 0x55;
        packet_content.fields.data[1] = 0x88;
        packet_content.fields.data[2] = 0x11;

        packet_to_send.data.fields.c_value = 2;
        packet_to_send.data.fields.hop_destination = 0x02;
        packet_to_send.data.fields.priority = 0;
        packet_to_send.data.fields.criticality = LOW_CRIT;
        packet_to_send.data.fields.destination = 0x02;
#elif (AT_CONF_NODE_ID == 1)
        packet_content.fields.length = 2;
        packet_content.fields.data[0] = 0xab;
        packet_content.fields.data[1] = 0xcd;

        packet_to_send.data.fields.c_value = 3;
        packet_to_send.data.fields.hop_destination = 0x00;
        packet_to_send.data.fields.priority = 2;
        packet_to_send.data.fields.criticality = HIGH_CRIT;
        packet_to_send.data.fields.destination = 0x00;
#endif

        memcpy(packet_to_send.data.fields.data, packet_content.raw, sizeof(packet_content.raw));
        Airtight_Send(&mac_state, &packet_to_send);
    }
}

/**
 * Integration example entry point.
 */
int main(void)
{
    puts("Airtight Example Integration & Application.");

    // XBee radio configuration.
    // Radio should be configured for:
    //  - 802.15.4,
    //  - API mode without escapes,
    //  - and strict 802.15.4 with ACKs.
    Airtight_Radio radio = {
        .serial.device = "/dev/ttyS3",
        .serial.baudrate = 115200,
    };
    mac_state.radio = &radio;

    if (!Airtight_Radio_Init(&radio))
    {
        puts("Error: failed to initialise.");
        return 1;
    }

    Airtight_Radio_AttachTransmitStatusHandler(Integration_TransmitStatusHandler);
    Airtight_Radio_AttachReceiveHandler(Integration_ReceiveHandler);

    xbee_dev_dump_settings(&radio.device, 0);

    puts("Press enter to start.");
    getchar();

    Airtight_InitialiseMACState(&mac_state);
    at_u8_t slot = 0xff;
    at_time_t counter = 0;

    Airtight_SetReceiveCallback(&mac_state, App_HandleReceive);
    Airtight_SetTransmitHandler(&mac_state, Integration_TransmitHandler);
    Airtight_SetNotificationHandler(&mac_state, Integration_NotificationHandler);

    // Core event loop
    while (1)
    {
        App_Tick(slot);
        Airtight_Radio_DeviceTick(&radio);
        Airtight_Slotter_SingleThreadedSlotterTick(&mac_state, &slot, &counter);
        Airtight_Radio_DeviceTick(&radio);
    }

    return 0;
}
