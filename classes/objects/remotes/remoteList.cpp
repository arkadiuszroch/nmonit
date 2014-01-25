#include "remoteList.hpp"

using namespace std;

NM_remoteList * NM_remoteList::instance = NULL;

NM_remoteList::NM_remoteList()
{
    this->first = NULL;
    this->current = NULL;
    this->last = NULL;
}

// Destructor

NM_remoteList::~NM_remoteList()
{
    while( this->first )
    {
        NM_remote * buf = this->first->get_next();
        delete this->first;
        buf->set_prev(NULL);
        this->first = buf;
    }

    this->current = NULL;
    this->last = NULL;
}

NM_remoteList * NM_remoteList::get_instance()
{
    if( NM_remoteList::instance == NULL )
        NM_remoteList::instance = new NM_remoteList();
    return NM_remoteList::instance;
}

void NM_remoteList::add_remote(NM_remote * remote)
{
    if( this->first == NULL )
    {
        this->first = remote;
        this->current = remote;
    }

    if( this->last != NULL )
    {
        this->last->set_next(remote);
        remote->set_prev(this->last);
    }

    this->last = remote;
}

void NM_remoteList::delete_remote(NM_remote * remote)
{
    if( remote == NULL )
        return;

    if( this->first == remote )
        this->first = this->current = this->last = NULL;
    else
    {
        if( this->last == remote )
            this->last = remote->get_prev();
        if( this->current == remote )
            this->current = remote->get_prev();
    }

    delete remote;
}

void NM_remoteList::delete_remote(NM_id remote_id)
{
    this->delete_remote(this->find_by_id(remote_id));
}

