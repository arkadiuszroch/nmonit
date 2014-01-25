#include "programList.hpp"

using namespace std;

NM_programList * NM_programList::instance = NULL;

NM_programList::NM_programList()
{
    this->first = NULL;
    this->current = NULL;
    this->last = NULL;
}

// Destructor

NM_programList::~NM_programList()
{
    while( this->first )
    {
        NM_program * buf = this->first->get_next();
        delete this->first;
        buf->set_prev(NULL);
        this->first = buf;
    }

    this->current = NULL;
    this->last = NULL;
}

NM_programList * NM_programList::get_instance()
{
    if( NM_programList::instance == NULL )
    {
        NM_programList::instance = new NM_programList();
    }
    return NM_programList::instance;
}

void NM_programList::add_program(NM_program * program)
{
    if( this->first == NULL )
    {
        this->first = program;
        this->current = program;
    }

    if( this->last != NULL )
    {
        this->last->set_next(program);
        program->set_prev(this->last);
    }

    this->last = program;
}

void NM_programList::delete_program(NM_program * program)
{
    if( program == NULL )
        return;

    if( this->first == program )
    {
        this->first = this->current = this->last = NULL;
    }
    else
    {
        if( this->last == program )
            this->last = program->get_prev();
        if( this->current == program )
            this->current = program->get_prev();
    }

    delete program;
}

void NM_programList::delete_program(NM_id program_id)
{
    this->delete_program(this->find_by_id(program_id));
}

NM_program * NM_programList::find_by_id(NM_id id)
{
    this->current = this->first;

    while( this->current )
    {
        if( this->current->get_program_id() == id )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_program * NM_programList::find_by_name(string name)
{
    this->current = this->first;

    while( this->current )
    {
        if( this->current->get_program_name().compare(name) == 0 )
            return this->current;
        else
            this->current = this->current->get_next();
    }

    return NULL;
}

NM_id NM_programList::get_program_id(string name)
{
    NM_program * buf = NULL;

    buf = this->find_by_name(name);

    if( buf == NULL )
        return 0;
    else
        return buf->get_program_id();
}

string NM_programList::get_program_name(NM_id id)
{
    NM_program * buf = this->find_by_id(id);
    if( buf == NULL )
        return string("");
    else
        return buf->get_program_name();
}

string NM_programList::out(NM_programList * list)
{
    ostringstream out;
    NM_program * current = list->first;
    
    out.str("");
    out << "Program list:" << endl;
    while( current )
    {
        out << "\tID:\t" << current->get_program_id() << endl;
        out << "\tName:\t" << current->get_program_name() << endl;
        out << "\tIs monitored:\t" << current->is_monitored() << endl;

        current = current->get_next();
        if( current != NULL )
            out << "-----------------" << endl;
    }
    out << "End program list" << endl;
    return out.str();
}

ostream & operator << (ostream & out, NM_programList * list)
{
    out << NM_programList::out(list);
    return out;
}

ostream & operator << (NM_stream & out, NM_programList * list)
{
    out << NM_programList::out(list);
    return out;
}