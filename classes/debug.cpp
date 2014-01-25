#include "debug.hpp"

using namespace std;

NM_stream NM_debug::out = NM_stream();
bool NM_debug::initialized = false;
NM_debug_list NM_debug::instances;// = NM_debug::initialize_instances();

NM_debug::NM_debug(string section_name)
{
    if(!NM_debug::initialized)
    {
        NM_debug::out.set_stream(NM_stream_file, (NM_check_installed()) ? ::NM_nmonit_dir + ".debug" : "./.debug");
        NM_debug::initialized = true;
    }
    
    NM_config * config = NM_config::get_config();
    this->section_name = section_name;

    this->active = (config->check_monitored_section(section_name) && config->get_flag(NM_FLAG_DEBUG));
}
/*
NM_debug_list NM_debug::initialize_instances()
{
    NM_debug_list instance;
    
    return instance;
}
*/
NM_debug::~NM_debug()
{
    NM_debug_list::iterator it;

    it = this->instances.find(this->section_name);

    if( it != this->instances.end() )
    {
        if( it->second != NULL )
        {
            delete it->second;
            this->instances.erase(it);
        }
    }
}

NM_debug * NM_debug::get_instance(string section_name)
{
    if( NM_debug::instances.find(section_name) == NM_debug::instances.end() )
    {
        NM_debug::instances[section_name] = new NM_debug(section_name);
    }

    return NM_debug::instances[section_name];
}

string NM_debug::prepare_out(string prefix)
{
    return string("[ " + this->section_name + "::" + prefix + " ]");
}

void NM_debug::info(string msg)
{
    if( this->active )
        NM_debug::out << this->prepare_out("STRING") << msg << endl;
}

void NM_debug::info(const char * msg)
{
    if( this->active )
        NM_debug::out << this->prepare_out("CHAR") << msg << endl;
}

void NM_debug::info(int val)
{
    if( this->active )
        NM_debug::out << this->prepare_out("INT") << val << endl;
}

void NM_debug::info(double val)
{
    if( this->active )
        NM_debug::out << this->prepare_out("DOUBLE") << val << endl;
}

void NM_debug::info(float val)
{
    if( this->active )
        NM_debug::out << this->prepare_out("FLOAT") << val << endl;
}

void NM_debug::info(bool val)
{
    if( this->active )
        NM_debug::out << this->prepare_out("BOOL") << val << endl;
}

void NM_debug::warning(string msg)
{
    if( this->active )
        NM_debug::out << this->prepare_out("STRING") << msg << endl;
}

void NM_debug::warning(const char * msg)
{
    if( this->active )
        NM_debug::out << this->prepare_out("CHAR") << msg << endl;
}

void NM_debug::warning(int val)
{
    if( this->active )
        NM_debug::out << this->prepare_out("INT") << val << endl;
}

void NM_debug::warning(double val)
{
    if( this->active )
        NM_debug::out << this->prepare_out("DOUBLE") << val << endl;
}

void NM_debug::warning(float val)
{
    if( this->active )
        NM_debug::out << this->prepare_out("FLOAT") << val << endl;
}

void NM_debug::warning(bool val)
{
    if( this->active )
        NM_debug::out << this->prepare_out("BOOL") << val << endl;
}

