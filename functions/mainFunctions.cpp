#include "mainFunctions.hpp"


using namespace std;

void NM_display_help()
{
    cout << "How to use:" << endl;
    cout << "  Parameters:" << endl;
    cout << "    " << NM_bold("-c") << endl;
    cout << "\t- create default config file but not overwrite if file already exists" << endl;
    cout << "    " << NM_bold("-ci") << endl;
    cout << "\t- create default config file or overwrite if file already exists" << endl;
    cout << "    " << NM_bold("/etc/init.d/nmonit start") << endl;
    cout << "\t - start daemon" << endl;
    cout << "    " << NM_bold("/etc/init.d/nmonit stop") << endl;
    cout << "\t - stop daemon" << endl;
    cout << "    " << NM_bold("/etc/init.d/nmonit restart") << endl;
    cout << "\t - restart daemon" << endl;
    cout << "    " << NM_bold("update.rc-d nmonit enable") << endl;
    cout << "\t - enable daemon" << endl;
    cout << "    " << NM_bold("update.rc-d nmonit disable") << endl;
    cout << "\t - disable daemon" << endl;
    cout << "    " << NM_bold("-s / --preview") << endl;
    cout << "\t - show current network acitvity" << endl;
    cout << "    " << NM_bold("-v / --version") << endl;
    cout << "\t - display version" << endl;
    cout << "    " << NM_bold("-h / --help") << endl;
    cout << "\t - display help" << endl;
    cout << "    " << NM_bold("-debug-console") << endl;
    cout << "\t - enable debug mode and write data to console" << endl;
    cout << "    " << NM_bold("-debug-console <section_names>") << endl;
    cout << "\t - enable debug for sections mode and write data to console" << endl << endl;
    
    NM_create_config_help();
}

void NM_create_config_help()
{
    cout << "How to use create default config:" << endl;
    cout << "  Parameters:" << endl;
    cout << "    " << NM_bold("-a / --all") << endl;
    cout << "\t - create default configs for all options" << endl;
    cout << "    " << NM_bold("-d / --daemon") << endl;
    cout << "\t - create default config for daemon" << endl;
    cout << "    " << NM_bold("-s / --preview") << endl;
    cout << "\t - create default config for monitor" << endl;
    cout << "    " << NM_bold("-l / --local") << endl;
    cout << "\t - create default config in current working directory" << endl;
    cout << "    " << NM_bold("-p / --presenter") << endl;
    cout << "\t - create default config for webservice" << endl;
}


