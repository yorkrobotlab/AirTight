#include "airtight_mac.h"

void Airtight_InitialiseMACState(Airtight_MACState *mac_state)
{
    AT_ENTER(Airtight_InitialiseMACState);
    mac_state->acknowledge_fails = 0;
    mac_state->criticality_mode = LOW_CRIT;
    mac_state->current_slot = 0xff;
    mac_state->previous_slot = 0xfe;
    mac_state->local_slot = 0;
    mac_state->fault_active = false;
    mac_state->receive_callback = NULL;
    mac_state->transmit_handler = NULL;
    mac_state->notification_handler = NULL;

    Airtight_PCQ_Init(&mac_state->queue);
    Airtight_History_Init(&mac_state->send_history);
    Airtight_History_Init(&mac_state->receive_history);
    Airtight_Time_Init(&mac_state->time);
    Airtight_Time_InitAlarm(&mac_state->fault_alarm);
}

void Airtight_SetReceiveCallback(Airtight_MACState *mac_state, Airtight_ReceiveCallback callback)
{
    AT_ENTER(Airtight_SetReceiveCallback);
    mac_state->receive_callback = callback;
}

void Airtight_SetTransmitHandler(Airtight_MACState *mac_state, Airtight_TransmitHandler handler)
{
    AT_ENTER(Airtight_SetTransmitHandler);
    mac_state->transmit_handler = handler;
}

void Airtight_SetNotificationHandler(Airtight_MACState *mac_state, Airtight_NotificationHandler handler)
{
    AT_ENTER(Airtight_SetNotificationHandler);
    mac_state->notification_handler = handler;
}

at_bool_t Airtight_CheckShouldGoHigh(Airtight_MACState *mac_state)
{
    AT_ENTER(Airtight_CheckShouldGoHigh);
    return mac_state->criticality_mode != HIGH_CRIT && mac_state->acknowledge_fails >= AT_CONF_CRITICALITY_CHANGE_THRESHOLD;
}

void Airtight_GoHigh(Airtight_MACState *mac_state)
{
    AT_ENTER(Airtight_GoHigh);
    mac_state->criticality_mode = HIGH_CRIT;

#if (AT_CONF_CLEAR_LOW_PACKETS_ON_HIGH == 1)
    Airtight_PCQ_ClearCriticality(&mac_state->queue, LOW_CRIT);
#endif
}

at_bool_t Airtight_CheckShouldDequeuePacket(Airtight_Packet *packet)
{
    AT_ENTER(Airtight_CheckShouldDequeuePacket);
    if (packet->data.fields.criticality == HIGH_CRIT)
    {
        return packet->meta.local_retransmit_count >= AT_CONF_RETRANSMISSION_LIMIT_HIGH;
    }
    else
    {
        return packet->meta.local_retransmit_count >= AT_CONF_RETRANSMISSION_LIMIT_LOW;
    }
}

void Airtight_TriggerFault(Airtight_MACState *mac_state)
{
    AT_ENTER(Airtight_TriggerFault);
    if (!mac_state->fault_active)
    {
        mac_state->fault_active = true;
        // Start fault clear timer
        Airtight_Time_SetAlarm(&mac_state->fault_alarm, AT_CONF_SLOT_LENGTH_MS * AT_CONF_FAULT_LENGTH_SLOTS);
    }
}

void Airtight_CheckForFaultTrigger(Airtight_MACState *mac_state, at_u8_t sync_slot)
{
    AT_ENTER(Airtight_CheckForFaultTrigger);
    at_i16_t offset = (sync_slot % AT_CONF_FAULT_PERIOD_INTERVAL_SLOTS) - AT_CONF_SLOT_FAULT_OFFSET;
    if (offset > 0 && offset < AT_CONF_SLOT_TABLE_ROWS)
    {
        Airtight_TriggerFault(mac_state);
    }
}

