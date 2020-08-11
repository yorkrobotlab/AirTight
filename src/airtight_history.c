/**
 * @addtogroup AirTight_History
 * @{
 * @file
 * AirTight: Fixed length history implementation.
 */
#include "airtight_history.h"

/**
 * Initialise an Airtight_History struct.
 */
void Airtight_History_Init(Airtight_History *history)
{
    history->head = 0;
    history->size = 0;
}

/**
 * Push an Airtight_Record into an Airtight_History struct.
 *
 * As history is fixed length, if the history is full, the oldest item will be
 * discarded while pushing a new item on.
 */
void Airtight_History_Push(Airtight_History *history, Airtight_Record *record)
{
    if (history->size < AT_CONF_HISTORY_SIZE)
    {
        history->size++;
    }
    else if (history->head == (AT_CONF_HISTORY_SIZE - 1))
    {
        history->head = 0;
    }
    else
    {
        history->head++;
    }

    memcpy(&history->queue[history->head], record, sizeof(Airtight_Record));
}

/**
 * Pop an Airtight_Record from an Airtight_History.
 *
 * @return true on success, false otherwise
 */
at_bool_t Airtight_History_Pop(Airtight_History *history, Airtight_Record *record_out)
{
    if (history->size > 0)
    {
        at_u8_t old_head = history->head;
        history->size--;

        if (history->head == 0)
        {
            history->head = AT_CONF_HISTORY_SIZE - 1;
        }
        else
        {
            history->head--;
        }

        memcpy(record_out, &history->queue[old_head], sizeof(Airtight_Record));

        return true;
    }

    return false;
}

/**
 * Get the size of an Airtight_History.
 *
 * @return the history size
 */
at_u8_t Airtight_History_Size(Airtight_History *history)
{
    return history->size;
}

/**
 * Get an item from the Airtight_History with a specific index.
 *
 * @return true on success, false otherwise
 */
at_bool_t Airtight_History_Get(Airtight_History *history, at_u8_t index, Airtight_Record *record_out)
{
    if (index >= history->size)
        return false;

    index = (index + history->head) % AT_CONF_HISTORY_SIZE;

    memcpy(record_out, &history->queue[index], sizeof(Airtight_Record));

    return true;
}