void NM_load_config()
{
    NM_config * conf = NM_config::get_config();
    NM_iniParser * ini_config = NULL; 
    string bufor;
    bool delete_tmp = false;
    
    if(NM_check_installed())
    {
        // sprawdzamy czy istnieje plik w /etc/nmonit
        if(file_exists(::NM_config_filename) == false)
        {
            // jesli nie istnieje sprawdzamy czy istnieje w /usr/lib/nmonit
            if(file_exists(::NM_nmonit_dir + ::NM_config_name))
            {
                ::NM_config_filename = ::NM_nmonit_dir + ::NM_config_name;
            }
            else
            {
                // jeśli nie istnieje sprawdzamy czy istnieje w katalogu roboczym
                if(file_exists(NM_config_name))
                {
                    ::NM_config_filename = "./" + ::NM_config_name;
                }
                else
                {
                    // jesli nie ma tworzymy plik tymczasowy z domyślną konfiguracją
                    ::NM_config_filename = ::NM_tmp_dir + ::NM_config_name;
                    NM_create_default_config(::NM_config_filename);
                    delete_tmp = true;
                    NM_trigger_warning("Configuration file was not found. Use default settings");
                    sleep(2);
                }
            }
        }
    }
    else
    {
        if(file_exists(::NM_config_name))
        {
            ::NM_config_filename = "./" + ::NM_config_name;
        }
        else
        {
            ::NM_config_filename = ::NM_tmp_dir + ::NM_config_name;
            NM_create_default_config(::NM_config_filename);
            delete_tmp = true;
            NM_trigger_warning("Configuration file was not found. Use default settings");
            sleep(2);
        }
    }
        
    ini_config = new NM_iniParser(::NM_config_filename);
    try
    {
        ini_config->read_from_ini_file();
    }
    catch( NM_exception ex )
    {
        ostringstream sstream;
        sstream << "There was an error while reading configuration file. Details: " << endl << ex.get_message();
        NM_trigger_error(sstream.str());
    }

    try
    {
        bufor = ini_config->get_var_value("programs", "logging_settings");
    }
    catch( NM_exception ex )
    {
        ostringstream sstream;
        sstream << "There was an error while reading configuration file. Details: " << endl << ex.get_message();
        NM_trigger_error(sstream.str());
    }

    if( bufor.substr(0, 3).compare("ALL") == 0 )
    {
        size_t pos;
        if( (pos = bufor.find("~")) != string::npos )
        {
            conf->set_programs_ignored(explode_to_vector(",", bufor.substr(pos + 1)));
        }
    }
    else if( bufor.substr(0, 4).compare("NONE") == 0 )
    {
        size_t pos;
        if( (pos = bufor.find("~")) != string::npos )
        {
            conf->set_programs_allowed(explode_to_vector(",", bufor.substr(pos + 1)));
        }
        else
        {
            NM_trigger_error("No programs will be monitored. Check your configuration file");
        }
    }
    else
    {
        conf->set_programs_allowed(explode_to_vector(",", bufor));
    }

    try
    {
        bufor = ini_config->get_var_value("programs_permanent", "logging_settings");
    }
    catch( NM_exception ex )
    {
        ostringstream sstream;
        sstream << "There was an error while reading configuration file. Details: " << endl << ex.get_message();
        NM_trigger_error(sstream.str());
    }

    conf->set_programs_permanent(explode_to_vector(",", bufor));

    try
    {
        bufor = ini_config->get_var_value("interfaces", "logging_settings");
    }
    catch( NM_exception ex )
    {
        ostringstream sstream;
        sstream << "There was an error while reading configuration file. Details: " << endl << ex.get_message();
        NM_trigger_error(sstream.str());
    }

    if( bufor.substr(0, 3).compare("ALL") == 0 )
    {
        size_t pos;
        if( (pos = bufor.find("~")) != string::npos )
        {
            conf->set_interfaces_ignored(explode_to_vector(",", bufor.substr(pos + 1)));
        }
    }
    else if( bufor.substr(0, 4).compare("NONE") == 0 )
    {
        size_t pos;
        if( (pos = bufor.find("~")) != string::npos )
        {
            conf->set_interfaces_allowed(explode_to_vector(",", bufor.substr(pos + 1)));
        }
        else
        {
            NM_trigger_error("No interfaces will be monitored. Will you want to disable saving data about interfaces?");
        }
    }
    else
    {
        conf->set_interfaces_allowed(explode_to_vector(",", bufor));
    }

    try
    {
        bufor = ini_config->get_var_value("interfaces_permanent", "logging_settings");
    }
    catch( NM_exception ex )
    {
        ostringstream sstream;
        sstream << "There was an error while reading configuration file. Details: " << endl << ex.get_message();
        NM_trigger_error(sstream.str());
    }

    conf->set_interfaces_permanent(explode_to_vector(",", bufor));

    try
    {
        bufor = ini_config->get_var_value("users", "logging_settings");
    }
    catch( NM_exception ex )
    {
        ostringstream sstream;
        sstream << "There was an error while reading configuration file. Details: " << endl << ex.get_message();
        NM_trigger_error(sstream.str());
    }

    if( bufor.substr(0, 3).compare("ALL") == 0 )
    {
        size_t pos;
        if( (pos = bufor.find("~")) != string::npos )
        {
            conf->set_users_ignored(explode_to_vector(",", bufor.substr(pos + 1)));
        }
    }
    else if( bufor.substr(0, 4).compare("NONE") == 0 )
    {
        size_t pos;
        if( (pos = bufor.find("~")) != string::npos )
        {
            conf->set_users_allowed(explode_to_vector(",", bufor.substr(pos + 1)));
        }
        else
        {
            NM_trigger_error("No users will be monitored. Will you want to disable saving data about users?");
        }
    }
    else
    {
        conf->set_users_allowed(explode_to_vector(",", bufor));
    }

    try
    {
        bufor = ini_config->get_var_value("users_permanent", "logging_settings");
    }
    catch( NM_exception ex )
    {
        ostringstream sstream;
        sstream << "There was an error while reading configuration file. Details: " << endl << ex.get_message();
        NM_trigger_error(sstream.str());
    }

    conf->set_users_permanent(explode_to_vector(",", bufor));
    
    if(delete_tmp)
        unlink(::NM_config_filename.c_str());
}

void NM_create_default_config(string file_name)
{
    bool createConfig = false;
    bool ignore = false;
    
    try
    {
        ignore = NM_config::get_config()->get_flag(NM_FLAG_CREATE_CONFIG_IGNORE);
    }
    catch( NM_exception e )
    {
        NM_trigger_error(e.get_message());
    }
    
    fstream file(file_name.c_str(), ios::in);

    if( !file.good() || (file.good() && ignore) )
        createConfig = true;

    file.close();
    if( createConfig )
    {
        NM_iniParser * ini = new NM_iniParser(file_name);

        NM_iniSection * section = NULL;
        NM_iniVar * var = NULL;

        ini->add_comment_to_file("NMonit v1.0.0");
        ini->add_comment_to_file("NMonit settings file");
        ini->add_comment_to_file("Author: Arkadiusz Roch");
        ini->add_comment_to_file("Project website: http://www.nmonit.aroch.pl");
        ini->add_comment_to_file("Author website: http://www.aroch.pl");
        ini->add_comment_to_file("Don't change anything in this file if you don't know what are you doing");
        ini->add_comment_to_file("If you know what are you doign, you do it on your own responsibility");
        ini->add_comment_to_file("Good luck ;)");
        ini->add_comment_to_file("In FAQ you can find more information about use this settings");

        section = new NM_iniSection("logging_settings", NULL, "Determine what programs, network interfaces and users should be monitored for network activity");

        ini->add_section(section);

        var = new NM_iniVar("programs", "ALL", "Determine what programs should be monitored for network activity");
        section->add_var(var);
        
        var = new NM_iniVar("programs_permanent", "", "Determine what programs must be monitored for network activity");
        section->add_var(var);

        var = new NM_iniVar("users", "ALL", "Determine what users should be monitored for network activity");
        section->add_var(var);

        var = new NM_iniVar("users_permanent", "", "Determine what users must be monitored for network activity");
        section->add_var(var);

        var = new NM_iniVar("interfaces", "ALL~lo", "Determine what network interfaces should be monitored for network activity");
        section->add_var(var);
        
        var = new NM_iniVar("interfaces_permanent", "", "Determine what network interfaces must be monitored for network activity");
        section->add_var(var);
        
        ini->create_ini_file();

        NM_trigger_notice("Configuration file was created successfully");
    }
    else
    {
        NM_iniParser * ini = new NM_iniParser(::NM_config_filename);

        try
        {
            ini->read_from_ini_file();
        }
        catch( NM_exception e )
        {
            NM_trigger_error(e.get_message());
        }

        NM_trigger_warning("Configuration file already exists. Use " + string(NM_PARAM_CREATE_CONFIG_IGNORE) + " parameter to overwrite confiruration file", true);
    }
}

