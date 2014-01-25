#include "remote.hpp"

using namespace std;

NM_id NM_remote::remote_id_count = 0;

NM_remote::NM_remote(string name, in_addr address, NM_id id, NM_id master_id)
{
    this->init(name, address, ip_to_in6_addr(NM_ip6_def_addr), id, master_id);
}

NM_remote::NM_remote(string name, in6_addr address, NM_id id, NM_id master_id)
{
    this->init(name, ip_to_in_addr(NM_ip_def_addr), address, id, master_id);
}

NM_remote::NM_remote(string name, in_addr address, in6_addr address6, NM_id id, NM_id master_id)
{
    this->init(name, address, address6, id, master_id);
}

void NM_remote::init(std::string name, in_addr address, in6_addr address6, NM_id id, NM_id master_id)
{
    if( id == 0 )
        id = NM_remote::generate_remote_id();

    if( id > this->remote_id_count )
        this->remote_id_count = id;

    if(master_id == 0)
        master_id = id;
    
    this->remote_id = id;
    this->master_id = master_id;
    this->remote_name = name;
    this->remote_ip_address = address;
    this->remote_ip6_address = address6;

    this->next = NULL;
    this->prev = NULL;

    this->monitored = NM_monitored;
}

NM_remote::~NM_remote()
{
    if( this->next != NULL )
        this->next->set_prev(this->prev);
    if( this->prev != NULL )
        this->prev->set_next(this->next);
}

void NM_remote::set_next(NM_remote * next)
{
    this->next = next;
}

NM_remote * NM_remote::get_next()
{
    return this->next;
}

void NM_remote::set_prev(NM_remote * prev)
{
    this->prev = prev;
}

NM_remote * NM_remote::get_prev()
{
    return this->prev;
}

void NM_remote::set_monitored(NM_monitored_state monitored)
{
    this->monitored = monitored;
}

NM_monitored_state NM_remote::is_monitored()
{
    return this->monitored;
}

NM_id NM_remote::generate_remote_id()
{
    NM_remote::incr_remote_id_count();
    return get_remote_id_count();
}

NM_id NM_remote::get_remote_id_count()
{
    return NM_remote::remote_id_count;
}

void NM_remote::incr_remote_id_count()
{
    NM_remote::remote_id_count++;
}

NM_id NM_remote::get_remote_id()
{
    return this->remote_id;
}

NM_id NM_remote::get_master_id()
{
    return this->master_id;
}

void NM_remote::set_master_id(NM_id master_id)
{
    this->master_id = master_id;
}

string NM_remote::get_remote_name()
{
    return this->remote_name;
}

bool NM_remote::set_remote_ip(in_addr ip)
{
    if(this->remote_ip_address.s_addr == ip.s_addr)
        return false;
    this->remote_ip_address = ip;
    return true;
}

bool NM_remote::set_remote_ip(std::string ip)
{
    struct in_addr address = ip_to_in_addr(ip);
    if(this->remote_ip_address.s_addr == address.s_addr)
        return false;
    this->remote_ip_address = address;
    return true;
}

bool NM_remote::set_remote_ip6(in6_addr ip)
{
    if(ip_to_string(this->remote_ip_address).compare(ip_to_string(ip)) == 0)
        return false;
    this->remote_ip6_address = ip;
    return true;
}

bool NM_remote::set_remote_ip6(std::string ip)
{
    struct in6_addr address = ip_to_in6_addr(ip);
    if(ip_to_string(this->remote_ip_address).compare(ip) == 0)
        return false;
    this->remote_ip6_address = address;
    return true;
}
string NM_remote::get_remote_ip()
{
    return ip_to_string(this->remote_ip_address);
}

string NM_remote::get_remote_ip6()
{
    return ip_to_string(this->remote_ip6_address);
}

in_addr NM_remote::get_remote_ip_in_addr()
{
    return this->remote_ip_address;
}

in6_addr NM_remote::get_remote_ip_in6_addr()
{
    return this->remote_ip6_address;
}

string NM_remote::out(NM_remote * remote)
{
    ostringstream out;
    out.str("");
    out << " -- Start remote info" << endl;
    if( remote != NULL )
    {
        out << "\tRemote:\t" << remote->get_remote_id() << endl;
        out << "\tName:\t" << remote->get_remote_name() << endl;
        out << "\tIs monitored:\t" << remote->is_monitored() << endl;
        out << "\tIP address v4:\t" << remote->get_remote_ip() << endl;
        out << "\tIP address v6:\t" << remote->get_remote_ip6() << endl;
    }
    else
    {
        out << "\t This remote object doesn't exists" << endl;
    }
    out << " -- End remote info" << endl;
    return out.str();
}

ostream & operator << (ostream & out, NM_remote * remote)
{
    out << NM_remote::out(remote);
    return out;
}

// NM_stream dziedziczy po ostream wiec powinno byc ok
ostream & operator << (NM_stream & out, NM_remote * remote)
{
    out << NM_remote::out(remote);
    return out;
}