void Airtight_RegisterFailedAck(Airtight_MACState *mac_state)
{
    AT_ENTER(Airtight_RegisterFailedAck);
    mac_state->acknowledge_fails++;
    if (mac_state->acknowledge_fails > AT_CONF_MAX_NODE_ACK_FAILS)
    {
        mac_state->acknowledge_fails = AT_CONF_MAX_NODE_ACK_FAILS;
    }
}

void Airtight_ClearAckFails(Airtight_MACState *mac_state)
{
    AT_ENTER(Airtight_ClearAckFails);
    mac_state->acknowledge_fails = 0;
}

void Airtight_GoLow(Airtight_MACState *mac_state)
{
    AT_ENTER(Airtight_GoLow);
    mac_state->criticality_mode = LOW_CRIT;
    Airtight_ClearAckFails(mac_state);
}

void Airtight_RecordSuccessfullySentPacket(Airtight_MACState *mac_state, Airtight_Packet *packet)
{
    AT_ENTER(Airtight_RecordSuccessfullySentPacket);
    Airtight_Record record;
    record.send.flow_id = packet->data.fields.flow_id;
    record.send.inject_time = packet->meta.inject_time;
    record.send.sequence_number = packet->data.fields.sequence_number;
    record.send.send_time = packet->meta.send_time;

    Airtight_History_Push(&mac_state->send_history, &record);
}

void Airtight_RecordSuccessfullyReceivedPacket(Airtight_MACState *mac_state, Airtight_Packet *packet)
{
    AT_ENTER(Airtight_RecordSuccessfullyReceivedPacket);
    Airtight_Record record;
    record.receive.flow_id = packet->data.fields.flow_id;
    record.receive.sequence_number = packet->data.fields.sequence_number;

    Airtight_History_Push(&mac_state->receive_history, &record);
}

void Airtight_ClearFault(Airtight_MACState *mac_state)
{
    AT_ENTER(Airtight_ClearFault);
    mac_state->fault_active = false;
}

void Airtight_HandleTransmitSlot(Airtight_MACState *mac_state)
{
    AT_ENTER(Airtight_HandleTransmitSlot);
    Airtight_Packet forward_packet;
    at_bool_t has_packet = false;

    AT_DEBUG("Airtight_HandleTransmitSlot: finding packet");
    if (mac_state->criticality_mode == HIGH_CRIT)
    {
        AT_DEBUG("Airtight_HandleTransmitSlot: finding high-crit packet");
        has_packet = Airtight_PCQ_HeadCriticality(&mac_state->queue, HIGH_CRIT, &forward_packet);
        if (!has_packet)
        {
            AT_DEBUG("Airtight_HandleTransmitSlot: no packet at criticality, going low");
            Airtight_GoLow(mac_state);
        }
    }

    if (mac_state->criticality_mode != HIGH_CRIT || !has_packet)
    {
        AT_DEBUG("Airtight_HandleTransmitSlot: finding any-crit packet");
        has_packet = Airtight_PCQ_Head(&mac_state->queue, &forward_packet);
    }

    if (!has_packet && Airtight_PCQ_Size(&mac_state->queue) > 0)
    {
        AT_DEBUG("Airtight_HandleTransmitSlot: warning, no packet found but size > 0");
        // Error has occurred
    }

    if (!has_packet)
    {
        AT_DEBUG("Airtight_HandleTransmitSlot: no packet found");
        return;
    }

    forward_packet.data.fields.hop_source = AT_CONF_NODE_ID;
    forward_packet.data.fields.hop_destination = Airtight_NextHop(forward_packet.data.fields.destination);

    forward_packet.meta.send_time = Airtight_Time_GetSynchronisedTime(&mac_state->time);
    forward_packet.meta.failed_ack_status = mac_state->acknowledge_fails;
    forward_packet.meta.hop_send_slot = mac_state->local_slot;

    if (NULL != mac_state->transmit_handler)
    {
        AT_DEBUG("Airtight_HandleTransmitSlot: calling transmit handler");
        AT_LOG_MAC(mac_state, "TRANSMIT", forward_packet);
        mac_state->transmit_handler(&forward_packet);
    }
    else
    {
        AT_DEBUG("Airtight_HandleTransmitSlot: warning, no transmit handler");
    }
}