void NM_create_default_webservice_config(string file_name)
{
    bool createConfig = false;
    bool ignore = false;
    
    try
    {
        ignore = NM_config::get_config()->get_flag(NM_FLAG_CREATE_CONFIG_IGNORE);
    }
    catch( NM_exception e )
    {
        NM_trigger_error(e.get_message());
    }
    
    fstream file(file_name.c_str(), ios::in);

    if( !file.good() || (file.good() && ignore) )
        createConfig = true;
    else if(file.good() && !ignore)
        NM_trigger_warning("Configuration file already exists. Use " + string(NM_PARAM_CREATE_CONFIG_IGNORE) + " parameter to overwrite confiruration file", true);

    file.close();
    if( createConfig )
    {
        fstream file(file_name.c_str(), ios::out | ios::trunc);
        file << "LISTEN 9595" << endl;
        file << endl;
        file << "<VirtualHost *:9595>" << endl;
	file << "\tServerAdmin webmaster@localhost" << endl;
	file << "\tDocumentRoot /usr/lib/nmonit/web/public" << endl;
	//file << "\t#DirectoryIndex index.php" << endl;
        file << endl;
	file << "\tLogLevel warn" << endl;
	file << "\tErrorLog  /usr/lib/nmonit/web/error.log" << endl;
	file << "\tCustomLog /usr/lib/nmonit/web/custom.log combined" << endl;
        file << endl;
	file << "\t<Directory />" << endl;
	file << "\t\tOptions Indexes FollowSymLinks MultiViews" << endl;
	file << "\t\tAllowOverride All" << endl;
	file << "\t\tOrder allow,deny" << endl;
	file << "\t\tallow from all" << endl;
	file << "\t</Directory>" << endl;
        file << "</VirtualHost>" << endl;
        
        file.close();
        
        NM_trigger_notice("Configuration file was created successfully");
    }
}

void NM_load_objects()
{
    if(NM_config::get_config()->get_flag(NM_FLAG_SHOW_NET_USAGE))
    {
        NM_remap_devices();
        return;
    }
    NM_debug * debug = NM_debug::get_instance("NM_load_objects");
    debug->info("Start loading objects");

    NM_database * db = NM_database::get_instance();
    
    NM_2D_string_list objects = db->query("SELECT * FROM objects");
    
    for(NM_uint i = 0; i < objects.size(); i++)
    {
        NM_string_list row = objects[i];
        switch(atoi(row[2].c_str()))
        {
            case NM_object_program : 
            {
                NM_program * program = new NM_program(row[3], atoi(row[0].c_str()), atoi(row[1].c_str()));
                program->set_monitored(NM_config::get_config()->check_program(row[2].c_str()));
                NM_programList::get_instance()->add_program(program);
            }
            break;
            case NM_object_user : 
            {
                NM_user * user = new NM_user(atoi(row[4].c_str()), row[3], atoi(row[0].c_str()), atoi(row[1].c_str()));
                user->set_monitored(NM_config::get_config()->check_user(row[2]));
                NM_userList::get_instance()->add_user(user);
            }
            break;
            case NM_object_device : 
            {
                string info, address, address6;
                info = row[4];
                address = info.substr(0, info.find(";"));
                address6 = info.substr(info.find(";") + 1);
                NM_device * device = new NM_device(row[3], ip_to_in_addr(address), ip_to_in6_addr(address6), atoi(row[0].c_str()), atoi(row[1].c_str()));
                device->set_monitored(NM_config::get_config()->check_interface(row[3]));
                debug->info("Device name is: " + device->get_device_name());
                
                NM_deviceList * device_list = NM_deviceList::get_instance();
                NM_device * dev_buf = device_list->find_by_name(device->get_device_name());

                debug->info("Checking weather device with that name exists");
                if( dev_buf != NULL )
                {
                    debug->info("Device with that name exists so we delete it");
                    device_list->delete_device(dev_buf);
                    dev_buf = NULL;
                }

                debug->info("Add new device");
                device_list->add_device(device);

                device->set_monitored(NM_config::get_config()->check_interface(device->get_device_name()));
            }
            break;
            case NM_object_remote : 
            {
                string info, address, address6;
                info = row[4];
                address = info.substr(0, info.find(";"));
                address6 = info.substr(info.find(";") + 1);
                NM_remote * remote = new NM_remote(row[3], ip_to_in_addr(address), ip_to_in6_addr(address6), atoi(row[0].c_str()), atoi(row[1].c_str()));
                remote->set_monitored(NM_monitored);
                debug->info("Remote name is: " + remote->get_remote_name());
                
                NM_remoteList * remote_list = NM_remoteList::get_instance();
                NM_remote * remote_buf = remote_list->find_by_name(remote->get_remote_name());

                debug->info("Checking weather device with that name exists");
                if( remote_buf != NULL )
                {
                    debug->info("Device with that name exists so we delete it");
                    remote_list->delete_remote(remote_buf);
                    remote_buf = NULL;
                }

                debug->info("Add new remote");
                remote_list->add_remote(remote);

                remote->set_monitored(NM_monitored);
            }
            break;
        }
    }
    NM_remap_devices();
}

