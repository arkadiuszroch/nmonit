#include "program.hpp"

using namespace std;

NM_id NM_program::program_id_count = 0;

NM_program::NM_program(string name, NM_id id, NM_id master_id)
{
    if( id == 0 )
        id = NM_program::generate_program_id();

    if( id > this->program_id_count )
        this->program_id_count = id;

    if(master_id == 0)
        master_id = id;
    
    this->program_id = id;
    this->master_id = master_id;
    this->program_name = name;

    this->next = NULL;
    this->prev = NULL;

    this->monitored = NM_monitored;
}

NM_program::~NM_program()
{
    if( this->next != NULL )
        this->next->set_prev(this->prev);
    if( this->prev != NULL )
        this->prev->set_next(this->next);
}

void NM_program::set_next(NM_program * next)
{
    this->next = next;
}

NM_program * NM_program::get_next()
{
    return this->next;
}

void NM_program::set_prev(NM_program * prev)
{
    this->prev = prev;
}

NM_program * NM_program::get_prev()
{
    return this->prev;
}

void NM_program::set_monitored(NM_monitored_state monitored)
{
    this->monitored = monitored;
}

NM_monitored_state NM_program::is_monitored()
{
    return this->monitored;
}

NM_id NM_program::generate_program_id()
{
    NM_program::incr_program_id_count();
    return get_program_id_count();
}

NM_id NM_program::get_program_id_count()
{
    return NM_program::program_id_count;
}

void NM_program::incr_program_id_count()
{
    NM_program::program_id_count++;
}

NM_id NM_program::get_program_id()
{
    return this->program_id;
}

NM_id NM_program::get_master_id()
{
    return this->master_id;
}

void NM_program::set_master_id(NM_id master_id)
{
    this->master_id = master_id;
}

string NM_program::get_program_name()
{
    return this->program_name;
}

string NM_program::out(NM_program * program)
{
    ostringstream out;
    
    out.str("");
    out << " -- Start program info" << endl;
    if( program != NULL )
    {
        out << "\tProgram:\t" << program->get_program_id() << endl;
        out << "\tName:\t" << program->get_program_name() << endl;
        out << "\tIs monitored:\t" << program->is_monitored() << endl;
    }
    else
    {
        out << "\t This program doesn't exists" << endl;
    }
    out << " -- End program info" << endl;
    return out.str();
}

ostream & operator << (ostream & out, NM_program * program)
{
    out << NM_program::out(program);
    return out;
}

ostream & operator << (NM_stream & out, NM_program * program)
{
    out << NM_program::out(program);
    return out;
}