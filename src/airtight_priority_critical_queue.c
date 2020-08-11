/**
 * @addtogroup Airtight_PriorityCriticalQueue
 * @{
 * @file
 * AirTight: priority critical queue implementation.
 */
#include "airtight_priority_critical_queue.h"

// \cond DO_NOT_DOCUMENT
#define MOD_SIZE(x) ((x) % PRIORITY_CRITICAL_QUEUE_SIZE)
#define INC_INDEX(x) (((x) + 1) % PRIORITY_CRITICAL_QUEUE_SIZE)
#define DEC_INDEX(x) ((x) == 0 ? (PRIORITY_CRITICAL_QUEUE_SIZE - 1) : ((x)-1))
#define INC_COUNT(x) ((x) == PRIORITY_CRITICAL_QUEUE_SIZE ? PRIORITY_CRITICAL_QUEUE_SIZE : ((x) + 1))
#define DEC_COUNT(x) ((x) == 0 ? 0 : ((x)-1))
// \endcond

static const Airtight_Criticality _CRITICALITIES[] = AT_CONF_CRITICALITIES;

/**
 * Initialise a Airtight_PriorityCriticalQueue struct.
 */
void Airtight_PCQ_Init(Airtight_PriorityCriticalQueue *pcq)
{
    // PORT: consider specifying that this loop should be unrolled.
    for (Airtight_Priority i = 0; i < PRIORITY_CRITICAL_QUEUE_PRIORITIES; i++)
    {
        pcq->heads[i] = 0;
        pcq->sizes[i] = 0;
        pcq->criticalities[i] = _CRITICALITIES[i];
    }
}

/**
 * Get the head of the queue.
 *
 * The found packet is copied into packet_out.
 *
 * @return true if an item is found, false otherwise.
 */
at_bool_t Airtight_PCQ_Head(Airtight_PriorityCriticalQueue *pcq, Airtight_Packet *packet_out)
{
    for (Airtight_Priority i = 0; i < PRIORITY_CRITICAL_QUEUE_PRIORITIES; i++)
    {
        if (pcq->sizes[i] > 0)
        {
            memcpy(packet_out, &pcq->queues[i][pcq->heads[i]], sizeof(Airtight_Packet));

            return true;
        }
    }

    return false;
}

/**
 * Get the head of the queue based on priority.
 *
 * The found packet is copied into packet_out.
 *
 * @return true if an item is found, false otherwise.
 */
at_bool_t Airtight_PCQ_HeadPriority(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Packet *packet_out)
{
    if (pcq->sizes[priority] > 0)
    {
        memcpy(packet_out, &pcq->queues[priority][pcq->heads[priority]], sizeof(Airtight_Packet));
    }

    return false;
}

/**
 * Get the head of the queue based on criticality.
 *
 * The found packet is copied into packet_out.
 *
 * @return true if an item is found, false otherwise.
 */
at_bool_t Airtight_PCQ_HeadCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Criticality crit, Airtight_Packet *packet_out)
{
    for (Airtight_Priority i = 0; i < PRIORITY_CRITICAL_QUEUE_PRIORITIES; i++)
    {
        if (pcq->sizes[i] > 0 && pcq->criticalities[i] == crit)
        {
            memcpy(packet_out, &pcq->queues[i][pcq->heads[i]], sizeof(Airtight_Packet));
        }
    }

    return false;
}

/**
 * Get the head of the queue based on priority and criticality.
 *
 * The found packet is copied into packet_out.
 *
 * @return true if an item is found, false otherwise.
 */
at_bool_t Airtight_PCQ_HeadPriorityCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Criticality crit, Airtight_Packet *packet_out)
{
    if (pcq->sizes[priority] > 0 && pcq->criticalities[priority] == crit)
    {
        memcpy(packet_out, &pcq->queues[priority][pcq->heads[priority]], sizeof(Airtight_Packet));
        return true;
    }

    return false;
}

/**
 * Get a pointer to the head of the queue.
 *
 * @return a pointer to the stored packet if found, NULL otherwise
 * @note This allows modification of queue items in place.
 */