void NM_remap_devices()
{
    NM_debug * debug = NM_debug::get_instance("NM_remap_devices");
    debug->info("Start remapping devices");
    if( pthread_mutex_trylock(&NM_mutex_remap_devices) != 0 )
    {
        while( pthread_mutex_trylock(&NM_mutex_remap_devices) != 0 );
        pthread_mutex_unlock(&NM_mutex_remap_devices);
        return;
    }

    struct ifaddrs * if_addr_struct = NULL;
    struct ifaddrs * ifa = NULL;

    map<string /*name*/, string /*address*/> device_list_buf;

    getifaddrs(&if_addr_struct);

    for(ifa = if_addr_struct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if( ifa ->ifa_addr->sa_family == AF_INET )
        {
            struct sockaddr_in * sock = (struct sockaddr_in *) ifa->ifa_addr;
            in_addr address = sock->sin_addr;
            string name = ifa->ifa_name;
            if( device_list_buf.count(name) > 0 )
                NM_add_device(name, address, ip_to_in6_addr(device_list_buf[name]));
            else
            {
                string saddress = ip_to_string(address);
                device_list_buf[name] = saddress;
            }
        }
        else if( ifa->ifa_addr->sa_family == AF_INET6 )
        {
            struct sockaddr_in6 * sock = (struct sockaddr_in6 *) ifa->ifa_addr;
            in6_addr address = sock->sin6_addr;
            string name = ifa->ifa_name;
            if( device_list_buf.count(name) > 0 )
                NM_add_device(name, ip_to_in_addr(device_list_buf[name]), address);
            else
            {
                string saddress = ip_to_string(address);
                device_list_buf[name] = saddress;
            }
        }
    }

    if( if_addr_struct != NULL )
        freeifaddrs(if_addr_struct);

    pthread_mutex_unlock(&NM_mutex_remap_devices);
}

void NM_add_device(string name, in_addr address, in6_addr address6)
{
    NM_deviceList * devices = NM_deviceList::get_instance();
    NM_device * device = devices->find_by_name(name);

    if( device == NULL )
    {
        device = new NM_device(name, address, address6);
        devices->add_device(device);
        NM_save_object(device);
    }
    else if( device->get_device_ip_in_addr().s_addr != address.s_addr || device->get_device_ip6().compare(ip_to_string(address6)) != 0 )
    {
        NM_id master_id = device->get_master_id();
        devices->delete_device(device);
        device = NULL; // dla bezpieczeństwa;
        device = new NM_device(name, address, address6, 0, master_id);
        devices->add_device(device);
        NM_save_object(device);
    }

    device->set_monitored(NM_config::get_config()->check_interface(name));
}

NM_remote * NM_add_remote(in_addr address)
{
    pthread_mutex_lock(&::NM_mutex_remap_remotes);
    NM_remoteList * remotes = NM_remoteList::get_instance();
    NM_remote * remote = remotes->find_by_ip(address);

    if( remote != NULL )
    {
        pthread_mutex_unlock(&::NM_mutex_remap_remotes);
        return remote;
    }
    else
    {
        struct hostent * info = NULL;
        info = gethostbyaddr(&address, sizeof (address), AF_INET);
        if( info != NULL )
        {
            in6_addr address6 = ip_to_in6_addr(NM_ip6_def_addr);
            string name(info->h_name);
            remote = remotes->find_by_name(name);
            NM_id master_id = 0;
            if( remote != NULL )
            {
                master_id = remote->get_master_id();
                address6 = remote->get_remote_ip_in6_addr();
                remotes->delete_remote(remote);
                remote = NULL;
            }
            remote = new NM_remote(name, address, address6, 0, master_id);

        }
        else
            remote = new NM_remote("name_unavailable(" + ip_to_string(address) + ")", address);

        remote->set_monitored(NM_monitored);
        remotes->add_remote(remote);
        NM_save_object(remote);
        pthread_mutex_unlock(&::NM_mutex_remap_remotes);
        return remote;
    }
}

string prepare_date(bool with_seconds)
{
    time_t t = time(0); // get time now
    struct tm * now = localtime(& t);
    
    ostringstream date;

    date << now->tm_hour << ":" << now->tm_min << ":";
    
    if(with_seconds)
        date << now->tm_sec;
    else
        date << "00";
    
    date << " " << now->tm_mday << "-" << (now->tm_mon + 1) << "-" << (now->tm_year + 1900);
    
    return date.str();
}

