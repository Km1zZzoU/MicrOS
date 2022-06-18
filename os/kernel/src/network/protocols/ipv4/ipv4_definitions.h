/*
    @JakubPrzystasz
    Created: 17.02.2021
    Modify: 
*/
#ifndef network_ipv4_definitions
#define network_ipv4_definitions

#include "../../network_definitions.h"
#include <stdint.h>

#define IPv4_PROTOCOL_TYPE 0x0800
#define IPv4_PROTOCOL_VERSION 0x4


typedef struct ipv4_packet
{
    char version_ihl_ptr[0];
    uint8_t version : 4;
    uint8_t ihl : 4;
    uint8_t tos;
    uint16_t length;
    uint16_t id;
    char flags_fragment_ptr[0];
    uint8_t flags : 3;
    uint8_t fragment_offset_high : 5;
    uint8_t fragment_offset_low;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    uint8_t data[];
} __attribute__((packed)) ipv4_packet_t;

typedef struct icmp_header
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint8_t data[];
} __attribute__((packed)) icmp_header;

#endif