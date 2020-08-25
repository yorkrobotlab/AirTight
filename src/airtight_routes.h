/**
 * @addtogroup Airtight_Routes
 * @{
 * @file
 * AirTight: route configuration header.
 */
#ifndef __AIRTIGHT_ROUTES_H
#define __AIRTIGHT_ROUTES_H

#include "airtight_types.h"
#include "airtight_mac_config.h"
#include "airtight_utilities.h"

Airtight_NodeId Airtight_NextHop(Airtight_NodeId destination_node_id);

#endif