void NM_save_object(NM_device * device)
{
    if( NM_config::get_config()->get_flag(NM_FLAG_SHOW_NET_USAGE) )
        return;
    
    NM_database * db = NM_database::get_instance();
    
    ostringstream query;
    
    //NM_info_pc_objects object = NM_object_device;
    NM_id id = device->get_device_id(), master_id = device->get_master_id();
    //struct in_addr address = device->get_device_ip_in_addr();
    string name = device->get_device_name(), address = device->get_device_ip(), address6 = device->get_device_ip6();
    NM_uint object = static_cast<NM_uint> (NM_object_device);

    
    query << "INSERT INTO objects VALUES(" << id << ", " << master_id << ", " << object << ", '" << name << "', '" << address << ";" << address6 << "', '" << prepare_date() << "');";
    
    try{
        db->query(query.str());
    } catch(NM_exception e)
    {
        NM_trigger_error("Database error: can not save new device");
    }
}

void NM_save_object(NM_remote * remote)
{
    if( NM_config::get_config()->get_flag(NM_FLAG_SHOW_NET_USAGE) )
        return;
    
    NM_database * db = NM_database::get_instance();
    
    ostringstream query;
    
    //NM_info_pc_objects object = NM_object_remote;
    NM_id id = remote->get_remote_id(), master_id = remote->get_master_id();
    //struct in_addr address = device->get_device_ip_in_addr();
    string name = remote->get_remote_name(), address = remote->get_remote_ip(), address6 = remote->get_remote_ip6();
    NM_uint object = static_cast<NM_uint> (NM_object_remote);
    
    query << "INSERT INTO objects VALUES(" << id << ", " << master_id << ", " << object << ", '" << name << "', '" << address << ";" << address6 << "', '" << prepare_date() << "');";
    
    try{
        db->query(query.str());
    } catch(NM_exception e)
    {
        NM_trigger_error("Database error: can not save new device");
    }
}

void NM_save_object(NM_program * program)
{
    if( NM_config::get_config()->get_flag(NM_FLAG_SHOW_NET_USAGE) )
        return;
    
    NM_database * db = NM_database::get_instance();
    
    ostringstream query;
    
    //NM_info_pc_objects object = NM_object_program;
    NM_id id = program->get_program_id(), master_id = program->get_master_id();
    string name = program->get_program_name();
    NM_uint object = static_cast<NM_uint> (NM_object_program);

    query << "INSERT INTO objects VALUES(" << id << ", " << master_id << ", " << object << ", '" << name << "', '', '" << prepare_date() << "');";
    
    try{
        db->query(query.str());
    } catch(NM_exception e)
    {
        NM_trigger_error("Database error: can not save new device");
    }
}

void NM_save_object(NM_user * user)
{
    if( NM_config::get_config()->get_flag(NM_FLAG_SHOW_NET_USAGE) )
        return;
    
    NM_database * db = NM_database::get_instance();
    
    ostringstream query;
    
    //NM_info_pc_objects object = NM_object_user;
    NM_id id = user->get_user_id(), master_id = user->get_master_id(), sys_id = user->get_user_system_id();
    string name = user->get_user_name();
    NM_uint object = static_cast<NM_uint> (NM_object_user);

    query << "INSERT INTO objects VALUES(" << id << ", " << master_id << ", " << object << ", '" << name << "', '" << sys_id << "', '" << prepare_date() << "');";
    
    try{
        db->query(query.str());
    } catch(NM_exception e)
    {
        NM_trigger_error("Database error: can not save new device");
    }
}

