/**
 * @addtogroup Airtight_Utilities
 * @{
 * @file
 * AirTight: utility functions and definitions implementation.
 */
#include "airtight_utilities.h"

#define INDENT "    "

void Airtight_PrintPacket(Airtight_Packet *packet)
{
    printf("Packet at %016zx\n", (size_t)packet);
    puts("{");

    printf(INDENT "       Priority : %u\n", packet->data.fields.priority);
    printf(INDENT "    Criticality : %u\n", packet->data.fields.criticality);
    printf(INDENT "        Flow Id : %u\n", packet->data.fields.flow_id);
    printf(INDENT "         Source : %u\n", packet->data.fields.source);
    printf(INDENT "    Destination : %u\n", packet->data.fields.destination);
    printf(INDENT "     Hop Source : %u\n", packet->data.fields.hop_source);
    printf(INDENT "Hop Destination : %u\n", packet->data.fields.hop_destination);
    printf(INDENT "        C Value : %u\n", packet->data.fields.c_value);
    printf(INDENT "Sequence Number : %u\n", packet->data.fields.sequence_number);
    printf("%s",
           INDENT "           Data : ");
    for (size_t i = 0; i < sizeof(packet->data.fields.data); i++)
    {
        printf("%02x", packet->data.fields.data[i]);
    }
    puts("\n}");
}