Airtight_Packet *Airtight_PCQ_HeadP(Airtight_PriorityCriticalQueue *pcq)
{
    for (Airtight_Priority i = 0; i < PRIORITY_CRITICAL_QUEUE_PRIORITIES; i++)
    {
        if (pcq->sizes[i] > 0)
        {
            return &pcq->queues[i][pcq->heads[i]];
        }
    }

    return NULL;
}

/**
 * Get a pointer to the head of the queue based on priority.
 *
 * @return a pointer to the stored packet if found, NULL otherwise
 * @note This allows modification of queue items in place.
 */
Airtight_Packet *Airtight_PCQ_HeadPriorityP(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority)
{
    if (pcq->sizes[priority] > 0)
    {
        return &pcq->queues[priority][pcq->heads[priority]];
    }

    return NULL;
}

/**
 * Get a pointer to the head of the queue based on criticality.
 *
 * @return a pointer to the stored packet if found, NULL otherwise
 * @note This allows modification of queue items in place.
 */
Airtight_Packet *Airtight_PCQ_HeadCriticalityP(Airtight_PriorityCriticalQueue *pcq, Airtight_Criticality crit)
{
    for (Airtight_Priority i = 0; i < PRIORITY_CRITICAL_QUEUE_PRIORITIES; i++)
    {
        if (pcq->sizes[i] > 0 && pcq->criticalities[i] == crit)
        {
            return &pcq->queues[i][pcq->heads[i]];
        }
    }

    return NULL;
}

/**
 * Get a pointer to the head of the queue based on priority and criticality.
 *
 * @return a pointer to the stored packet if found, NULL otherwise
 * @note This allows modification of queue items in place.
 */
Airtight_Packet *Airtight_PCQ_HeadPriorityCriticalityP(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Criticality crit)
{
    if (pcq->sizes[priority] > 0 && pcq->criticalities[priority] == crit)
    {
        return &pcq->queues[priority][pcq->heads[priority]];
    }

    return NULL;
}

/**
 * Enqueue a packet. The packet's priority/criticality will be inspected to enqueue it.
 *
 * @note The packet will be copied into the PCQ.
 */
void Airtight_PCQ_Enqueue(Airtight_PriorityCriticalQueue *pcq, Airtight_Packet *packet)
{
    const Airtight_Priority priority = packet->data.fields.priority;
    Airtight_QueueIndex insertion_index = MOD_SIZE(pcq->heads[priority] + pcq->sizes[priority]);

    if (pcq->sizes[priority] == PRIORITY_CRITICAL_QUEUE_SIZE)
    {
#if (REJECT_WHEN_BUFFER_FULL == 1)
        return;
#else
        pcq->heads[priority] = INC_INDEX(pcq->heads[priority]);
#endif
    }

    memcpy(&pcq->queues[priority][insertion_index], packet, sizeof(Airtight_Packet));
    pcq->sizes[priority] = INC_COUNT(pcq->sizes[priority]);
}

/**
 * Dequeue any item.
 *
 * @return true if an item to dequeue is found, false otherwise
 */
at_bool_t Airtight_PCQ_Dequeue(Airtight_PriorityCriticalQueue *pcq, Airtight_Packet *packet_out)
{
    for (Airtight_Priority i = 0; i < PRIORITY_CRITICAL_QUEUE_PRIORITIES; i++)
    {
        if (pcq->sizes[i] > 0)
        {
            if (NULL != packet_out)
                memcpy(packet_out, &pcq->queues[i][pcq->heads[i]], sizeof(Airtight_Packet));
            pcq->sizes[i] = DEC_COUNT(pcq->sizes[i]);
            pcq->heads[i] = INC_INDEX(pcq->heads[i]);

            return true;
        }
    }

    return false;
}

/**
 * Dequeue an item based on priority.
 *
 * @return true if an item to dequeue is found, false otherwise
 */
at_bool_t Airtight_PCQ_DequeuePriority(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Packet *packet_out)
{
    if (pcq->sizes[priority] > 0)
    {
        if (NULL != packet_out)
            memcpy(packet_out, &pcq->queues[priority][pcq->heads[priority]], sizeof(Airtight_Packet));
        pcq->sizes[priority] = DEC_COUNT(pcq->sizes[priority]);
        pcq->heads[priority] = INC_INDEX(pcq->heads[priority]);

        return true;
    }

    return false;
}