void NM_remap_inodes()
{
    if( pthread_mutex_trylock(&NM_mutex_remap_inodes) != 0 )
    {
        while( pthread_mutex_trylock(&NM_mutex_remap_inodes) != 0 );
        pthread_mutex_unlock(&NM_mutex_remap_inodes);
        return;
    }

    if( ::NM_inodes.count(0) == 0 )
    {
        NM_inodes[0].device = NULL;
        NM_inodes[0].program = NULL;
        NM_inodes[0].user = NULL;
        NM_inodes[0].remote = NULL;
    }

    DIR * proc = opendir("/proc");
    dirent * entry = NULL;

    if( proc == 0 )
    {
        throw NM_exception(NM_ex_code_inodes_remap_proc_dir_err, "Error while opening /proc dir");
    }

    while( (entry = readdir(proc)) )
    {
        char * name = NULL;
        bool do_continue = false;
        DIR * fd = NULL;
        dirent * fd_entry = NULL;
        ostringstream sstream;
        NM_user * user = NULL;
        NM_program * program = NULL;

        if( entry->d_type != DT_DIR )
        {
            continue;
        }

        name = entry->d_name;
        while( *name )
        {
            if( !isdigit(*name) )
            {
                name++;
                do_continue = true;
                break;
            }
            name++;
        }

        if( do_continue )
            continue;

        sstream << "/proc/" << entry->d_name << "/fd";

        fd = opendir(sstream.str().c_str());

        if( fd == 0 )
        {
            NM_trigger_warning("Error while opening " + sstream.str());
            continue;
        }

        while( (fd_entry = readdir(fd)) )
        {
            int link_length = 50;
            char link[link_length];
            char * inode_ptr = NULL;
            int inode = 0;
            string slink;
            sstream.str("");
            sstream << "/proc/" << entry->d_name << "/fd/" << fd_entry->d_name;

            readlink(sstream.str().c_str(), link, link_length - 1);
            slink = link;

            if( slink.compare(0, 8, "socket:[") != 0 )
                continue;

            inode_ptr = link + 8;

            while( *inode_ptr )
            {
                if( *inode_ptr == ']' )
                    break;
                inode *= 10;
                inode += (*inode_ptr - '0');
                inode_ptr++;
            }

            if( ::NM_inodes.count(inode) == 0 )
            {
                struct NM_inode bufor;

                if( program == NULL || user == NULL )
                {

                    fstream file;
                    bool name_get = false, user_get = false;

                    if( user != NULL )
                        user_get = true;
                    if( program != NULL )
                        name_get = true;

                    sstream.str("");
                    sstream << "/proc/" << entry->d_name << "/status";
                    file.open(sstream.str().c_str(), ios::in);
                    if( !file.good() )
                    {
                        NM_trigger_warning("Couldn't open " + sstream.str());
                        continue;
                    }

                    do
                    {
                        string program_name, line;
                        int user_id = 0;
                        getline(file, line);

                        if( (name_get && user_get) || file.eof() )
                        {
                            break;
                        }

                        if( program == NULL )
                        {
                            if( line.substr(0, 5).compare("Name:") == 0 )
                            {
                                NM_programList * program_list = NM_programList::get_instance();
                                program_name = line.substr(6);

                                program = program_list->find_by_name(program_name);
                                if( program == NULL )
                                {
                                    program = new NM_program(program_name);
                                    program->set_monitored(NM_config::get_config()->check_program(program_name));
                                    NM_programList::get_instance()->add_program(program);
                                    NM_save_object(program);
                                }

                                name_get = true;
                                continue;
                            }
                        }

                        if( user == NULL )
                        {
                            if( line.substr(0, 4).compare("Uid:") == 0 )
                            {
                                size_t pos = line.length() - 1;
                                struct passwd * user_info = NULL;
                                istringstream int_buf;
                                NM_userList * user_list = NM_userList::get_instance();

                                pos = line.find_last_of("	");
                                int_buf.str(line.substr(pos));
                                int_buf >> user_id;

                                user = user_list->find_by_system_id(user_id);
                                if( user == NULL )
                                {
                                    user_info = getpwuid(user_id);
                                    if( user_info == NULL )
                                    {
                                        sstream.str("");
                                        sstream << "Error while getting info abuto user with id: " << user_id;
                                        NM_trigger_warning(sstream.str());
                                    }
                                    user = new NM_user(user_id, user_info->pw_name);
                                    user->set_monitored(NM_config::get_config()->check_user(user_info->pw_name));
                                    NM_userList::get_instance()->add_user(user);
                                    NM_save_object(user);
                                }

                                user_get = true;
                                continue;
                            }
                        }
                    }
                    while( 1 );
                }

                bufor.user = user;
                bufor.program = program;
                bufor.device = NULL;
                bufor.remote = NULL;
                ::NM_inodes[inode] = bufor;
            }
        }
        closedir(fd);
    }

    closedir(proc);

    pthread_mutex_unlock(&NM_mutex_remap_inodes);
}

