#ifndef NM_PROTOCOL_UDP
#define NM_PROTOCOL_UDP

#include "../../functions/main_h.hpp"
#include "../../functions/mainFunctions.hpp"
#include "../connection.hpp"
#include "../protocol.hpp"

extern NM_connection_list NM_connections_v4;

/* UDP header */
struct NM_header_udp
{
    u_short uh_sport; /* source port */
    u_short uh_dport; /* destination port */
    u_short uh_length; /* length */
    u_short uh_checksum; /* checksum */
};

class NM_protocolUdp : public NM_protocol
{
  public:
    NM_protocolUdp();
    NM_protocolUdp(NM_header_udp * /*packet*/, in_addr /*source_ip*/, in_addr /*dest_ip*/, int /*packet_size*/, int /*ip_size*/);

    virtual void load_connections();

  private:
    std::string generate_list_key(std::string /*local_ip*/, unsigned int /*local_port*/, std::string /*rem_ip*/, unsigned int /*rem_port*/);
};

#endif

