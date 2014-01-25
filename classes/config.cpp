#include "config.hpp"

using namespace std;

NM_config * NM_config::config_instance = NULL;

NM_config::NM_config()
{
    this->debug_sections_all = true;
    
    this->programs_permanent_check = false;
    this->programs_allowed_check = false;
    this->programs_ignored_check = false;
            
    this->interfaces_permanent_check = false;
    this->interfaces_allowed_check = false;
    this->interfaces_ignored_check = false;
            
    this->users_permanent_check = false;
    this->users_allowed_check = false;
    this->users_ignored_check = false;
}

NM_config * NM_config::get_config()
{
    if( NM_config::config_instance == NULL )
    {
        NM_config::config_instance = new NM_config();
    }

    return NM_config::config_instance;
}

void NM_config::set_flag(string flag_name, bool val)
{
    this->flags[flag_name] = val;
}

bool NM_config::get_flag(string flag_name)
{
    if( this->flags.find(flag_name) == this->flags.end() )
        throw NM_exception(NM_ex_code_flag_doesnt_exists, "Flag: " + flag_name + " doesn't exists");
    return this->flags[flag_name];
}

void NM_config::set_monitored_section(string section_name)
{
    if( section_name.compare("ALL") != 0 )
    {
        this->debug_sections_all = false;
        if( find(this->debug_sections.begin(), this->debug_sections.end(), section_name) == this->debug_sections.end() )
        {
            this->debug_sections.push_back(section_name);
        }
    }
    else
        this->debug_sections_all = true;
}

bool NM_config::check_monitored_section(string section_name)
{
    if( this->debug_sections_all == true )
        return true;

    if( find(this->debug_sections.begin(), this->debug_sections.end(), section_name) == this->debug_sections.end() )
    {
        return false;
    }

    return true;
}

void NM_config::set_programs_permanent(NM_string_list program_list)
{
    if( program_list.size() == 0 )
        return;

    this->programs_permanent = program_list;
    this->programs_permanent_check = true;
}

void NM_config::set_programs_allowed(NM_string_list program_list)
{
    if( program_list.size() == 0 )
        return;

    this->programs_allowed = program_list;
    this->programs_allowed_check = true;
}

void NM_config::set_programs_ignored(NM_string_list program_list)
{
    if( program_list.size() == 0 )
        return;

    this->programs_ignored = program_list;
    this->programs_ignored_check = true;
}

NM_monitored_state NM_config::check_program(string program_name)
{
    if(this->programs_permanent_check)
    {
        if( find(this->programs_permanent.begin(), this->programs_permanent.end(), program_name) != this->programs_permanent.end() )
            return NM_monitored_permanent;
    }
    
    if( this->programs_allowed_check )
    {
        if( find(this->programs_allowed.begin(), this->programs_allowed.end(), program_name) != this->programs_allowed.end() )
            return NM_monitored;
        else
            return NM_not_monitored;
    }

    if( this->programs_ignored_check )
    {
        if( find(this->programs_ignored.begin(), this->programs_ignored.end(), program_name) != this->programs_ignored.end() )
            return NM_not_monitored;
        else
            return NM_monitored;
    }

    return NM_monitored;
}

void NM_config::set_interfaces_permanent(NM_string_list interface_list)
{
    if( interface_list.size() == 0 )
        return;

    this->interfaces_permanent = interface_list;
    this->interfaces_permanent_check = true;
}

void NM_config::set_interfaces_allowed(NM_string_list interface_list)
{
    if( interface_list.size() == 0 )
        return;

    this->interfaces_allowed = interface_list;
    this->interfaces_allowed_check = true;
}

void NM_config::set_interfaces_ignored(NM_string_list interface_list)
{
    if( interface_list.size() == 0 )
        return;

    this->interfaces_ignored = interface_list;
    this->interfaces_ignored_check = true;
}

NM_monitored_state NM_config::check_interface(string interface_name)
{
    if( this->interfaces_permanent_check )
    {
        if( find(this->interfaces_permanent.begin(), this->interfaces_permanent.end(), interface_name) != this->interfaces_permanent.end() )
            return NM_monitored_permanent;
    }
    
    if( this->interfaces_allowed_check )
    {
        if( find(this->interfaces_allowed.begin(), this->interfaces_allowed.end(), interface_name) != this->interfaces_allowed.end() )
            return NM_monitored;
        else
            return NM_not_monitored;
    }

    if( this->interfaces_ignored_check )
    {
        if( find(this->interfaces_ignored.begin(), this->interfaces_ignored.end(), interface_name) != this->interfaces_ignored.end() )
            return NM_not_monitored;
        else
            return NM_monitored;
    }

    return NM_monitored;
}

void NM_config::set_users_permanent(NM_string_list user_list)
{
    if( user_list.size() == 0 )
        return;

    this->users_permanent = user_list;
    this->users_permanent_check = true;
}

void NM_config::set_users_allowed(NM_string_list user_list)
{
    if( user_list.size() == 0 )
        return;

    this->users_allowed = user_list;
    this->users_allowed_check = true;
}

void NM_config::set_users_ignored(NM_string_list user_list)
{
    if( user_list.size() == 0 )
        return;

    this->users_ignored = user_list;
    this->users_ignored_check = true;
}

NM_monitored_state NM_config::check_user(string user_name)
{
    if( this->users_permanent_check )
    {
        if( find(this->users_permanent.begin(), this->users_permanent.end(), user_name) != this->users_permanent.end() )
            return NM_monitored_permanent;
    }
    
    if( this->users_allowed_check )
    {
        if( find(this->users_allowed.begin(), this->users_allowed.end(), user_name) != this->users_allowed.end() )
            return NM_monitored;
        else
            return NM_not_monitored;
    }

    if( this->users_ignored_check )
    {
        if( find(this->users_ignored.begin(), this->users_ignored.end(), user_name) != this->users_ignored.end() )
            return NM_not_monitored;
        else
            return NM_monitored;
    }

    return NM_monitored;
}

