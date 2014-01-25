#include "deviceList.hpp"

using namespace std;

NM_deviceList * NM_deviceList::instance = NULL;

NM_deviceList::NM_deviceList()
{
    this->first = NULL;
    this->current = NULL;
    this->last = NULL;
}

// Destructor

NM_deviceList::~NM_deviceList()
{
    while( this->first )
    {
        NM_device * buf = this->first->get_next();
        delete this->first;
        buf->set_prev(NULL);
        this->first = buf;
    }

    this->current = NULL;
    this->last = NULL;
}

NM_deviceList * NM_deviceList::get_instance()
{
    if( NM_deviceList::instance == NULL )
        NM_deviceList::instance = new NM_deviceList();
    return NM_deviceList::instance;
}

void NM_deviceList::add_device(NM_device * device)
{
    if( this->first == NULL )
    {
        this->first = device;
        this->current = device;
    }

    if( this->last != NULL )
    {
        this->last->set_next(device);
        device->set_prev(this->last);
    }

    this->last = device;
}

void NM_deviceList::delete_device(NM_device * device)
{
    if( device == NULL )
        return;

    if( this->first == device )
        this->first = this->current = this->last = NULL;
    else
    {
        if( this->last == device )
            this->last = device->get_prev();
        if( this->current == device )
            this->current = device->get_prev();
    }

    delete device;
}

void NM_deviceList::delete_device(NM_id device_id)
{
    this->delete_device(this->find_by_id(device_id));
}

NM_device * NM_deviceList::get_first()
{
    return this->first;
}

