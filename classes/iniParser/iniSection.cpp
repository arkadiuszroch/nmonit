#include "iniSection.hpp"

using namespace std;

NM_iniSection::NM_iniSection(string name, NM_iniVarList * var_list, string comment)
{
    this->next = NULL;
    this->prev = NULL;
    this->child = (var_list == NULL) ? new NM_iniVarList : var_list;

    this->name = name;

    this->comment = "";
    this->add_comment(comment);
}

NM_iniSection::~NM_iniSection()
{
    if( this->next != NULL )
        this->next->set_prev(this->prev);

    if( this->prev != NULL )
        this->prev->set_next(this->next);

    delete this->child;
}

void NM_iniSection::set_next(NM_iniSection * next)
{
    this->next = next;
}

void NM_iniSection::set_prev(NM_iniSection * prev)
{
    this->prev = prev;
}

NM_iniSection * NM_iniSection::get_next()
{
    return this->next;
}

NM_iniSection * NM_iniSection::get_prev()
{
    return this->prev;
}

NM_iniVarList * NM_iniSection::get_child()
{
    return this->child;
}

string NM_iniSection::get_name()
{
    return this->name;
}

void NM_iniSection::add_comment(string comment)
{
    if( comment.compare("") == 0 )
        return;
    if( this->comment.length() > 0 )
        this->comment += "\n;";

    this->comment += comment;
}

string NM_iniSection::get_comment()
{
    return this->comment;
}

string NM_iniSection::get_var_comment(string var_name)
{
    return this->child->get_var_comment(var_name);
}

void NM_iniSection::add_var(NM_iniVar * var)
{
    this->child->add_var(var);
}

void NM_iniSection::add_var(string var_name, string var_value, string comment)
{
    this->add_var(new NM_iniVar(var_name, var_value, comment));
}

NM_iniVar * NM_iniSection::find_var(string var_name)
{
    return this->child->find_var(var_name);
}

void NM_iniSection::add_comment_to_var(string comment, string var_name)
{
    this->child->add_comment_to_var(comment, var_name);
}

void NM_iniSection::change_comment(string comment)
{
    this->comment = comment;
}

void NM_iniSection::change_var(NM_iniVar * var)
{
    this->child->change_var(var);
}

void NM_iniSection::change_comment_in_var(string comment, string var_name)
{
    this->child->change_comment_in_var(comment, var_name);
}

void NM_iniSection::remove_comment()
{
    this->comment = "";
}

void NM_iniSection::remove_var(string var_name)
{
    this->child->remove_var(var_name);
}

void NM_iniSection::remove_comment_from_var(string var_name)
{
    this->child->remove_comment_from_var(var_name);
}

string NM_iniSection::get_var_value(string var_name)
{
    return this->child->get_var_value(var_name);
}
