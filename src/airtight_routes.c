/**
 * @addtogroup Airtight_Routes
 * @{
 * @file
 * AirTight: route configuration implementation.
 */
#include "airtight_routes.h"

#define HOP(id, destination, next_hop)                               \
    if (current_node_id == id && destination_node_id == destination) \
    {                                                                \
        return next_hop;                                             \
    }

Airtight_NodeId Airtight_NextHop(Airtight_NodeId destination_node_id)
{
    const Airtight_NodeId current_node_id = AT_CONF_NODE_ID;

#include "airtight_routes_config.txt"

    return destination_node_id;
}
