/**
 * @addtogroup Airtight_Config
 * @{
 * @file
 * AirTight: core MAC based configuration macros.
 */
#ifndef __AIRTIGHT_MAC_CONFIG_H
#define __AIRTIGHT_MAC_CONFIG_H

/**
 * Node ID of this node.
 */
#ifndef AT_CONF_NODE_ID
#define AT_CONF_NODE_ID 0
#endif

/**
 * The maximum number of ACK fails before entering HIGH criticality mode.
 */
#define AT_CONF_MAX_NODE_ACK_FAILS 5

/**
 * The number of transmissions for every packet.
 */
#define AT_CONF_MAX_C_VALUE 3

/**
 * Whether to remove all low criticality packets from the PCQ on entering HIGH
 * criticality mode.
 */
#define AT_CONF_CLEAR_LOW_PACKETS_ON_HIGH 1

/**
 * Whether to remove all high criticality packets from the PCQ on entering
 * best-effort mode.
 *
 * @note Best-effort mode is not currently implemented.
 */
#define AT_CONF_CLEAR_HIGH_PACKETS_ON_BEST_EFFORT 1

/**
 * Whether to discard newly enqueued LOW criticality packets while in HIGH
 * mode.
 */
#define AT_CONF_DISCARD_LOW_WHILE_HIGH 1

/**
 * Whether to remove all packets from the PCQ on entering best-effort mode.
 *
 * @note Best-effort mode is not currently implemented.
 */
#define AT_CONF_CLEAR_PACKETS_ON_BEST_EFFORT 1

/**
 * The number of times a LOW criticality packet can re-attempt transmission.
 */
#define AT_CONF_RETRANSMISSION_LIMIT_LOW 2

/**
 * The number of times a HIGH criticality packet can re-attempt transmission.
 */
#define AT_CONF_RETRANSMISSION_LIMIT_HIGH 4

/**
 * The interval period of a fault in slots.
 */
#define AT_CONF_FAULT_PERIOD_INTERVAL_SLOTS 100

/**
 * The offset of a fault in slots.
 */
#define AT_CONF_SLOT_FAULT_OFFSET 15

/**
 * The length of a fault in slots.
 */
#define AT_CONF_FAULT_LENGTH_SLOTS 15

/**
 * The length of a slot in milliseconds.
 */
#define AT_CONF_SLOT_LENGTH_MS 100

/**
 * The threshold for criticality change events.
 */
#define AT_CONF_CRITICALITY_CHANGE_THRESHOLD 2

/**
 * The ID of the node responsible for synchronisation.
 */
#define AT_CONF_SYNC_NODE_ID 1

/**
 * The slot of the slot table used for synchronisation broadcasts.
 */
#define AT_CONF_SYNC_SLOT_INDEX 0

/**
 * The length of transmit and receive histories.
 */
#define AT_CONF_HISTORY_SIZE 10

/**
 * Whether a copy should be performed during enqueuing.
 */
#define AT_CONF_COPY_IN_ENQUEUE 0

/**
 * The high-byte of the 16-bit IEEE 16-bit address for 802.15.4, the low byte
 * is taken from the node id.
 */
#define AT_CONF_ADDRESS_HIGH_BYTE 0x32

/**
 * Whether to zero the "data" segment of Airtight packets while initialising
 * them.
 *
 * Takes time but ensures no "bonus data" is introduced to confuse logs.
 */
#define AT_CONF_ZERO_PACKET_DATA 1

/**
 * The criticalities of each priority queue in the PCQ.
 */
#define AT_CONF_CRITICALITIES         \
    {                                 \
        LOW_CRIT, LOW_CRIT, HIGH_CRIT \
    }

#endif
