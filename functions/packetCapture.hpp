#ifndef NM_PACKET_CAPTURE
#define NM_PACKET_CAPTURE

#include "global.hpp"
#include "objects.hpp"
#include "main_h.hpp"
#include "../classes/protocols/tcp.hpp"
#include "../classes/protocols/udp.hpp"
#include <pcap.h>
#include <netinet/ip6.h> // ip6 structure

/* Default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* Ethernet headers are always exactly 14 bytes */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

/* IP type is 2048 in dec, in hex is 0x0800*/
#define ETHER_TYPE_IP 0x0800
/* IP v6 type is 34525 in dec, in hex is 0x86DD*/
#define ETHER_TYPE_IP6 0x86DD

struct NM_ethernet
{
    u_char ether_dhost[ETHER_ADDR_LEN]; /* destination host address */
    u_char ether_shost[ETHER_ADDR_LEN]; /* source host address */
    u_short ether_type; /* IP? ARP? RARP? etc */
};

/* IP header */
struct NM_header_ip
{
    u_char ip_vhl; /* version << 4 | header length >> 2 */
    u_char ip_tos; /* type of service */
    u_short ip_len; /* total length */
    u_short ip_id; /* identification */
    u_short ip_off; /* fragment offset field */
#define IP_RF 0x8000            /* reserved fragment flag */
#define IP_DF 0x4000            /* dont fragment flag */
#define IP_MF 0x2000            /* more fragments flag */
#define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
    u_char ip_ttl; /* time to live */
    u_char ip_p; /* protocol */
    u_short ip_sum; /* checksum */
    struct in_addr ip_src, ip_dst; /* source and dest address */
};
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)

void * NM_capture_packets(void * /*device*/);

void NM_get_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

void NM_finish_thread(void * /*ptr*/);

#endif

