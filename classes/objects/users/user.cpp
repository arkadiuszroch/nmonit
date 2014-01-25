#include "user.hpp"

using namespace std;

NM_id NM_user::user_id_count = 0;

NM_user::NM_user(NM_id system_id, string name, NM_id id, NM_id master_id)
{
    if( id == 0 )
        id = NM_user::generate_user_id();

    if( id > this->user_id_count )
        this->user_id_count = id;

    if(master_id == 0)
        master_id = id;
    
    this->user_id = id;
    this->master_id = master_id;
    this->user_system_id = system_id;
    this->user_name = name;

    this->next = NULL;
    this->prev = NULL;

    this->monitored = NM_monitored;
}

NM_user::~NM_user()
{
    if( this->next != NULL )
        this->next->set_prev(this->prev);
    if( this->prev != NULL )
        this->prev->set_next(this->next);
}

void NM_user::set_next(NM_user * next)
{
    this->next = next;
}

NM_user * NM_user::get_next()
{
    return this->next;
}

void NM_user::set_prev(NM_user * prev)
{
    this->prev = prev;
}

NM_user * NM_user::get_prev()
{
    return this->prev;
}

void NM_user::set_monitored(NM_monitored_state monitored)
{
    this->monitored = monitored;
}

NM_monitored_state NM_user::is_monitored()
{
    return this->monitored;
}

NM_id NM_user::generate_user_id()
{
    NM_user::incr_user_id_count();
    return get_user_id_count();
}

NM_id NM_user::get_user_id_count()
{
    return NM_user::user_id_count;
}

void NM_user::incr_user_id_count()
{
    NM_user::user_id_count++;
}

NM_id NM_user::get_user_id()
{
    return this->user_id;
}

NM_id NM_user::get_master_id()
{
    return this->master_id;
}

void NM_user::set_master_id(NM_id master_id)
{
    this->master_id = master_id;
}

NM_id NM_user::get_user_system_id()
{
    return this->user_system_id;
}

string NM_user::get_user_name()
{
    return this->user_name;
}

string NM_user::out(NM_user * user)
{
    ostringstream out;
    
    out.str("");
    out << " -- Start user info" << endl;
    if( user != NULL )
    {
        out << "\tUser:\t" << user->get_user_id() << endl;
        out << "\tSystem id:\t" << user->get_user_system_id() << endl;
        out << "\tName:\t" << user->get_user_name() << endl;
        out << "\tIs monitored:\t" << user->is_monitored() << endl;
    }
    else
    {
        out << "\t This user doesn't exists" << endl;
    }
    out << " -- End user info" << endl;
    return out.str();
}


ostream & operator << (ostream & out, NM_user * user)
{
    out << NM_user::out(user);
    return out;
}

ostream & operator << (NM_stream & out, NM_user * user)
{
    out << NM_user::out(user);
    return out;
}