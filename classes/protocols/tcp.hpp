#ifndef NM_PROTOCOL_TCP
#define NM_PROTOCOL_TCP

#include "../../functions/main_h.hpp"
#include "../../functions/mainFunctions.hpp"
#include "../connection.hpp"
#include "../protocol.hpp"

extern NM_connection_list NM_connections_v4;

/* TCP header */

//typedef u_int tcp_seq;

struct NM_header_tcp
{
    u_short th_sport; /* source port */
    u_short th_dport; /* destination port */
    u_int th_seq; /* sequence number */
    u_int th_ack; /* acknowledgement number */
    u_char th_offx2; /* data offset, rsvd */
#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
    u_char th_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short th_win; /* window */
    u_short th_sum; /* checksum */
    u_short th_urp; /* urgent pointer */
};

class NM_protocolTcp : public NM_protocol
{
  public:
    NM_protocolTcp();
    NM_protocolTcp(NM_header_tcp * /*packet*/, in_addr /*source_ip*/, in_addr /*dest_ip*/, int /*packet_size*/, int /*ip_size*/);

    virtual void load_connections();

  private:
    virtual void load_file();
    std::string generate_list_key(std::string /*local_ip*/, unsigned int /*local_port*/, std::string /*rem_ip*/, unsigned int /*rem_port*/);
};

#endif