void Airtight_HandleBroadcastSync(Airtight_MACState *mac_state)
{
    Airtight_Notification notification =
        {{.fault_activity = FAULT_SYNC,
          .root_id = AT_CONF_NODE_ID,
          .sync_slot = mac_state->local_slot,
          .sync_time = Airtight_Time_GetSynchronisedTime(&mac_state->time)}};

    if (NULL != mac_state->transmit_handler)
    {
        AT_DEBUG("Airtight_HandleTransmitSlot: calling transmit handler");

        mac_state->notification_handler(&notification);
    }
    else
    {
        AT_DEBUG("Airtight_HandleTransmitSlot: warning, no transmit handler");
    }
}

void Airtight_DoSlot(Airtight_MACState *mac_state, at_u8_t slot)
{
    AT_ENTER("Airtight_DoSlot");
    Airtight_SlotAction scheduled_action;

    at_u8_t previous_slot = mac_state->current_slot;

    mac_state->current_slot = slot;

    if (mac_state->previous_slot != slot)
    {
        mac_state->local_slot += 1;
    }

    mac_state->previous_slot = previous_slot;

    scheduled_action = Airtight_GetSlotAction(slot);

    AT_DEBUGF("Slot index: %u, %x\n", slot, scheduled_action);

    if (slot == AT_CONF_SYNC_SLOT_INDEX && AT_CONF_NODE_ID == AT_CONF_SYNC_NODE_ID)
    {
        Airtight_HandleBroadcastSync(mac_state);
    }

    if (slot != AT_CONF_SYNC_SLOT_INDEX && scheduled_action == ACTION_TRANSMIT)
    {
        Airtight_HandleTransmitSlot(mac_state);
    }
}

void Airtight_Enqueue(Airtight_MACState *mac_state, Airtight_Packet *packet)
{
    AT_ENTER(Airtight_Enqueue);
    const Airtight_Priority priority = packet->data.fields.priority;

    if (priority >= AIRTIGHT_PRIORITY_MIN && priority <= AIRTIGHT_PRIORITY_MAX)
    {
        AT_DEBUG("Airtight_Enqueue: enqueuing packet.");
        AT_LOG_MAC(mac_state, "ENQUEUE", *packet);
#if (AT_CONF_COPY_IN_ENQUEUE == 1)
        // We make a copy so we don't alter the original
        Airtight_Packet data_packet;
        memcpy(&data_packet, packet, sizeof(Airtight_Packet));

        data_packet.meta.local_retransmit_count = 0;
        data_packet.meta.enqueue_slot = mac_state->local_slot;
        data_packet.meta.inject_time = Airtight_Time_GetSynchronisedTime(&mac_state->time);

        Airtight_PCQ_Enqueue(&mac_state->queue, &data_packet);
#else
        packet->meta.local_retransmit_count = 0;
        packet->meta.enqueue_slot = mac_state->local_slot;
        packet->meta.inject_time = Airtight_Time_GetSynchronisedTime(&mac_state->time);

        Airtight_PCQ_Enqueue(&mac_state->queue, packet);
#endif
    }
    else
    {
        AT_DEBUG("Airtight_Enqueue: wrong priority packet.");
    }
}

