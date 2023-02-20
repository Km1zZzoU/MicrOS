#include "ipv4.h"

kvector *fragments;

uint32_t ipv4_process_packet(nic_data_t *data)
{
    ipv4_packet_t *packet = ((ethernet_frame_t *)(data->frame))->data;

    if (packet->version != IPv4_PROTOCOL_VERSION)
        return 0;

    char tmp[32];

    kernel_sprintf(tmp, "IP SRC: %d.%d.%d.%d",
                   packet->src.oct_a,
                   packet->src.oct_b,
                   packet->src.oct_c,
                   packet->src.oct_d);

    logger_log_ok(tmp);

#ifndef TRUST_ME_BRO
    // verify checksum
    uint16_t checksum = packet->header_checksum;
    if (checksum != ipv4_checksum(packet))
        return 0;
#endif

    // TODO: replying fragmented data
    //  Assembly packet if it is fragmented
    // for (uint16_t i = 0; i < fragments->count; i++)
    // {
    //     nic_data_t *nic = (nic_data_t *)fragments->data[i];
    //     ipv4_packet_t *pkt = (ipv4_packet_t *)(nic->frame + sizeof(ethernet_frame_t));
    //     if (pkt->src.address == packet->src.address && pkt->id == packet->id)
    //     {
    //         // assume that data->frame buffer is big enough (driver sets its size to 1.5KB)
    //         uint16_t offset = (packet->offset << 8 | packet->offset2);
    //         memcpy(pkt->data + offset * 8, packet->data, ntohs(packet->length) - packet->ihl);
    //         data->keep = false;
    //         if (packet->flags_mf == IPv4_FLAG_LAST_FRAGMENT)
    //         {
    //             data = nic;
    //             packet = (ipv4_packet_t *)(data->frame + sizeof(ethernet_frame_t));
    //             goto process;
    //         }
    //         return;
    //     }
    // }

    // if (packet->flags_mf == IPv4_FLAG_MORE_FRAGMENTS)
    // {
    //     kvector_add(fragments, data);
    //     data->keep = true;
    //     return;
    // }

process:
    switch (packet->protocol)
    {
    case IP_PROTOCOL_ICMP:
        return icmp_process_packet(data);
    case IP_PROTOCOL_UDP:
        return udp_process_datagram(data);
    case IP_PROTOCOL_TCP:
        return tcp_process_segment(data);
    default:
        return 0;
    }
}

void ipv4_init()
{
    fragments = heap_kernel_alloc(sizeof(kvector), 0);
    kvector_init(fragments);
}

uint16_t ipv4_checksum(ipv4_packet_t *packet)
{
    packet->header_checksum = 0;
    return packet->header_checksum = __ip_wrapsum(__ip_checksum((unsigned char *)packet, sizeof(ipv4_packet_t), 0));
}

nic_data_t *ipv4_create_packet(net_device_t *device, uint8_t protocol, ipv4_addr_t dst, uint32_t data_size)
{
    nic_data_t *data = ethernet_create_frame(device, IPv4_PROTOCOL_TYPE, data_size + sizeof(ipv4_packet_t));
    ipv4_packet_t *packet = ((ethernet_frame_t *)(data->frame))->data;
    static uint16_t id = 0;
    uint8_t options_length = 0;

    packet->version = IPv4_PROTOCOL_VERSION;
    packet->ihl = ((sizeof(ipv4_packet_t) + options_length) / 4);
    packet->tos = 0;
    packet->flags_offset = 0;
    packet->df = 1;
    packet->ttl = device->interface.ttl;
    packet->protocol = protocol;
    packet->length = htons((sizeof(ipv4_packet_t) + options_length) + data_size);
    packet->id = htons(id++);
    packet->src = device->interface.ipv4_address;
    packet->dst = dst;

    return data;
}

uint32_t ipv4_send_packet(nic_data_t *data)
{
    ipv4_packet_t *packet = ((ethernet_frame_t *)(data->frame))->data;
#ifndef TRUST_ME_BRO
    ipv4_checksum(packet);
#endif
    return ethernet_send_frame(data);
}

uint8_t *ipv4_get_data_ptr(ipv4_packet_t *packet)
{
    return (uint8_t *)packet + (packet->length - packet->ihl * 4);
}
