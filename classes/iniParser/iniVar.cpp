#include "iniVar.hpp"

using namespace std;

NM_iniVar::NM_iniVar(string name, string value, string comment)
{
    this->next = NULL;
    this->prev = NULL;
    this->name = name;
    this->value = value;
    this->comment = "";
    this->add_comment(comment);
}

NM_iniVar::~NM_iniVar()
{
    if( this->next != NULL )
        this->next->set_prev(this->prev);
    if( this->prev != NULL )
        this->prev->set_next(this->next);
}

void NM_iniVar::set_next(NM_iniVar * var)
{
    this->next = var;
}

void NM_iniVar::set_prev(NM_iniVar * var)
{
    this->prev = var;
}

void NM_iniVar::add_comment(string comment)
{
    if( comment.compare("") == 0 )
        return;
    if( this->comment.length() > 0 )
        this->comment += "\n;";

    this->comment += comment;
}

NM_iniVar * NM_iniVar::get_next()
{
    return this->next;
}

NM_iniVar * NM_iniVar::get_prev()
{
    return this->prev;
}

string NM_iniVar::get_name()
{
    return this->name;
}

string NM_iniVar::get_value()
{
    return this->value;
}

string NM_iniVar::get_comment()
{
    return this->comment;
}

void NM_iniVar::change_comment(string comment)
{
    this->comment = comment;
}

void NM_iniVar::remove_comment()
{
    this->comment = "";
}
