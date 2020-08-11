/**
 * AirTight: Core protocol implementation.
 */
#ifndef __AIRTIGHT_MAC_H
#define __AIRTIGHT_MAC_H

#include "airtight_types.h"
#include "airtight_mac_config.h"
#include "airtight_routes.h"
#include "airtight_slots.h"
#include "airtight_critical_sections.h"
#include "airtight_history.h"
#include "airtight_utilities.h"
#include "airtight_radio.h"
#include "airtight_time.h"
#include "airtight_priority_critical_queue.h"
#include "airtight_logging.h"

typedef void (*Airtight_ReceiveCallback)(Airtight_Packet *packet);
typedef void (*Airtight_TransmitHandler)(Airtight_Packet *packet);

/**
 * Full MAC State store.
 */
typedef struct
{
    Airtight_Criticality criticality_mode;
    at_bool_t fault_active;

    // Slots used for the slot table
    at_u8_t current_slot;
    at_u8_t previous_slot;

    // Local slot count, keeps incrementing
    at_u16_t local_slot;

    at_u8_t acknowledge_fails;

    Airtight_PriorityCriticalQueue queue;

    Airtight_History send_history;
    Airtight_History receive_history;

    Airtight_Time time;
    Airtight_Alarm fault_alarm;

    Airtight_ReceiveCallback receive_callback;
    Airtight_TransmitHandler transmit_handler;
    Airtight_Radio *radio;
} Airtight_MACState;

void Airtight_InitialiseMACState(Airtight_MACState *mac_state);
void Airtight_SetReceiveCallback(Airtight_MACState *mac_state, Airtight_ReceiveCallback callback);
void Airtight_Send(Airtight_MACState *mac_state, Airtight_Packet *packet);
void Airtight_DoSlot(Airtight_MACState *mac_state, at_u8_t slot);
void Airtight_RegisterSendComplete(Airtight_MACState *mac_state, Airtight_Packet *packet, at_bool_t was_acked);
void Airtight_SetTransmitHandler(Airtight_MACState *mac_state, Airtight_TransmitHandler handler);
void Airtight_ClearFault(Airtight_MACState *mac_state);
void Airtight_HandleReceive(Airtight_MACState *mac_state, Airtight_Packet *packet);

#endif