void NM_show_net_usage(ostringstream & out)
{
    NM_debug * debug = NM_debug::get_instance("NM_show_net_usage");
    debug->info("Starting calculating net usage");
    NM_net_usage_list net_usage;

    debug->info("Starting loop for calculating usage");
    for(NM_connection_list::iterator it = ::NM_connections_v4.begin(); it != ::NM_connections_v4.end(); it++)
    {
        string program_name;
        NM_uint program_id;

        debug->info("Finding program");

        if( it->second->get_inode()->first > 0 )
        {
            program_name = it->second->get_inode()->second.program->get_program_name();
            program_id = it->second->get_inode()->second.program->get_program_id();
        }
        else
        {
            program_name = "unknown";
            program_id = 0;
            /*NM_out << it->first << endl;
            NM_out << it->second->get_inode()->first << endl;
            NM_exit(0);*/
        }

        if(!it->second->is_monitored() || (it->second->get_rx_size() == 0 && it->second->get_tx_size() == 0))
            continue;
        
        debug->info("Checkig that program: " + program_name + " exists in our list");
        if( net_usage.count(program_id) == 0 )
        {
            debug->info("Program doesn't exists so we add it");
            struct program_net_usage info;

            debug->info("Getting info about device and user");

            info.program_name = program_name;

            if( it->second->get_inode()->first > 0 )
            {
                info.device_name = it->second->get_inode()->second.device->get_device_name();
                info.device_id = it->second->get_inode()->second.device->get_device_id();
                info.user_name = it->second->get_inode()->second.user->get_user_name();
                info.user_id = it->second->get_inode()->second.user->get_user_id();
            }
            else
            {
                info.device_name = "unknown";
                info.device_id = 0;
                info.user_name = "unknown";
                info.user_id = 0;
            }
            debug->info("Found device: " + info.device_name);

            debug->info("Found user: " + info.user_name);

            debug->info("Setting size to 0b");
            info.tx_size = info.rx_size = 0.0;
            info.tx_size_type = info.rx_size_type = NM_b;
            net_usage[program_id] = info;
        }

        debug->info("Checking that adding tx size is more than 0");
        // calculating tx
        if( it->second->get_tx_size() > 0.0 )
        {
            debug->info("Tx size is more than 0 so we add this size");
            NM_calculate_net_size(net_usage[program_id].tx_size, net_usage[program_id].tx_size_type, it->second->get_tx_size(), it->second->get_tx_size_type());
        }
        debug->info("checking that adding rx size is more than 0");
        // calculating rx
        if( it->second->get_rx_size() > 0.0 )
        {
            debug->info("Rx size is more than 0 so we add this size");
            NM_calculate_net_size(net_usage[program_id].rx_size, net_usage[program_id].rx_size_type, it->second->get_rx_size(), it->second->get_rx_size_type());
        }

        debug->info("Standarizing size type to upload and download");

        if( net_usage[program_id].rx_size_type != net_usage[program_id].tx_size_type )
        {
            NM_double & rx = net_usage[program_id].rx_size,
                    & tx = net_usage[program_id].tx_size;
            NM_size_type & rx_type = net_usage[program_id].rx_size_type,
                    & tx_type = net_usage[program_id].tx_size_type;

            if( rx_type < tx_type )
            {
                unsigned short int p = tx_type - rx_type;

                rx /= pow(1024, p);
                rx_type = tx_type;
            }
            else
            {
                unsigned short int p = rx_type - tx_type;

                tx /= pow(1024, p);
                tx_type = rx_type;
            }

            it->second->reset_size();
        }
    }

    debug->info("Checking program running mode");
    
    if( NM_config::get_config()->get_flag(NM_FLAG_SHOW_NET_USAGE) )
    {
        debug->info("Program running in monitoring mode");

        debug->info("Get instance of UI");
        NM_ui * ui = NM_ui::get_instance();

        for(NM_net_usage_list::iterator it = net_usage.begin(); it != net_usage.end(); it++)
        {
            if( it->second.rx_size > 0 || it->second.tx_size > 0 )
                ui->add(it->second.program_name, it->second.user_name, it->second.tx_size, it->second.rx_size, it->second.rx_size_type);
        }

        debug->info("Data added to GUI");
    }
}

void NM_write_net_usage()
{
    NM_debug * debug = NM_debug::get_instance("NM_write_net_usage");
    debug->info("Start write image");
    if( NM_config::get_config()->get_flag(NM_FLAG_SHOW_NET_USAGE) )
        return;
    debug->info("Initalize vars");
    ostringstream query;
    NM_database * db = NM_database::get_instance();
    
    time_t rawtime;
    time (&rawtime);
    
    time_t current_time = mktime(localtime(&rawtime));
    
    NM_write_usage_info buf_list;
    
    debug->info("Start loop");
    for(NM_connection_list::iterator it = ::NM_connections_v4.begin(); it != ::NM_connections_v4.end(); it++)
    {
        NM_id program_id, user_id, device_id, remote_id, program_master_id, user_master_id, device_master_id, remote_master_id;
        NM_uint rx_size_type, tx_size_type;
        NM_double rx_size, tx_size;
        ostringstream stream_key;
        string key;
        
        if(!it->second->is_monitored() || (it->second->get_rx_size() == 0 && it->second->get_tx_size() == 0))
            continue;
        
        debug->info("Checking inode");
        if( it->second->has_inode() && it->second->get_inode()->first > 0 )
        {
            debug->info("Connection have inode");
            debug->info("Getting program id");
            program_id = it->second->get_inode()->second.program->get_program_id();
            program_master_id = it->second->get_inode()->second.program->get_master_id();
            debug->info("Getting user id");
            user_id = it->second->get_inode()->second.user->get_user_id();
            user_master_id = it->second->get_inode()->second.user->get_master_id();
            debug->info("Getting device id");
            device_id = it->second->get_inode()->second.device->get_device_id();
            device_master_id = it->second->get_inode()->second.device->get_master_id();
            debug->info("Getting remote id");
            if( it->second->get_inode()->second.remote != NULL )
            {
                remote_id = it->second->get_inode()->second.remote->get_remote_id();
                remote_master_id = it->second->get_inode()->second.remote->get_master_id();
            }
            else
            {
                remote_id = 0;
                remote_master_id = 0;
            }
        }
        else
        {
            NM_remote * remote = it->second->get_remote();
            debug->info("Connection do not have inode");
            program_id = 0;
            program_master_id = 0;
            user_id = 0;
            user_master_id = 0;
            device_id = 0;
            device_master_id = 0;
            
            if(remote == NULL)
            {
                remote_id = 0;
                remote_master_id = 0;
            }
            else
            {
                remote_id = remote->get_remote_id();
                remote_master_id = remote->get_master_id();
            }
        }

        debug->info("Write data to file");
        
        rx_size = it->second->get_rx_size();
        rx_size_type = static_cast<NM_uint> (it->second->get_rx_size_type());
        rx_size = rx_size * pow(1024, rx_size_type - 1);
        
        tx_size = it->second->get_tx_size();
        tx_size_type = static_cast<NM_uint> (it->second->get_tx_size_type());
        tx_size = tx_size * pow(1024, tx_size_type - 1);
        
        stream_key << program_id << ";" << user_id << ";" << device_id << ";" << remote_id;
        key = stream_key.str();
        if(buf_list.find(key) == buf_list.end())
        {
            buf_list[key].program_id = program_id;
            buf_list[key].program_master_id = program_master_id;
            buf_list[key].user_id = user_id;
            buf_list[key].user_master_id = user_master_id;
            buf_list[key].device_id = device_id;
            buf_list[key].device_master_id = device_master_id;
            buf_list[key].remote_id = remote_id;
            buf_list[key].remote_master_id = remote_master_id;
            
            buf_list[key].rx_size = 0;
            buf_list[key].tx_size = 0;
        }
        
        buf_list[key].rx_size += round(rx_size, 2);
        buf_list[key].tx_size += round(tx_size, 2);
        
    }
    
    NM_config * config = NM_config::get_config();
    if(config->get_flag(NM_FLAG_SIGNAL_SEND))
        return;
    
    config->set_flag(NM_FLAG_WRITE_DATA, true);
    
    query.str("");
    query << "BEGIN TRANSACTION; ";
    NM_uint i = 1;
    for(NM_write_usage_info::iterator it = buf_list.begin(); it != buf_list.end(); it++)
    {
        query << "INSERT INTO net_usage VALUES ( '" << current_time << "', " 
                << it->second.program_id << ", " << it->second.user_id << ", " 
                << it->second.device_id << ", " << it->second.remote_id << ", '" 
                << it->second.rx_size << "', '" << it->second.tx_size << "');";
        
        // @TODO dopisać dodawanie danych do tabeli objects summary
        if((i % 30) == 0)
        {
            i = 1;
            query << "END TRANSACTION;";
            bool success = false;
            NM_uint j = 10;
            do
            {
                try
                {
                    db->query(query.str());
                    success = true;
                }catch(NM_exception e)
                {
                    //if(db->get_error_code() != 6)
                       // throw e;
                    success = false;
                    usleep(500);
                }

                // drop data;
                if(--j == 0)
                    break;
        
            }while(!success);
        }
    }
    query << "END TRANSACTION;";
    buf_list.clear();
    
    bool success = false;
    i = 10;
    
    do
    {
        try
        {
            db->query(query.str());
            success = true;
        }catch(NM_exception e)
        {
            //if(db->get_error_code() != 6)
               // throw e;
            success = false;
            usleep(500);
        }
        
        // drop data;
        if(--i == 0)
            break;
        
    }while(!success);
    
    config->set_flag(NM_FLAG_WRITE_DATA, false);
}

