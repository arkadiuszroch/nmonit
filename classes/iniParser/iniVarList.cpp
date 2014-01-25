#include "iniVarList.hpp"

using namespace std;

NM_iniVarList::NM_iniVarList()
{
    this->first = NULL;
    this->current = NULL;
    this->last = NULL;
}

NM_iniVarList::~NM_iniVarList()
{
    NM_iniVar * buf;
    while( this->first )
    {
        buf = this->first->get_next();
        delete this->first;
        this->first = buf;
    }
}

NM_iniVar * NM_iniVarList::get_first()
{
    return this->first;
}

NM_iniVar * NM_iniVarList::get_current()
{
    return this->current;
}

NM_iniVar * NM_iniVarList::get_last()
{
    return this->last;
}

NM_iniVar * NM_iniVarList::find_var(string name)
{
    this->current = this->first;
    while( this->current )
    {
        if( name.compare(this->current->get_name()) == 0 )
            return this->current;

        this->current = this->current->get_next();
    }
}

void NM_iniVarList::add_var(NM_iniVar * var)
{
    if( this->find_var(var->get_name()) == NULL )
    {
        if( this->first == NULL )
        {
            this->first = var;
            this->current = var;
        }

        if( this->last != NULL )
        {
            this->last->set_next(var);
            var->set_prev(this->last);
        }

        this->last = var;
    }
    else
    {
        throw NM_exception(NM_ex_code_ini_var_exists, "The variable with that name already exists ( " + var->get_name() + " )");
    }
}

void NM_iniVarList::add_comment_to_var(string comment, string var_name)
{
    NM_iniVar * var = NULL;

    var = this->find_var(var_name);
    if( var != NULL )
    {
        var->add_comment(comment);
    }
    else
    {
        throw NM_exception(NM_ex_code_ini_var_doesnt_exists, "The variable with that name doesn't exists ( " + var_name + " )");
    }
}

string NM_iniVarList::get_var_comment(string var_name)
{
    NM_iniVar * var = this->find_var(var_name);
    if( var == NULL )
    {
        throw NM_exception(NM_ex_code_ini_var_doesnt_exists, "The variable with that name doesn't exists ( " + var_name + " )");
    }
    else
    {
        return var->get_comment();
    }
}

void NM_iniVarList::change_var(NM_iniVar * var)
{
    NM_iniVar * old_var = NULL;

    old_var = this->find_var(var->get_name());

    if( old_var == NULL )
    {
        this->add_var(var);
    }
    else
    {
        NM_iniVar * buf = NULL;
        // Set to new var the prev and next var from old var
        var->set_next(old_var->get_next());
        var->set_prev(old_var->get_prev());

        // Set to prev and next var the new var 
        buf = var->get_next();
        buf->set_prev(var);
        buf = NULL;
        buf = var->get_prev();
        buf->set_next(var);
        buf = NULL;

        if( this->first == old_var )
            this->first = var;
        if( this->last == old_var )
            this->last = var;
        if( this->current = old_var )
            this->current = this->first;

        old_var->set_next(NULL);
        old_var->set_prev(NULL);

        delete old_var;
    }
}

void NM_iniVarList::change_comment_in_var(string comment, string var_name)
{
    NM_iniVar * var = NULL;

    var = this->find_var(var_name);
    if( var != NULL )
    {
        var->change_comment(comment);
    }
    else
    {
        throw NM_exception(NM_ex_code_ini_var_doesnt_exists, "The variable with that name doesn't exists ( " + var_name + " )");
    }
}

void NM_iniVarList::remove_var(string var_name)
{
    NM_iniVar * var = NULL;

    var = this->find_var(var_name);
    if( var != NULL )
        delete var;
}

void NM_iniVarList::remove_comment_from_var(string var_name)
{
    NM_iniVar * var = NULL;

    var = this->find_var(var_name);
    if( var != NULL )
        var->remove_comment();
}

string NM_iniVarList::get_var_value(string var_name)
{
    NM_iniVar * var = NULL;

    var = this->find_var(var_name);
    if( var != NULL )
    {
        return var->get_value();
    }
    else
    {
        throw NM_exception(NM_ex_code_ini_var_doesnt_exists, "The variable with that name doesn't exists ( " + var_name + " )");
    }
}