void Airtight_Send(Airtight_MACState *mac_state, Airtight_Packet *packet)
{
    AT_ENTER(Airtight_Send);

    const Airtight_Priority priority = packet->data.fields.priority;
    const Airtight_Priority criticality = packet->data.fields.criticality;

    if (!(priority >= AIRTIGHT_PRIORITY_MIN && priority <= AIRTIGHT_PRIORITY_MAX))
    {
        AT_DEBUG("Airtight_Send: packet has invalid priority, discarding.");
        return;
    }
    if (criticality != mac_state->queue.criticalities[priority])
    {
        AT_DEBUG("Airtight_Send: packet has invalid criticality for priority, discarding.");
        return;
    }

    at_u8_t c_count_limit = packet->data.fields.c_value;
    if (c_count_limit > AT_CONF_MAX_C_VALUE)
    {
        c_count_limit = AT_CONF_MAX_C_VALUE;
    }
    // If c_count_limit was signed we would perform a sign check, it's not however

    AT_DEBUG("Airtight_Send: packet set to send.");

#if (AT_CONF_DISCARD_LOW_WHILE_HIGH == 1)
    if (mac_state->criticality_mode == HIGH_CRIT && criticality == LOW_CRIT)
    {
        AT_DEBUG("Airtight_Send: Discarded LOW packet while HIGH.");
        // TODO: Count discards
        return;
    }
#endif

    AT_LOG_MAC(mac_state, "SEND", *packet);

    at_u8_t base_sequence_number = packet->data.fields.sequence_number;

    for (at_u8_t c_value = 0; c_value < c_count_limit; c_value++)
    {
        packet->data.fields.sequence_number = base_sequence_number + c_value;
        packet->meta.burst_number = c_value;
        AT_DEBUGF("Airtight_Send: enqueue c_value = %u", c_value);

        packet->meta.inject_time = Airtight_Time_GetSynchronisedTime(&mac_state->time);

        Airtight_Enqueue(mac_state, packet);
    }
}

void Airtight_RegisterSendComplete(Airtight_MACState *mac_state, Airtight_Packet *packet, at_bool_t was_acked)
{
    AT_ENTER(Airtight_RegisterSendComplete);

#ifdef AIRTIGHT_DEBUG
    Airtight_PrintPacket(packet);
#endif

    AT_DEBUG("Airtight_RegisterSendComplete: Handling send complete of above packet.");

    const Airtight_Priority priority = packet->data.fields.priority;
    const Airtight_Priority criticality = packet->data.fields.criticality;

    if (was_acked && !mac_state->fault_active)
    {
        AT_DEBUG("Airtight_RegisterSendComplete: acked successfully with no active fault.");

        AT_LOG_MAC(mac_state, "ACK_SUCCESS", *packet);
        Airtight_RecordSuccessfullySentPacket(mac_state, packet);

        if (priority >= AIRTIGHT_PRIORITY_MIN && priority <= AIRTIGHT_PRIORITY_MAX)
        {
            AT_DEBUG("Airtight_RegisterSendComplete: Dequeued packet");
            AT_LOG_MAC(mac_state, "DEQUEUE", *packet);
            Airtight_PCQ_DequeuePriorityCriticality(&mac_state->queue, priority, criticality, NULL);
        }
        else
        {
            AT_DEBUG("Airtight_RegisterSendComplete: incorrect priority.");
        }
    }
    else
    {
        AT_DEBUG("Airtight_RegisterSendComplete: ack failed.");

        if (mac_state->fault_active)
        {
            AT_DEBUG("Airtight_RegisterSendComplete: ack failure due to fault activity.");
        }

        AT_DEBUG("Airtight_RegisterSendComplete: Registering failed ack.");
        AT_LOG_MAC(mac_state, "ACK_FAIL", *packet);
        Airtight_RegisterFailedAck(mac_state);

        AT_DEBUG("Airtight_RegisterSendComplete: checking to go high...");
        if (Airtight_CheckShouldGoHigh(mac_state))
        {
            AT_DEBUG("Airtight_RegisterSendComplete: going high");
            Airtight_GoHigh(mac_state);
        }

#if (AT_CONF_CLEAR_HIGH_PACKETS_ON_BEST_EFFORT == 1)
        // TODO
#endif

        AT_DEBUG("Airtight_RegisterSendComplete: checking to dequeue packet...");
        if (Airtight_CheckShouldDequeuePacket(packet))
        {
            AT_DEBUG("Airtight_RegisterSendComplete: dequeueing packet");
            if (priority >= AIRTIGHT_PRIORITY_MIN && priority <= AIRTIGHT_PRIORITY_MAX)
            {
                AT_LOG_MAC(mac_state, "DEQUEUE", *packet);
                Airtight_PCQ_DequeuePriorityCriticality(&mac_state->queue, priority, criticality, NULL);
            }
            else
            {
                AT_DEBUG("Airtight_RegisterSendComplete: incorrect priority for retransmit limit.");
            }
        }
        else
        {
            AT_DEBUG("Airtight_RegisterSendComplete: not dequeueing packet");
            if (priority >= AIRTIGHT_PRIORITY_MIN && priority <= AIRTIGHT_PRIORITY_MAX)
            {
                Airtight_Packet *head_packet = Airtight_PCQ_HeadPriorityCriticalityP(&mac_state->queue, priority, criticality);

                if (NULL != head_packet)
                    head_packet->meta.local_retransmit_count++;
            }
            else
            {
                AT_DEBUGF("Airtight_RegisterSendComplete: incorrect priority for retransmit increment (%u).", priority);
            }
        }
    }
}