NM_remote * NM_remoteList::find_by_id(NM_id id)
{
    this->current = this->first;

    while( this->current )
    {
        if( this->current->get_remote_id() == id )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_remote * NM_remoteList::find_by_name(string name)
{
    this->current = this->first;

    while( this->current )
    {
        if( this->current->get_remote_name().compare(name) == 0 )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_remote * NM_remoteList::find_by_ip(in_addr ip)
{
    this->current = this->first;

    while( this->current )
    {
        if( this->current->get_remote_ip_in_addr().s_addr == ip.s_addr )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_remote * NM_remoteList::find_by_ip(in6_addr ip)
{
    this->current = this->first;

    while( this->current )
    {
        if( ip_to_string(this->current->get_remote_ip_in6_addr()).compare(ip_to_string(ip)) == 0 )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_remote * NM_remoteList::find_by_ip(string ip, NM_ip_type ip_type)
{
    this->current = this->first;

    if(ip_type == NM_ip_v4)
    {
        while( this->current )
        {
            if( this->current->get_remote_ip().compare(ip) == 0 )
                return this->current;
            else
                this->current = this->current->get_next();
        }
    }
    else
    {
        while( this->current ) 
        {
            if( this->current->get_remote_ip6().compare(ip) == 0 )
                return this->current;
            else
                this->current = this->current->get_next();
        }
    }

    return NULL;
}

NM_id NM_remoteList::get_remote_id(string name)
{
    NM_remote * buf;
    buf = NULL;

    buf = this->find_by_name(name);
    if( buf == NULL )
        buf = this->find_by_ip(name);

    if( buf == NULL )
        return 0;
    else
        return buf->get_remote_id();
}

NM_id NM_remoteList::get_remote_id(in_addr ip)
{
    NM_remote * buf = this->find_by_ip(ip);
    if( buf == NULL )
        return 0;
    else
        return buf->get_remote_id();
}

NM_id NM_remoteList::get_remote_id(in6_addr ip)
{
    NM_remote * buf = this->find_by_ip(ip);
    if( buf == NULL )
        return 0;
    else
        return buf->get_remote_id();
}

NM_id NM_remoteList::get_remote_id(string ip, NM_ip_type ip_type)
{
    NM_remote * buf = this->find_by_ip(ip, ip_type);
    if( buf == NULL )
        return 0;
    else
        return buf->get_remote_id();
}

string NM_remoteList::get_remote_name(NM_id id)
{
    NM_remote * buf = this->find_by_id(id);
    if( buf == NULL )
        return string("");
    else
        return buf->get_remote_name();
}

string NM_remoteList::get_remote_name(in_addr ip)
{
    NM_remote * buf = this->find_by_ip(ip);
    if( buf == NULL )
        return string("");
    else
        return buf->get_remote_name();
}

string NM_remoteList::get_remote_name(in6_addr ip)
{
    NM_remote * buf = this->find_by_ip(ip);
    if( buf == NULL )
        return string("");
    else
        return buf->get_remote_name();
}

string NM_remoteList::get_remote_name(string ip, NM_ip_type ip_type)
{
    NM_remote * buf = this->find_by_ip(ip, ip_type);
    if( buf == NULL )
        return string("");
    else
        return buf->get_remote_name();
}

string NM_remoteList::get_remote_ip(NM_id id)
{
    NM_remote * buf = this->find_by_id(id);
    if( buf == NULL )
        return string(NM_ip_def_addr);
    else
        return buf->get_remote_ip();
}

string NM_remoteList::get_remote_ip(string name)
{
    NM_remote * buf = this->find_by_name(name);

    if( buf == NULL )
        return string(NM_ip_def_addr);
    else
        return buf->get_remote_ip();
}

string NM_remoteList::get_remote_ip(in6_addr ip)
{
    NM_remote * buf = this->find_by_ip(ip);

    if( buf == NULL )
        return string(NM_ip_def_addr);
    else
        return buf->get_remote_ip();
}

string NM_remoteList::get_remote_ip6(NM_id id)
{
    NM_remote * buf = this->find_by_id(id);
    if( buf == NULL )
        return string(NM_ip6_def_addr);
    else
        return buf->get_remote_ip();
}

string NM_remoteList::get_remote_ip6(string name)
{
    NM_remote * buf = this->find_by_name(name);

    if( buf == NULL )
        return string(NM_ip6_def_addr);
    else
        return buf->get_remote_ip();
}

string NM_remoteList::get_remote_ip6(in_addr ip)
{
    NM_remote * buf = this->find_by_ip(ip);

    if( buf == NULL )
        return string(NM_ip6_def_addr);
    else
        return buf->get_remote_ip();
}

in_addr NM_remoteList::get_remote_ip_in_addr(NM_id id)
{
    NM_remote * buf = this->find_by_id(id);

    if( buf == NULL )
        return ip_to_in_addr(NM_ip_def_addr);
    else
        return buf->get_remote_ip_in_addr();
}

in_addr NM_remoteList::get_remote_ip_in_addr(string name)
{
    NM_remote * buf = this->find_by_name(name);

    if( buf == NULL )
        return ip_to_in_addr(NM_ip_def_addr);
    else
        return buf->get_remote_ip_in_addr();
}

in_addr NM_remoteList::get_remote_ip_in_addr(in6_addr ip)
{
    NM_remote * buf = this->find_by_ip(ip);

    if( buf == NULL )
        return ip_to_in_addr(NM_ip_def_addr);
    else
        return buf->get_remote_ip_in_addr();
}

in6_addr NM_remoteList::get_remote_ip_in6_addr(NM_id id)
{
    NM_remote * buf = this->find_by_id(id);

    if( buf == NULL )
        return ip_to_in6_addr(NM_ip6_def_addr);
    else
        return buf->get_remote_ip_in6_addr();
}

in6_addr NM_remoteList::get_remote_ip_in6_addr(string name)
{
    NM_remote * buf = this->find_by_name(name);

    if( buf == NULL )
        return ip_to_in6_addr(NM_ip6_def_addr);
    else
        return buf->get_remote_ip_in6_addr();
}

in6_addr NM_remoteList::get_remote_ip_in6_addr(in_addr ip)
{
    NM_remote * buf = this->find_by_ip(ip);

    if( buf == NULL )
        return ip_to_in6_addr(NM_ip6_def_addr);
    else
        return buf->get_remote_ip_in6_addr();
}

string NM_remoteList::out(NM_remoteList * list)
{
    ostringstream out;
    NM_remote * current = list->first;
    
    out.str("");
    out << "Device list:" << endl;
    while( current )
    {
        out << "\tID:\t" << current->get_remote_id() << endl;
        out << "\tName:\t" << current->get_remote_name() << endl;
        out << "\tIs monitored:\t" << current->is_monitored() << endl;
        out << "\tIP addres:\t" << current->get_remote_ip() << endl;
        out << "\tIP in_addr:\t" << current->get_remote_ip_in_addr().s_addr << endl;

        current = current->get_next();
        if( current != NULL )
            out << "-----------------" << endl;
    }
    out << "End remote list" << endl;
    return out.str();
}

ostream & operator << (ostream & out, NM_remoteList * list)
{
    out << NM_remoteList::out(list);
    return out;
}

ostream & operator << (NM_stream & out, NM_remoteList * list)
{
    out << NM_remoteList::out(list);
    return out;
}