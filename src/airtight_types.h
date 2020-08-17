/**
 * @addtogroup Airtight_Types
 * @{
 * @file
 * AirTight: type definitions for Airtight protocol, this should be ported to the target platform.
 *
 * Example implementation for Windows/Posix.
 */
#ifndef __AIRTIGHT_TYPES_H
#define __AIRTIGHT_TYPES_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Boolean type.
 */
typedef bool at_bool_t;

/**
 * 8-bit unsigned type.
 */
typedef uint8_t at_u8_t;

/**
 * 16-bit unsigned type.
 */
typedef uint16_t at_u16_t;

/**
 * 32-bit unsigned type.
 */
typedef uint32_t at_u32_t;

/**
 * 16-bit signed type.
 */
typedef int16_t at_i16_t;

/**
 * 32-bit signed type.
 */
typedef int32_t at_i32_t;

/**
 * Number of priority levels.
 */
#define AIRTIGHT_PRIORITIES 3

/**
 * Max priority level.
 */
#define AIRTIGHT_PRIORITY_MAX (AIRTIGHT_PRIORITIES - 1)

/**
 * Min priority level.
 */
#define AIRTIGHT_PRIORITY_MIN 0

/**
 * Priority level type.
 */
#if (AIRTIGHT_PRIORITIES < 256)
typedef at_u8_t Airtight_Priority;
#elif (AIRTIGHT_PRIORITIES < 65536)
typedef at_u16_t Airtight_Priority;
#endif

/**
 * Criticality levels, typically HIGH and LOW.
 */
typedef enum
{
    HIGH_CRIT,
    LOW_CRIT
} Airtight_Criticality;

/**
 * Node ID type.
 */
typedef at_u8_t Airtight_NodeId;

/**
 * Time representation type.
 */
typedef at_u32_t at_time_t;

/**
 * Time difference type.
 */
typedef at_i32_t at_timediff_t;

#endif
