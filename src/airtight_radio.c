/**
 * @addtogroup Airtight_Radio
 * @{
 * @file
 * AirTight: radio specific implementation.
 */
#include "airtight_radio.h"

// Not listed in devices.h but is listed in official documentation
#define XBEE_FRAME_RECEIVE_16 0x81

typedef XBEE_PACKED(xbee_frame_receive_16_t, {
    uint8_t frame_type; ///< 16-bit Receive Packet - 0x81
    uint16_t ieee_address;
    uint8_t rssi;
    uint8_t options;
    uint8_t payload[1]; ///< multi-byte payload
}) xbee_frame_receive_16_t;

static Airtight_Radio_ReceiveHandler _receive_handler = NULL;
static Airtight_Radio_TransmitStatusHandler _transmit_status_handler = NULL;

int Airtight_Radio_InternalReceiveHandler(struct xbee_dev_t *xbee, const void FAR *raw, uint16_t length, void FAR *context)
{
    AT_ENTER(Airtight_Radio_InternalReceiveHandler);

    const xbee_frame_receive_16_t FAR *rx_frame = raw;

    XBEE_UNUSED_PARAMETER(xbee);
    XBEE_UNUSED_PARAMETER(context);

    if (NULL != _receive_handler && length >= offsetof(xbee_frame_receive_16_t, payload))
    {
        // We don't perform address-based filtering as the xbee does this.

        _receive_handler((at_u8_t *)rx_frame->payload, length - offsetof(xbee_frame_receive_16_t, payload));
    }

    return 0;
}

int Airtight_Radio_InternalTransmitStatusHandler(xbee_dev_t *xbee,
                                                 const void FAR *payload, uint16_t length, void FAR *context)
{
    AT_ENTER(Airtight_Radio_InternalTransmitStatusHandler);
    const xbee_frame_transmit_status_t FAR *frame = payload;

    XBEE_UNUSED_PARAMETER(xbee);
    XBEE_UNUSED_PARAMETER(context);
    XBEE_UNUSED_PARAMETER(length);

    if (NULL != _transmit_status_handler)
        _transmit_status_handler(frame->frame_id, frame->delivery);

    return 0;
}

const xbee_dispatch_table_entry_t xbee_frame_handlers[] =
    {
        XBEE_FRAME_HANDLE_LOCAL_AT,
        {XBEE_FRAME_TRANSMIT_STATUS, 0, Airtight_Radio_InternalTransmitStatusHandler, NULL},
        {XBEE_FRAME_RECEIVE_16, 0, Airtight_Radio_InternalReceiveHandler, NULL},
        XBEE_FRAME_TABLE_END};

static inline at_bool_t Airtight_Radio_CmdWait(xbee_dev_t *device)
{
    int status = 0;
    do
    {
        xbee_dev_tick(device);
        status = xbee_cmd_query_status(device);
    } while (status == -EBUSY);
    if (status)
    {
        printf("Error %d waiting for query to complete.\n", status);
        return false;
    }

    return true;
}

at_bool_t Airtight_Radio_Init(Airtight_Radio *radio)
{
    int error = 0;
    // initialize the serial and device layer for this XBee device
    if ((error = xbee_dev_init(&radio->device, &radio->serial, NULL, NULL)))
    {
        printf("Failed to initialize device. %d\n", error);
        return false;
    }

    // Initialize the AT Command layer for this XBee device and have the
    // driver query it for basic information (hardware version, firmware version,
    // serial number, IEEE address, etc.)
    xbee_cmd_init_device(&radio->device);
    printf("Waiting for driver to query the XBee device...\n");
    if (!Airtight_Radio_CmdWait(&radio->device))
        return false;

    at_u16_t cmd = xbee_cmd_create(&radio->device, "MY");
    xbee_cmd_set_param(cmd, (AT_CONF_ADDRESS_HIGH_BYTE << 8) + AT_CONF_NODE_ID);
    xbee_cmd_send(cmd);
    if (!Airtight_Radio_CmdWait(&radio->device))
        return false;

    // report on the settings
    xbee_dev_dump_settings(&radio->device, XBEE_DEV_DUMP_FLAG_DEFAULT);
    return true;
}

void Airtight_Radio_DeviceTick(Airtight_Radio *radio)
{
    xbee_dev_tick(&radio->device);
    xbee_cmd_tick();
}

void Airtight_Radio_AttachReceiveHandler(Airtight_Radio_ReceiveHandler handler)
{
    _receive_handler = handler;
}

void Airtight_Radio_AttachTransmitStatusHandler(Airtight_Radio_TransmitStatusHandler handler)
{
    _transmit_status_handler = handler;
}