/**
 * Dequeue an item based on criticality.
 *
 * @return true if an item to dequeue is found, false otherwise
 */
at_bool_t Airtight_PCQ_DequeueCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Criticality crit, Airtight_Packet *packet_out)
{
    for (Airtight_Priority i = 0; i < PRIORITY_CRITICAL_QUEUE_PRIORITIES; i++)
    {
        if (pcq->sizes[i] > 0 && pcq->criticalities[i] == crit)
        {
            if (NULL != packet_out)
                memcpy(packet_out, &pcq->queues[i][pcq->heads[i]], sizeof(Airtight_Packet));
            pcq->sizes[i] = DEC_COUNT(pcq->sizes[i]);
            pcq->heads[i] = INC_INDEX(pcq->heads[i]);
            return true;
        }
    }

    return false;
}

/**
 * Dequeue an item based on priority and criticality.
 *
 * @return true if an item to dequeue is found, false otherwise
 */
at_bool_t Airtight_PCQ_DequeuePriorityCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Criticality crit, Airtight_Packet *packet_out)
{
    if (pcq->sizes[priority] > 0 && pcq->criticalities[priority] == crit)
    {
        if (NULL != packet_out)
            memcpy(packet_out, &pcq->queues[priority][pcq->heads[priority]], sizeof(Airtight_Packet));
        pcq->sizes[priority] = DEC_COUNT(pcq->sizes[priority]);
        pcq->heads[priority] = INC_INDEX(pcq->heads[priority]);

        return true;
    }

    return false;
}

/**
 * Get the number of items in the PCQ.
 *
 * @return the number of items
 */
size_t Airtight_PCQ_Size(Airtight_PriorityCriticalQueue *pcq)
{
    size_t count = 0;

    for (Airtight_Priority i = 0; i < PRIORITY_CRITICAL_QUEUE_PRIORITIES; i++)
    {
        count += pcq->sizes[i];
    }

    return count;
}

/**
 * Get the number of items which match the specified priority.
 *
 * @return the number of items
 */
size_t Airtight_PCQ_SizePriority(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority)
{
    return pcq->sizes[priority];
}

/**
 * Get the number of items which match the specified criticality.
 *
 * @return the number of items
 */
size_t Airtight_PCQ_SizeCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Criticality crit)
{
    size_t count = 0;

    for (Airtight_Priority i = 0; i < PRIORITY_CRITICAL_QUEUE_PRIORITIES; i++)
    {
        if (pcq->criticalities[i] == crit)
        {
            count += pcq->sizes[i];
        }
    }

    return count;
}

/**
 * Get the number of items which match the specified priority and criticality.
 *
 * @return the number of items
 */
size_t Airtight_PCQ_SizePriorityCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Criticality crit)
{
    size_t count = 0;

    if (pcq->criticalities[priority] == crit)
    {
        count += pcq->sizes[priority];
    }

    return count;
}

/**
 * Clear all items from a PCQ.
 */
void Airtight_PCQ_Clear(Airtight_PriorityCriticalQueue *pcq)
{
    for (Airtight_Priority i = 0; i < PRIORITY_CRITICAL_QUEUE_PRIORITIES; i++)
    {
        pcq->sizes[i] = 0;
    }
}

/**
 * Clear a PCQ based on priority.
 */
void Airtight_PCQ_ClearPriority(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority)
{
    pcq->sizes[priority] = 0;
}

/**
 * Clear a PCQ based on criticality.
 */
void Airtight_PCQ_ClearCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Criticality crit)
{
    for (Airtight_Priority i = 0; i < PRIORITY_CRITICAL_QUEUE_PRIORITIES; i++)
    {
        if (pcq->criticalities[i] == crit)
        {
            pcq->sizes[i] = 0;
        }
    }
}

/**
 * Clear a PCQ based on both priority and criticality.
 */
void Airtight_PCQ_ClearPriorityCriticality(Airtight_PriorityCriticalQueue *pcq, Airtight_Priority priority, Airtight_Criticality crit)
{
    if (pcq->criticalities[priority] == crit)
    {
        pcq->sizes[priority] = 0;
    }
}