void Airtight_HandleReceive(Airtight_MACState *mac_state, Airtight_Packet *packet)
{
    AT_ENTER(Airtight_HandleReceive);

    const Airtight_NodeId received_destination = packet->data.fields.destination;

    AT_LOG_MAC(mac_state, "RECEIVE", *packet);

    AT_DEBUGF("Airtight_HandleReceive: received packet destination %x.\n", received_destination);

    if (!Airtight_SlotShouldReceive(mac_state->current_slot))
    {
        AT_DEBUG("Airtight_HandleReceive: packet should not have been received in this slot.");
    }

    if (received_destination != AT_CONF_NODE_ID)
    {
        AT_DEBUG("Airtight_HandleReceive: enqueuing packet to forward.");

        if (mac_state->fault_active)
        {
            AT_DEBUG("Airtight_HandleReceive: fault active, dropping forward packet.");
        }
        else
        {
            packet->meta.inject_time = Airtight_Time_GetSynchronisedTime(&mac_state->time);
            Airtight_Enqueue(mac_state, packet);
        }
    }
    else
    {
        AT_DEBUG("Airtight_HandleReceive: passing packet to application layer.");

        Airtight_RecordSuccessfullyReceivedPacket(mac_state, packet);

        if (NULL != mac_state->receive_callback)
        {
            mac_state->receive_callback(packet);
        }
    }
}

/**
 * Synchronise protocol state based on sync slot and time.
 */
void Airtight_SynchroniseNow(Airtight_MACState *mac_state, at_u8_t sync_slot, at_time_t sync_time)
{
    AT_ENTER(Airtight_SynchroniseNow);

    at_i16_t slot_difference = sync_slot - mac_state->local_slot;
    mac_state->local_slot = sync_slot;
    mac_state->current_slot += slot_difference;
    mac_state->previous_slot += slot_difference;
    Airtight_Time_SetSynchronisationPoint(&mac_state->time, sync_time + AT_CONF_SYNC_TIME_OFFSET);

    // More complex synchronisation may be performed here in later revisions.
}

/**
 * Handle reception of notification.
 */
void Airtight_HandleNotificationReceive(Airtight_MACState *mac_state, Airtight_Notification *notification)
{
    AT_ENTER(Airtight_HandleNotificationReceive);

    if (notification->fields.fault_activity == FAULT_SYNC)
    {
        AT_DEBUG("Airtight_HandleNotificationReceive: received sync fault");

#ifdef AIRTIGHT_DEBUG
        if (notification->fields.root_id != AT_CONF_SYNC_NODE_ID)
        {
            AT_DEBUG("Airtight_HandleNotificationReceive: warning, sync not from sync node");
        }
#endif

#if (AT_CONF_NODE_ID == AT_CONF_SYNC_NODE_ID)
        // We do not synchronise if we are the sync node.
#else
        Airtight_SynchroniseNow(mac_state, notification->fields.sync_slot, notification->fields.sync_time);
#endif
    }
}
