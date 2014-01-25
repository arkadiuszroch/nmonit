#include "iniSectionList.hpp"

using namespace std;

NM_iniSectionList::NM_iniSectionList()
{
    this->first = NULL;
    this->current = NULL;
    this->last = NULL;
    this->root = NULL;
}

NM_iniSectionList::~NM_iniSectionList()
{
    NM_iniSection * buf;
    while( this->first )
    {
        buf = this->first->get_next();
        delete this->first;
        this->first = buf;
    }
}

NM_iniSection * NM_iniSectionList::find_section(string name)
{
    if( name.compare("") == 0 )
    {
        return this->root;
    }

    this->current = this->first;

    while( this->current )
    {
        if( name.compare(this->current->get_name()) == 0 )
            return this->current;

        this->current = this->current->get_next();
    }
    return NULL;
}

NM_iniSection * NM_iniSectionList::get_root_section()
{
    return this->root;
}

NM_iniSection * NM_iniSectionList::get_first()
{
    return this->first;
}

NM_iniSection * NM_iniSectionList::get_current()
{
    return this->current;
}

NM_iniSection * NM_iniSectionList::get_last()
{
    return this->last;
}

void NM_iniSectionList::add_root_section(NM_iniSection * root_section)
{
    if( this->root != NULL )
    {
        this->root = root_section;
    }
    else
    {
        throw NM_exception(NM_ex_code_ini_root_section_exists, "The root section already exists");
    }
}

void NM_iniSectionList::add_section(NM_iniSection * section)
{
    if( this->find_section(section->get_name()) == NULL )
    {
        if( this->first == NULL )
        {
            this->first = section;
            this->current = section;
        }

        if( this->last != NULL )
        {
            this->last->set_next(section);
            section->set_prev(this->last);
        }
        this->last = section;
    }
    else
    {
        throw NM_exception(NM_ex_code_ini_section_exists, "The section with that name already exists ( " + section->get_name() + " )");
    }
}

void NM_iniSectionList::add_comment_to_section(string comment, string section_name)
{
    NM_iniSection * section = NULL;

    section = this->find_section(section_name);
    if( section != NULL )
    {
        section->add_comment(comment);
    }
    else
    {
        throw NM_exception(NM_ex_code_ini_section_doesnt_exists, "The section with that name doesn't exists ( " + section_name + " )");
    }
}

void NM_iniSectionList::add_var(NM_iniVar * var, string section_name, bool create_section)
{
    NM_iniSection * section = NULL;
    section = this->find_section(section_name);

    if( section == NULL )
    {
        if( create_section )
        {
            section = new NM_iniSection(section_name);
        }
        else
        {
            throw NM_exception(NM_ex_code_ini_section_doesnt_exists, "The section with that name doesn't exists ( " + section_name + " )");
        }
    }

    if( section->find_var(var->get_name()) == NULL )
    {
        section->add_var(var);
    }
    else
    {
        throw NM_exception(NM_ex_code_ini_var_exists, "The var with that name already exists ( " + var->get_name() + " )");
    }
}

void NM_iniSectionList::add_comment_to_var(string comment, string var_name, string section_name)
{
    NM_iniSection * section = NULL;

    section = this->find_section(section_name);
    if( section != NULL )
    {
        section->add_comment_to_var(comment, var_name);
    }
    else
    {
        throw NM_exception(NM_ex_code_ini_section_doesnt_exists, "The section with that name doesn't exists ( " + section_name + " )");
    }
}

void NM_iniSectionList::change_root_section(NM_iniSection * section)
{
    if( this->root != NULL )
    {
        delete this->root;
    }

    this->root = section;
}

void NM_iniSectionList::change_section(NM_iniSection * section)
{
    NM_iniSection * old_section = NULL;


    if( section->get_name().compare("") == 0 )
    {
        this->change_root_section(section);
        return;
    }

    old_section = this->find_section(section->get_name());

    if( old_section == NULL )
    {
        this->add_section(section);
    }
    else
    {
        NM_iniSection * buf = NULL;
        // Set to new section the prev and next sections from old section
        section->set_next(old_section->get_next());
        section->set_prev(old_section->get_prev());

        // Set to prev and next section the new section 
        buf = section->get_next();
        buf->set_prev(section);
        buf = NULL;
        buf = section->get_prev();
        buf->set_next(section);
        buf = NULL;

        if( this->first == old_section )
            this->first = section;
        if( this->last == old_section )
            this->last = section;
        if( this->current = old_section )
            this->current = this->first;

        old_section->set_next(NULL);
        old_section->set_prev(NULL);

        delete old_section;
    }
}

void NM_iniSectionList::change_comment_in_section(string comment, string section_name)
{
    NM_iniSection * section = NULL;
    section = this->find_section(section_name);
    if( section != NULL )
    {
        section->change_comment(comment);
    }
    else
    {
        throw NM_exception(NM_ex_code_ini_section_doesnt_exists, "The section with that name doesn't exists ( " + section_name + " )");
    }
}

void NM_iniSectionList::change_var(NM_iniVar * var, string section_name)
{
    NM_iniSection * section = this->find_section(section_name);

    if( section == NULL )
        throw NM_exception(NM_ex_code_ini_section_doesnt_exists, "The section with that name doesn't exists ( " + section_name + " )");

    section->change_var(var);
}

void NM_iniSectionList::change_comment_in_var(string comment, string var_name, string section_name)
{
    NM_iniSection * section = this->find_section(section_name);

    if( section == NULL )
    {
        throw NM_exception(NM_ex_code_ini_section_doesnt_exists, "The section with that name doesn't exists ( " + section_name + " )");
    }

    section->change_comment_in_var(comment, var_name);
}

void NM_iniSectionList::remove_root_section()
{
    if( this->root != NULL )
        delete this->root;
}

void NM_iniSectionList::remove_section(string section_name)
{
    NM_iniSection * section = NULL;
    section = this->find_section(section_name);
    if( section != NULL )
        delete section;
}

void NM_iniSectionList::remove_comment_from_section(string section_name)
{
    NM_iniSection * section = NULL;
    section = this->find_section(section_name);

    if( section != NULL )
        section->remove_comment();
}

void NM_iniSectionList::remove_var(string var_name, string section_name)
{
    NM_iniSection * section = NULL;

    if( section != NULL )
        section->remove_var(var_name);
}

void NM_iniSectionList::remove_comment_from_var(string var_name, string section_name)
{
    NM_iniSection * section = NULL;

    if( section != NULL )
        section->remove_comment_from_var(var_name);
}
