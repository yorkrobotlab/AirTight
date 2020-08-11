/**
 * @addtogroup Airtight_PriorityCriticalQueue
 * @{
 * @file
 * AirTight: priority critical queue header.
 */
#ifndef __AIRTIGHT_PRIORITY_CRITICAL_QUEUE_H
#define __AIRTIGHT_PRIORITY_CRITICAL_QUEUE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "airtight_types.h"
#include "airtight_packet.h"
#include "airtight_mac_config.h"

#ifndef PRIORITY_CRITICAL_QUEUE_SIZE
/**
 * Defines the depth of each priority.
 */
#define PRIORITY_CRITICAL_QUEUE_SIZE 10
#endif

#if (PRIORITY_CRITICAL_QUEUE_SIZE < AT_CONF_MAX_C_VALUE)
#error C value cannot be greater than PCQ size as this would lead to saturation of the PCQ.
#endif

/**
 * Index type for queues.
 */
#if (PRIORITY_CRITICAL_QUEUE_SIZE < 256)
typedef at_u8_t Airtight_QueueIndex;
#elif (PRIORITY_CRITICAL_QUEUE_SIZE < 65536)
typedef at_u16_t Airtight_QueueIndex;
#endif

#ifndef PRIORITY_CRITICAL_QUEUE_PRIORITIES
/**
 * The number of different priorities and therefore queues.
 */
#define PRIORITY_CRITICAL_QUEUE_PRIORITIES AIRTIGHT_PRIORITIES
#endif

/**
 * Whether new packets should be rejected when a queue is full. If not then the
 * oldest entry in the buffer will be overwritten when a new packet is added to
 * a full queue.
 */
#define REJECT_WHEN_BUFFER_FULL 0

/**
 * Struct representing a priority critical queue.
 *
 * A priority critical queue has a number of queues, one for each priority
 * level. Each queue also has a criticality level. This allows some items to be
 * classed as having high-priority (typically handled with first) and some high
 * criticality (always should be handled, but not necessarily needing fast
 * handling). This allows low-criticality items to be ignored during fault
 * conditions to ensure the most critical tasks are still completed.
 */
typedef struct
{
    Airtight_Packet queues[PRIORITY_CRITICAL_QUEUE_PRIORITIES][PRIORITY_CRITICAL_QUEUE_SIZE];
    Airtight_QueueIndex heads[PRIORITY_CRITICAL_QUEUE_PRIORITIES];
    Airtight_QueueIndex sizes[PRIORITY_CRITICAL_QUEUE_PRIORITIES];
    Airtight_Criticality criticalities[PRIORITY_CRITICAL_QUEUE_PRIORITIES];
} Airtight_PriorityCriticalQueue;

void Airtight_PCQ_Init(Airtight_PriorityCriticalQueue *pcq);

at_bool_t Airtight_PCQ_Head(Airtight_PriorityCriticalQueue *pcq, Airtight_Packet *packet_out);
at_bool_t Airtight_PCQ_HeadPriority(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Packet *packet_out);
at_bool_t Airtight_PCQ_HeadCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Criticality crit, Airtight_Packet *packet_out);
at_bool_t Airtight_PCQ_HeadPriorityCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Criticality crit, Airtight_Packet *packet_out);

Airtight_Packet *Airtight_PCQ_HeadP(Airtight_PriorityCriticalQueue *pcq);
Airtight_Packet *Airtight_PCQ_HeadPriorityP(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority);
Airtight_Packet *Airtight_PCQ_HeadCriticalityP(Airtight_PriorityCriticalQueue *pcq, Airtight_Criticality crit);
Airtight_Packet *Airtight_PCQ_HeadPriorityCriticalityP(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Criticality crit);

void Airtight_PCQ_Enqueue(Airtight_PriorityCriticalQueue *pcq, Airtight_Packet *packet);

at_bool_t Airtight_PCQ_Dequeue(Airtight_PriorityCriticalQueue *pcq, Airtight_Packet *packet_out);
at_bool_t Airtight_PCQ_DequeuePriority(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Packet *packet_out);
at_bool_t Airtight_PCQ_DequeueCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Criticality crit, Airtight_Packet *packet_out);
at_bool_t Airtight_PCQ_DequeuePriorityCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Criticality crit, Airtight_Packet *packet_out);

size_t Airtight_PCQ_Size(Airtight_PriorityCriticalQueue *pcq);
size_t Airtight_PCQ_SizePriority(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority);
size_t Airtight_PCQ_SizeCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Criticality crit);
size_t Airtight_PCQ_SizePriorityCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Criticality crit);

void Airtight_PCQ_Clear(Airtight_PriorityCriticalQueue *pcq);
void Airtight_PCQ_ClearPriority(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority);
void Airtight_PCQ_ClearCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Criticality crit);
void Airtight_PCQ_ClearPriorityCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Criticality crit);

#endif
