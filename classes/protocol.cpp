#include "protocol.hpp"

NM_protocol::~NM_protocol()
{

}

unsigned int NM_protocol::get_inode()
{
    return this->inode;
}

unsigned int NM_protocol::get_local_port()
{
    return this->local_port;
}

in_addr NM_protocol::get_local_ip()
{
    return this->local_ip;
}

unsigned int NM_protocol::get_rem_port()
{
    return this->rem_port;
}

in_addr NM_protocol::get_rem_ip()
{
    return this->rem_ip;
}

unsigned int NM_protocol::get_size()
{
    return this->size;
}

void NM_protocol::init()
{
    this->local_port = 0;
    this->rem_port = 0;

    this->local_ip = ip_to_in_addr("0.0.0.0");
    this->rem_ip = ip_to_in_addr("0.0.0.0");

    this->size = 0;
}

