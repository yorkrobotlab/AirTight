/**
 * @addtogroup Airtight_Radio
 * @{
 * @file
 * AirTight: radio specific header.
 */
#ifndef __AIRTIGHT_RADIO_H
#define __AIRTIGHT_RADIO_H

#include <stddef.h>

#include "airtight_utilities.h"
#include "airtight_types.h"
#include "airtight_mac_config.h"

#include "xbee/platform.h"
#include "xbee/device.h"
#include "xbee/atcmd.h"
#include "xbee/tx_status.h"
#include "xbee/user_data.h"
#include "xbee/serial.h"
#include "xbee/wpan.h"

typedef void (*Airtight_Radio_ReceiveHandler)(at_u8_t *data, at_u16_t datalen);
typedef void (*Airtight_Radio_TransmitStatusHandler)(at_u8_t packet_id, at_u8_t status);

typedef struct
{
    xbee_dev_t device;
    xbee_serial_t serial;
} Airtight_Radio;

at_bool_t Airtight_Radio_Init(Airtight_Radio *radio);
void Airtight_Radio_DeviceTick(Airtight_Radio *radio);
void Airtight_Radio_AttachReceiveHandler(Airtight_Radio_ReceiveHandler handler);
void Airtight_Radio_AttachTransmitStatusHandler(Airtight_Radio_TransmitStatusHandler handler);

#endif
