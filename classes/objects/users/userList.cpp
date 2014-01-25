#include "userList.hpp"

using namespace std;

NM_userList * NM_userList::instance = NULL;

NM_userList::NM_userList()
{
    this->first = NULL;
    this->current = NULL;
    this->last = NULL;
}

// Destructor

NM_userList::~NM_userList()
{
    while( this->first )
    {
        NM_user * buf = this->first->get_next();
        delete this->first;
        buf->set_prev(NULL);
        this->first = buf;
    }

    this->current = NULL;
    this->last = NULL;
}

NM_userList * NM_userList::get_instance()
{
    if( NM_userList::instance == NULL )
    {
        NM_userList::instance = new NM_userList();
    }
    return NM_userList::instance;
}

void NM_userList::add_user(NM_user * user)
{
    if( this->first == NULL )
    {
        this->first = user;
        this->current = user;
    }

    if( this->last != NULL )
    {
        this->last->set_next(user);
        user->set_prev(this->last);
    }

    this->last = user;
}

void NM_userList::delete_user(NM_user * user)
{
    if( user == NULL )
        return;

    if( this->first == user )
    {
        this->first = this->current = this->last = NULL;
    }
    else
    {
        if( this->last == user )
            this->last = user->get_prev();
        if( this->current == user )
            this->current = user->get_prev();
    }

    delete user;
}

void NM_userList::delete_user(NM_id user_id)
{
    this->delete_user(this->find_by_id(user_id));
}

NM_user * NM_userList::find_by_id(NM_id id)
{
    this->current = this->first;

    while( this->current )
    {
        if( this->current->get_user_id() == id )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_user * NM_userList::find_by_system_id(NM_id id)
{
    this->current = this->first;

    while( this->current )
    {
        if( this->current->get_user_system_id() == id )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_user * NM_userList::find_by_name(string name)
{
    this->current = this->first;

    while( this->current )
    {
        if( this->current->get_user_name().compare(name) == 0 )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_id NM_userList::get_user_id(NM_id system_id)
{
    NM_user * buf;

    buf = this->find_by_system_id(system_id);
    if( buf == NULL )
        return 0;
    else
        return buf->get_user_id();
}

NM_id NM_userList::get_user_id(string name)
{
    NM_user * buf;

    buf = this->find_by_name(name);
    if( buf == NULL )
        return 0;
    else
        return buf->get_user_id();
}

NM_id NM_userList::get_user_system_id(NM_id id)
{
    NM_user * buf;

    buf = this->find_by_id(id);
    if( buf == NULL )
        return 0;
    else
        return buf->get_user_system_id();
}

NM_id NM_userList::get_user_system_id(string name)
{
    NM_user * buf;

    buf = this->find_by_name(name);
    if( buf == NULL )
        return 0;
    else
        return buf->get_user_system_id();
}

string NM_userList::get_user_name(NM_id id, bool system_id)
{
    NM_user * buf;
    if( system_id )
        buf = this->find_by_system_id(id);
    else
        buf = this->find_by_id(id);

    if( buf == NULL )
        return string("");
    else
        return buf->get_user_name();
}

string NM_userList::out(NM_userList * list)
{
    ostringstream out;
    NM_user * current = list->first;
    
    out.str("");
    out << "User list:" << endl;
    while( current )
    {
        out << "\tID:\t" << current->get_user_id() << endl;
        out << "\tSystem ID:\t" << current->get_user_system_id() << endl;
        out << "\tName:\t" << current->get_user_name() << endl;
        out << "\tIs monitored:\t" << current->is_monitored() << endl;

        current = current->get_next();
        if( current != NULL )
            out << "-----------------" << endl;
    }
    out << "End user list" << endl;
    return out.str();
}


ostream & operator << (ostream & out, NM_userList * list)
{
    out << NM_userList::out(list);
    return out;
}

ostream & operator << (NM_stream & out, NM_userList * list)
{
    out << NM_userList::out(list);
    return out;
}