void NM_calculate_net_size(NM_double & actual_size, NM_size_type & actual_size_type, NM_double adding_size, NM_size_type adding_size_type)
{
    NM_debug * debug = NM_debug::get_instance("NM_calculate_net_size");
    debug->info("Start adding size");
    if( actual_size_type == adding_size_type )
    {
        debug->info("If adding size is the same to actual size we simply add size");
        actual_size += adding_size;
    }
    else
    {
        debug->info("Adding size is difrent from actual size");
        NM_double * greater = NULL, * smaller = NULL;
        int p = 0;
        if( actual_size_type > adding_size_type )
        {
            debug->info("actual size type is higher than adding size type");
            p = actual_size_type - adding_size_type;
            greater = &actual_size;
            smaller = &adding_size;
        }
        else
        {
            debug->info("adding size type is higher than actual size type");
            p = adding_size_type - actual_size_type;
            // ponieważ niżej będziemy konwertować rozmiar do większego typu

            actual_size_type = adding_size_type;
            greater = &adding_size;
            smaller = &actual_size;
        }

        debug->info("calculating smaller size to higher size type");
        *smaller /= pow(1024, p);
        actual_size = *greater + *smaller;


        debug->info("calculating size per second");
        // size per second
        actual_size /= NM_refresh_time;

        debug->info("clearing pointers");
        // clearing pointers
        greater = smaller = NULL;
    }

    debug->info("loop to standarizing size");
    while( actual_size > 1024.00 && actual_size_type < NM_tb )
    {
        actual_size /= 1024.00;
        switch( actual_size_type )
        {
            case NM_gb:
                actual_size_type = NM_tb;
                break;
            case NM_mb:
                actual_size_type = NM_gb;
                break;
            case NM_kb:
                actual_size_type = NM_mb;
                break;
            case NM_b:
                actual_size_type = NM_kb;
                break;
        }
    }

    debug->info("second loop to standarizing size");
    while( actual_size < 1.00 && actual_size_type > NM_b )
    {
        actual_size *= 1024.00;
        switch( actual_size_type )
        {
            case NM_tb:
                actual_size_type = NM_gb;
                break;
            case NM_gb:
                actual_size_type = NM_mb;
                break;
            case NM_mb:
                actual_size_type = NM_kb;
                break;
            case NM_kb:
                actual_size_type = NM_b;
                break;
        }
    }

    debug->info("Rounding size");
    actual_size = (round(actual_size * 100)) / 100;
    debug->info("exit adding size functions");
}

string NM_size_type_to_string(NM_size_type type)
{
    switch( type )
    {
        case NM_b:
            return "b";
        case NM_kb:
            return "kb";
        case NM_mb:
            return "mb";
        case NM_gb:
            return "gb";
        case NM_tb:
            return "tb";
        default:
            return "unknown type";
    }
}