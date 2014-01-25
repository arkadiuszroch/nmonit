#include "device.hpp"

using namespace std;

NM_id NM_device::device_id_count = 0;

NM_device::NM_device(string name, in_addr address, in6_addr address6, NM_id id, NM_id master_id)
{
    if( id == 0 )
        id = NM_device::generate_device_id();
    
    if( id > this->device_id_count )
        this->device_id_count = id;

    if(master_id == 0)
        master_id = id;
    
    this->device_id = id;
    this->master_id = master_id;
    this->device_name = name;
    this->device_ip_address = address;
    this->device_ip6_address = address6;

    this->next = NULL;
    this->prev = NULL;

    this->monitored = NM_monitored;
}

NM_device::~NM_device()
{
    if( this->next != NULL )
        this->next->set_prev(this->prev);
    if( this->prev != NULL )
        this->prev->set_next(this->next);
}

void NM_device::set_next(NM_device * next)
{
    this->next = next;
}

NM_device * NM_device::get_next()
{
    return this->next;
}

void NM_device::set_prev(NM_device * prev)
{
    this->prev = prev;
}

NM_device * NM_device::get_prev()
{
    return this->prev;
}

NM_id NM_device::get_master_id()
{
    return this->master_id;
}

void NM_device::set_master_id(NM_id master_id)
{
    this->master_id = master_id;
}

void NM_device::set_monitored(NM_monitored_state monitored)
{
    this->monitored = monitored;
}

NM_monitored_state NM_device::is_monitored()
{
    return this->monitored;
}

NM_id NM_device::generate_device_id()
{
    NM_device::incr_device_id_count();
    return get_device_id_count();
}

NM_id NM_device::get_device_id_count()
{
    return NM_device::device_id_count;
}

void NM_device::incr_device_id_count()
{
    NM_device::device_id_count++;
}

NM_id NM_device::get_device_id()
{
    return this->device_id;
}

string NM_device::get_device_name()
{
    return this->device_name;
}

string NM_device::get_device_ip()
{
    return ip_to_string(this->device_ip_address);
}

string NM_device::get_device_ip6()
{
    return ip_to_string(this->device_ip6_address);
}

in_addr NM_device::get_device_ip_in_addr()
{
    return this->device_ip_address;
}

in6_addr NM_device::get_device_ip_in6_addr()
{
    return this->device_ip6_address;
}

string NM_device::out(NM_device * device)
{
    ostringstream out;
    
    out.str("");
    out << " -- Start device info" << endl;
    if( device != NULL )
    {
        out << "\tDevice:\t" << device->get_device_id() << endl;
        out << "\tName:\t" << device->get_device_name() << endl;
        out << "\tIs monitored:\t" << device->is_monitored() << endl;
        out << "\tIP addres:\t" << device->get_device_ip() << endl;
        out << "\tIP v6 addres:\t" << device->get_device_ip6() << endl;
    }
    else
        out << "\t Device doesn't exists" << endl;
    out << " -- End device info" << endl;
    return out.str();
}

ostream & operator << (ostream & out, NM_device * device)
{
    out << NM_device::out(device);
    return out;
}

ostream & operator << (NM_stream & out, NM_device * device)
{
    out << NM_device::out(device);
    return out;
}