NM_device * NM_deviceList::find_by_id(NM_id id)
{
    this->current = this->first;

    while( this->current )
    {
        if( this->current->get_device_id() == id )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_device * NM_deviceList::find_by_name(string name)
{
    this->current = this->first;

    while( this->current )
    {
        if( this->current->get_device_name().compare(name) == 0 )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_device * NM_deviceList::find_by_ip(in_addr ip)
{
    this->current = this->first;

    while( this->current )
    {
        if( this->current->get_device_ip_in_addr().s_addr == ip.s_addr )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_device * NM_deviceList::find_by_ip(in6_addr ip)
{
    this->current = this->first;

    while( this->current )
    {
        if( ip_to_string(this->current->get_device_ip_in6_addr()).compare(ip_to_string(ip)) == 0 )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_device * NM_deviceList::find_by_ip(string ip, NM_ip_type ip_type)
{
    this->current = this->first;

    if(ip_type == NM_ip_v4)
    {
        while( this->current )
        {
            if( this->current->get_device_ip().compare(ip) == 0 )
                return this->current;
            else
                this->current = this->current->get_next();
        }
    }
    else
    {
        while( this->current )
        {
            if( this->current->get_device_ip6() == ip )
                return this->current;
            else
                this->current = this->current->get_next();
        }
    }

    return NULL;
}

NM_id NM_deviceList::get_device_id(string name)
{
    NM_device * buf;
    buf = NULL;

    buf = this->find_by_name(name);
    if( buf == NULL )
        buf = this->find_by_ip(name);

    if( buf == NULL )
        return 0;
    else
        return buf->get_device_id();
}

NM_id NM_deviceList::get_device_id(in_addr ip)
{
    NM_device * buf = this->find_by_ip(ip);
    if( buf == NULL )
        return 0;
    else
        return buf->get_device_id();
}

NM_id NM_deviceList::get_device_id(in6_addr ip)
{
    NM_device * buf = this->find_by_ip(ip);
    if( buf == NULL )
        return 0;
    else
        return buf->get_device_id();
}

NM_id NM_deviceList::get_device_id(std::string ip, NM_ip_type ip_type)
{
    NM_device * buf = this->find_by_ip(ip, ip_type);
    if( buf == NULL )
        return 0;
    else
        return buf->get_device_id();
}

string NM_deviceList::get_device_name(NM_id id)
{
    NM_device * buf = this->find_by_id(id);
    if( buf == NULL )
        return string("");
    else
        return buf->get_device_name();
}

string NM_deviceList::get_device_name(in_addr ip)
{
    NM_device * buf = this->find_by_ip(ip);
    if( buf == NULL )
        return string("");
    else
        return buf->get_device_name();
}

string NM_deviceList::get_device_name(in6_addr ip)
{
    NM_device * buf = this->find_by_ip(ip);
    if( buf == NULL )
        return string("");
    else
        return buf->get_device_name();
}

string NM_deviceList::get_device_name(string ip, NM_ip_type ip_type)
{
    NM_device * buf = this->find_by_ip(ip, ip_type);
    if( buf == NULL )
        return string("");
    else
        return buf->get_device_name();
}

string NM_deviceList::get_device_ip(NM_id id)
{
    NM_device * buf = this->find_by_id(id);
    if( buf == NULL )
        return string(NM_ip_def_addr);
    else
        return buf->get_device_ip();
}

string NM_deviceList::get_device_ip(string name)
{
    NM_device * buf = this->find_by_name(name);

    if( buf == NULL )
        return string(NM_ip_def_addr);
    else
        return buf->get_device_ip();
}

string NM_deviceList::get_device_ip(in6_addr ip)
{
    NM_device * buf = this->find_by_ip(ip);

    if( buf == NULL )
        return string(NM_ip_def_addr);
    else
        return buf->get_device_ip();
}

string NM_deviceList::get_device_ip6(NM_id id)
{
    NM_device * buf = this->find_by_id(id);
    if( buf == NULL )
        return string(NM_ip6_def_addr);
    else
        return buf->get_device_ip6();
}

string NM_deviceList::get_device_ip6(string name)
{
    NM_device * buf = this->find_by_name(name);

    if( buf == NULL )
        return string(NM_ip6_def_addr);
    else
        return buf->get_device_ip6();
}

string NM_deviceList::get_device_ip6(in_addr ip)
{
    NM_device * buf = this->find_by_ip(ip);

    if( buf == NULL )
        return string(NM_ip6_def_addr);
    else
        return buf->get_device_ip6();
}

in_addr NM_deviceList::get_device_ip_in_addr(NM_id id)
{
    NM_device * buf = this->find_by_id(id);

    if( buf == NULL )
        return ip_to_in_addr(NM_ip_def_addr);
    else
        return buf->get_device_ip_in_addr();
}

in_addr NM_deviceList::get_device_ip_in_addr(string name)
{
    NM_device * buf = this->find_by_name(name);

    if( buf == NULL )
        return ip_to_in_addr(NM_ip_def_addr);
    else
        return buf->get_device_ip_in_addr();
}

in_addr NM_deviceList::get_device_ip_in_addr(in6_addr ip)
{
    NM_device * buf = this->find_by_ip(ip);

    if( buf == NULL )
        return ip_to_in_addr(NM_ip_def_addr);
    else
        return buf->get_device_ip_in_addr();
}

in6_addr NM_deviceList::get_device_ip_in6_addr(NM_id id)
{
    NM_device * buf = this->find_by_id(id);

    if( buf == NULL )
        return ip_to_in6_addr(NM_ip6_def_addr);
    else
        return buf->get_device_ip_in6_addr();
}

in6_addr NM_deviceList::get_device_ip_in6_addr(string name)
{
    NM_device * buf = this->find_by_name(name);

    if( buf == NULL )
        return ip_to_in6_addr(NM_ip6_def_addr);
    else
        return buf->get_device_ip_in6_addr();
}

in6_addr NM_deviceList::get_device_ip_in6_addr(in_addr ip)
{
    NM_device * buf = this->find_by_ip(ip);

    if( buf == NULL )
        return ip_to_in6_addr(NM_ip6_def_addr);
    else
        return buf->get_device_ip_in6_addr();
}

string NM_deviceList::out(NM_deviceList * list)
{
    ostringstream out;
    NM_device * current = list->first;
    
    out.str("");
    out << "Device list:" << endl;
    while( current )
    {
        out << "\tID:\t" << current->get_device_id() << endl;
        out << "\tName:\t" << current->get_device_name() << endl;
        out << "\tIs monitored:\t" << current->is_monitored() << endl;
        out << "\tIP addres:\t" << current->get_device_ip() << endl;
        out << "\tIP v6 addres:\t" << current->get_device_ip6() << endl;
        
        current = current->get_next();
        if( current != NULL )
            out << "-----------------" << endl;
    }
    out << "End device list" << endl;
    return out.str();
}

ostream & operator <<(NM_stream & out, NM_deviceList * list)
{
    out << NM_deviceList::out(list);
    return out;
}
ostream & operator <<(ostream & out, NM_deviceList * list)
{
    out << NM_deviceList::out(list);
    return out;
}