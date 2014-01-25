#include "nmonit.hpp"

using namespace std;

int main(int argc, char * argv[])
{
    try //global try for ungaught exceptions
    {
        NM_debug * debug = NULL;
        NM_config * config = NM_config::get_config();
        int reset_max = 1;
        NM_out.set_stream(NM_stream_cout);
        
        unsigned short int reset = reset_max;
        // Sprawdzenie czy mamy jakies parametry. Jezeli nie ma lub parametry to -h  lub -help wyswietlamy help
        if( argc == 1 || string(NM_PARAM_HELP).compare(argv[1]) == 0 || string(NM_PARAM_HELP_EXT).compare(argv[1]) == 0 )
        {
            NM_display_help();
            return 0;
        }
        
        if( string(NM_PARAM_VERSION).compare(argv[1]) == 0 || string(NM_PARAM_VERSION_EXT).compare(argv[1]) == 0 )
        {
            cout << "NMonit version: " << NM_VERSION << endl;
            return 0;
        }
        
        if( string(NM_PARAM_DAEMON).compare(argv[1]) == 0 )
        {
            NM_out.set_stream(NM_stream_cout);
            if(argc >= 3)
            {
                if(string(NM_PARAM_DAEMON_DISABLE).compare(argv[2]))
                {
                    system("update-rc.d nmonit defaults");
                    NM_trigger_notice("NMonit was added to autostart");
                    NM_exit(0);
                }
                else if(string(NM_PARAM_DAEMON_DISABLE).compare(argv[2]))
                {
                    system("update-rc.d nmonit remove");
                    NM_trigger_notice("NMonit was removed from autostart");
                    NM_exit(0);
                }
            }
            NM_trigger_error("Unable to recognice option. Allowed options is 'enable' or 'disable'");
        }
        
        config->set_flag(NM_FLAG_DAEMON, false);
        config->set_flag(NM_FLAG_DEBUG, false);
        config->set_flag(NM_FLAG_DEBUG_CONSOLE, false);
        config->set_flag(NM_FLAG_SHOW_NET_USAGE, false);
        config->set_flag(NM_FLAG_SIGNAL_SEND, false);
        config->set_flag(NM_FLAG_WRITE_DATA, false);

        NM_check_root();

        // Sprawdzenie czy parametry odpowiadaja za utworzenie domyslnej konfiguracji
        if( string(NM_PARAM_CREATE_CONFIG).compare(argv[1]) == 0 || string(NM_PARAM_CREATE_CONFIG_IGNORE).compare(argv[1]) == 0 )
        {
            config->set_flag(NM_FLAG_CREATE_CONFIG, string(NM_PARAM_CREATE_CONFIG).compare(argv[1]) == 0);
            config->set_flag(NM_FLAG_CREATE_CONFIG_IGNORE, string(NM_PARAM_CREATE_CONFIG_IGNORE).compare(argv[1]) == 0);
            
            if(argc <= 2)
            {
                NM_create_config_help();
                return 0;
            }
            
            if(!NM_check_installed() && !(string(NM_PARAM_CREATE_CONFIG_LOCAL).compare(argv[2]) == 0 || string(NM_PARAM_CREATE_CONFIG_LOCAL_EXT).compare(argv[2]) == 0 ))
            {
                NM_trigger_warning("NMonit is not installed. Please install NMonit first to use this option or use -l / --local flag to create local configuration file", true);
            }
            
            if(string(NM_PARAM_CREATE_CONFIG_LOCAL).compare(argv[2]) == 0 || string(NM_PARAM_CREATE_CONFIG_LOCAL_EXT).compare(argv[2]) == 0 )
            {
                NM_create_default_config("./" + ::NM_config_name);
            }
            else if(string(NM_PARAM_CREATE_CONFIG_ALL).compare(argv[2]) == 0 || string(NM_PARAM_CREATE_CONFIG_ALL_EXT).compare(argv[2]) == 0)
            {
                NM_create_default_config(NM_daemon_config_filename);
                NM_create_default_config(NM_monitor_config_filename);
                NM_create_default_webservice_config(NM_webservice_config_filename);
            }
            else if(string(NM_PARAM_CREATE_CONFIG_DAEMON).compare(argv[2]) == 0 || string(NM_PARAM_CREATE_CONFIG_DAEMON_EXT).compare(argv[2]) == 0)
                NM_create_default_config(NM_daemon_config_filename);
            else if(string(NM_PARAM_CREATE_CONFIG_MONITOR).compare(argv[2]) == 0 || string(NM_PARAM_CREATE_CONFIG_MONITOR_EXT).compare(argv[2]) == 0)
                NM_create_default_config(NM_monitor_config_filename);
            else if(string(NM_PARAM_CREATE_CONFIG_WEBSERVICE).compare(argv[2]) == 0 || string(NM_PARAM_CREATE_CONFIG_WEBSERVICE_EXT).compare(argv[2]) == 0)
                NM_create_default_webservice_config(NM_webservice_config_filename);
            else
            {
                NM_create_config_help();
                return 0;
            }
            
            return 0;
        }

        if( string(NM_PARAM_START_DAEMON).compare(argv[1]) == 0 )
        {
            if(!NM_check_installed())
            {
                bool godmode = false;
                if(argc > 2)
                    if(string("-godmode").compare(argv[2]) == 0)
                        godmode = true;
                if(!godmode)
                NM_trigger_error("NMonit is not installed. Please install NMonit first.");
            }
            config->set_flag(NM_FLAG_DAEMON, true);
            
            ::NM_refresh_time = 60;
            NM_config_filename = NM_daemon_config_filename;

        }
        else if( string(NM_PARAM_SHOW_NETWORK_USAGE).compare(argv[1]) == 0 || string(NM_PARAM_SHOW_NETWORK_USAGE_EXT).compare(argv[1]) == 0 )
        {
            config->set_flag(NM_FLAG_SHOW_NET_USAGE, true);
            
            ::NM_pid_filename = ".nmonit-show.pid";
            reset_max = 1;
            
            NM_config_filename = NM_monitor_config_filename;
        }
        else
        {
            NM_display_help();
            return 0;
        }

        if( argc > 2 )
        {
            for(int i = 2; i < argc; i++)
            {
                if( string(NM_PARAM_DEBUG).compare(argv[i]) == 0 )
                {
                    config->set_flag(NM_FLAG_DEBUG, true);
                    config->set_monitored_section("ALL");
                }
                else if( string(NM_PARAM_DEBUG_CONSOLE).compare(argv[i]) == 0 )
                {
                    if(config->get_flag(NM_FLAG_SHOW_NET_USAGE))
                    {
                        config->set_flag(NM_FLAG_DEBUG_CONSOLE, false);
                        config->set_flag(NM_FLAG_DEBUG, false);
                        NM_trigger_warning("Console debug mode is unavailable in this option. Disable debugging");
                        sleep(5);
                    }
                    else
                    {
                        config->set_flag(NM_FLAG_DEBUG_CONSOLE, true);
                        config->set_flag(NM_FLAG_DEBUG, true);
                    }
                }
                else
                {
                    if( config->get_flag(NM_FLAG_DEBUG) )
                    {
                        config->set_monitored_section(argv[i]);
                    }
                }
            } // for(i = 2; i < argc; i++)

        } // if argc > 2

        debug = NM_debug::get_instance("main");
        debug->info("Initalize");
        NM_initialize_env();

        debug->info("Load config");
        // zaladowanie pliku konfiguracyjnego
        NM_load_config();

        debug->info("Load objects");
        // Wczytanie intersejsów sieciowych, uzytkownikow oraz programów
        NM_load_objects();
        //NM_out << NM_deviceList::get_instance()->find_by_name("wlan0") << endl;

        if( NM_deviceList::get_instance()->get_first() == NULL )
            NM_trigger_error("No interfaces found on the system");

        debug->info("Start main loop");
        // uruchomienie wątku łapiącego pakiety dla każdego z interfejsów sieciowych
        while( true )
        {
            debug->info("Load first device");
            // wczytanie pierwszego interfejsu sieciowego
            NM_device * device = NM_deviceList::get_instance()->get_first();
            ostringstream out;
            /*NM_deviceList * dl = NM_deviceList::get_instance();
            
            NM_out << dl << endl;
             */
            if( device != NULL )
                debug->info("First device is: " + device->get_device_name());

            while( device )
            {
                int device_id = device->get_device_id();
                debug->info("Actual device is: " + device->get_device_name());
                // sprawdzenie czy informacje mają być zbierane z danego interfejsu sieciowego
                debug->info("Checking wheater device is monitored and thread for it is running");
                if( device->is_monitored() )
                {
                    if( ::NM_threads[device_id].state == NM_thread_state_stopped )
                    {
                        pthread_t thread_id;
                        pthread_attr_t attr;

                        if( ::NM_threads[device_id].state == NM_thread_state_stopped )
                        {
                            debug->info("Thread for  " + device->get_device_name() + " device is stopped");
                            ::NM_threads[device_id].thread = static_cast<pthread_t> (0);
                        }

                        pthread_attr_init(&attr);
                        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

                        pthread_create(&thread_id, &attr, NM_capture_packets, device);
                        ::NM_threads[device_id].thread = thread_id;
                        ::NM_threads[device_id].state = NM_thread_state_running;
                        debug->info("start new thread for device: " + device->get_device_name());
                    }
                    else
                    {
                        debug->info("Thread for device " + device->get_device_name() + " is already running");
                    }
                } // if device->is_monitored()
                else
                {
                    debug->info("Device " + device->get_device_name() + " is not monitored");
                }

                device = device->get_next();
            } //while( device )

            if( ::NM_threads.size() < 1 )
            {
                ::NM_out.set_stream(NM_stream_cout);
            
                NM_trigger_warning("No interfaces will be monitored", true);
            }
            if(config->get_flag(NM_FLAG_SHOW_NET_USAGE))
            {
                ::NM_refresh_time = 1;
            }
            else
            {
                NM_optimize_data();
            
                time_t t = time(0); // get time now
                struct tm * now = NULL;
                now = localtime(&t);
                if(now != NULL)
                    ::NM_refresh_time = 60 - now->tm_sec; //time to refresh
            }
            sleep(::NM_refresh_time);

            pthread_mutex_lock(&NM_mutex_conn_list);
            pthread_mutex_lock(&NM_mutex_remap_inodes);
            if( config->get_flag(NM_FLAG_SHOW_NET_USAGE) )
            {
                NM_show_net_usage(out);
                NM_ui::get_instance()->tick();
            }
            else if( config->get_flag(NM_FLAG_DAEMON) )
            {
                NM_write_net_usage();
            }
            
            pthread_mutex_unlock(&NM_mutex_remap_inodes);

            debug->info("Actual reset is: ");
            debug->info((int) reset);
            debug->info("Preparing to delete connection list");

            for(NM_connection_list::iterator it = ::NM_connections_v4.begin(); it != ::NM_connections_v4.end(); it++)
            {
                NM_connection * buf = NULL;

                buf = it->second;
                debug->info("Checking to delete connection with key: " + it->first);
                if( buf->decrement_counter() == 0 || reset < 1 )
                {
                    debug->info("Delete connection with key: " + it->first);
                    delete buf;
                    ::NM_connections_v4.erase(it);
                }
            }

            debug->info("connections cleaned");

            if( reset < 1 )
            {
                pthread_mutex_lock(&NM_mutex_remap_inodes);
                debug->info("cleaning inodes");
                ::NM_inodes.clear();
                pthread_mutex_unlock(&NM_mutex_remap_inodes);
                debug->info("unlock mutexes");
            }


            pthread_mutex_unlock(&NM_mutex_conn_list);
            debug->info("unlock connections");
            cout << out.str();

            if( reset < 1 )
                reset = reset_max + 1;
            reset--;
        } // while( true )

    } // try
    catch( NM_exception e )
    {
        cout << e.get_message() << endl;
        NM_trigger_error(e.get_message());
    }
    catch( NM_exception * e )
    {
        cout << e->get_message() << endl;
        NM_trigger_error(e->get_message());
    }
    catch( length_error & le )
    {
        NM_trigger_error(le.what());
    }
    return 0;
}

void NM_initialize_env()
{
    NM_debug * debug = NM_debug::get_instance("NM_initialize_env");
    NM_config * config = NM_config::get_config();
    fstream file;
    bool create_file = false;
    
    if(NM_check_installed())
        NM_out.set_stream(NM_stream_file, NM_nmonit_dir + NM_log_name, false);
    else
        NM_out.set_stream(NM_stream_file, NM_log_name, false);
        
    debug->info("Initialize signal handler");
    
    void (*prev_fn)(int);

    prev_fn = signal(SIGSEGV, NM_signal_handler);
    if (prev_fn==SIG_IGN) 
        signal (SIGSEGV,SIG_IGN);
    
    prev_fn = signal(SIGINT, NM_signal_handler);
    if (prev_fn==SIG_IGN) 
        signal (SIGINT,SIG_IGN);
    
    debug->info("Initialize pid file");
    file.open(NM_pid_filename.c_str(), ios::in);

    debug->info("Checking pid file");
    if( file.is_open() )
    {
        debug->info("Pid file exists");
        string line;
        int pid;
        fstream pid_file;
        ostringstream sstream;
        getline(file, line);

        pid = atoi(line.c_str());
        debug->info("Old pid: " + line);
        sstream << "/proc/" << pid << "/comm";

        pid_file.open(sstream.str().c_str(), ios::in);

        debug->info("Checking process comm file");
        if( pid_file.is_open() )
        {
            debug->info("Process comm file exists");
            string comm;

            getline(pid_file, comm);
            debug->info("Process name is: " + comm);
            if( comm.compare("nmonit") == 0 )
                NM_trigger_error("NMonit daemon already running");
            else
                create_file = true;

            pid_file.close();
        }

        else
        {
            create_file = true;
        }

        file.close();
    }
    else
    {
        create_file = true;
    }

    if( config->get_flag(NM_FLAG_SHOW_NET_USAGE) )
    {
        ::NM_refresh_time = 1;
        NM_ui::get_instance();
    }
    else if(config->get_flag(NM_FLAG_DAEMON))
    {
        //initialize database
        try{
            NM_database::get_instance();
        }
        catch(NM_exception e)
        {
            NM_trigger_error("Error while initializing database: " + e.get_message());
        }
    }
    

    debug->info("Cheking create_file flag");
    if( create_file )
    {
        debug->info("Creating new pid file");
        file.open(::NM_pid_filename.c_str(), ios::out | ios::trunc);
        if( !file.is_open() )
            NM_trigger_error("Can not open nmonit.pid file");
        file << getpid();
        file.close();
    }

    debug->info("Initalize time");
    time_t t = time(0); // get time now
    struct tm * now = localtime(&t);

    ::NM_current_date.year = now->tm_year;
    ::NM_current_date.month = now->tm_mon;
    ::NM_current_date.day = now->tm_mday;
    ::NM_current_date.hour = now->tm_hour;
    ::NM_current_date.minute = now->tm_min;
    ::NM_current_date.second = now->tm_sec;

    debug->info("Initalize mutexes");
    if( pthread_mutex_init(&::NM_mutex_conn_list, NULL) != 0 )
        NM_trigger_error("Couldn't initialize connection list mutex");

    if( pthread_mutex_init(&::NM_mutex_remap_inodes, NULL) != 0 )
        NM_trigger_error("Couldn't initialize inodes mutex");

    if( pthread_mutex_init(&::NM_mutex_remap_devices, NULL) != 0 )
        NM_trigger_error("Couldn't initialize devices mutex");
    
    if( pthread_mutex_init(&::NM_mutex_remap_remotes, NULL) != 0 )
        NM_trigger_error("Couldn't initialize remotes mutex");
    
    if( pthread_mutex_init(&::NM_mutex_converter, NULL) != 0 )
        NM_trigger_error("Couldn't initialize converter mutex");
}

void NM_check_root()
{
    if( getuid() != 0 )
    {
        cout << "-------------------------------------" << endl;
        cout << "| You need to be root to run NMonit |" << endl;
        cout << "-------------------------------------" << endl;
        NM_exit(0);
    }
}

void NM_signal_handler(int signal)
{
    NM_config * config = NM_config::get_config();
    bool restart = false;
    config->set_flag(NM_FLAG_SIGNAL_SEND, true);
    
    do
    {
        if(config->get_flag(NM_FLAG_WRITE_DATA) == false)
            break;
        usleep(500);
    }while(1);
    
    if(signal == SIGSEGV && config->get_flag(NM_FLAG_DAEMON))
    {
        restart = true;
        NM_out << "Received signal: " << signal;
    }
    if(!config->get_flag(NM_FLAG_DAEMON))
    {
        if(signal == SIGSEGV)
        {
            NM_out.set_stream(NM_stream_cout);
            NM_bug("Segmentation fault signal was received");
        }
    }
    NM_exit(0, restart);
}

void NM_optimize_data()
{
    ostringstream query;
    NM_database * db = NM_database::get_instance();
    
    query << "BEGIN TRANSACTION; ";
    
    query << "INSERT OR REPLACE INTO objects_summary SELECT o.master_id, o.type, "
        << "SUM(rx_size) + ifnull((SELECT s.rx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0), "
        << "SUM(tx_size) + ifnull((SELECT s.tx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0) "
        << "FROM net_usage u LEFT JOIN objects o ON (o.id = u.program_id AND o.type = " << NM_object_program << ") "
        << "WHERE u.date > (SELECT od.date FROM optimize_date od WHERE od.type = \"summary\") AND o.master_id IS NOT NULL "
        << "GROUP BY o.master_id; ";
    
    query << "INSERT OR REPLACE INTO objects_summary SELECT o.master_id, o.type, "
        << "SUM(rx_size) + ifnull((SELECT s.rx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0), "
        << "SUM(tx_size) + ifnull((SELECT s.tx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0) "
        << "FROM net_usage u LEFT JOIN objects o ON (o.id = u.user_id AND o.type = " << NM_object_user << ") "
        << "WHERE u.date > (SELECT od.date FROM optimize_date od WHERE od.type = \"summary\") AND o.master_id IS NOT NULL "
        << "GROUP BY o.master_id; ";

    query << "INSERT OR REPLACE INTO objects_summary SELECT o.master_id, o.type, "
        << "SUM(rx_size) + ifnull((SELECT s.rx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0), "
        << "SUM(tx_size) + ifnull((SELECT s.tx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0) "
        << "FROM net_usage u LEFT JOIN objects o ON (o.id = u.device_id AND o.type = " << NM_object_device << ") "
        << "WHERE u.date > (SELECT od.date FROM optimize_date od WHERE od.type = \"summary\") AND o.master_id IS NOT NULL "
        << "GROUP BY o.master_id; ";
   
    query << "INSERT OR REPLACE INTO objects_summary SELECT o.master_id, o.type, "
        << "SUM(rx_size) + ifnull((SELECT s.rx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0), "
        << "SUM(tx_size) + ifnull((SELECT s.tx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0) "
        << "FROM net_usage u LEFT JOIN objects o ON (o.id = u.remote_id AND o.type = " << NM_object_remote << ") "
        << "WHERE u.date > (SELECT od.date FROM optimize_date od WHERE od.type = \"summary\") AND o.master_id IS NOT NULL "
        << "GROUP BY o.master_id; ";
    
    
    // dane compare dla programów
    query << "INSERT OR REPLACE INTO objects_compare SELECT o.master_id, o.type, o2.master_id as compare_master_id, o2.type as compare_type, "
        << "SUM(u.rx_size) + ifnull((SELECT rx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as rx_size, "
        << "SUM(u.tx_size) + ifnull((SELECT tx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as tx_size "
        << "FROM net_usage u "
        << "LEFT JOIN objects o ON(o.id = u.program_id AND o.type = " << NM_object_program << ") "
        << "LEFT JOIN objects o2 ON (o2.id = u.user_id AND o2.type = " << NM_object_user << ") "
        << "WHERE u.date > (SELECT date FROM optimize_date WHERE type = 'compare') AND o.master_id IS NOT NULL AND o2.master_id IS NOT NULL "
        << "GROUP BY o.master_id, o2.master_id; ";
    
    query << "INSERT OR REPLACE INTO objects_compare SELECT o.master_id, o.type, o2.master_id as compare_master_id, o2.type as compare_type, "
        << "SUM(u.rx_size) + ifnull((SELECT rx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as rx_size, "
        << "SUM(u.tx_size) + ifnull((SELECT tx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as tx_size "
        << "FROM net_usage u "
        << "LEFT JOIN objects o ON(o.id = u.program_id AND o.type = " << NM_object_program << ") "
        << "LEFT JOIN objects o2 ON (o2.id = u.device_id AND o2.type = " << NM_object_device << ") "
        << "WHERE u.date > (SELECT date FROM optimize_date WHERE type = 'compare') AND o.master_id IS NOT NULL AND o2.master_id IS NOT NULL "
        << "GROUP BY o.master_id, o2.master_id; ";
    
    query << "INSERT OR REPLACE INTO objects_compare SELECT o.master_id, o.type, o2.master_id as compare_master_id, o2.type as compare_type, "
        << "SUM(u.rx_size) + ifnull((SELECT rx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as rx_size, "
        << "SUM(u.tx_size) + ifnull((SELECT tx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as tx_size "
        << "FROM net_usage u "
        << "LEFT JOIN objects o ON(o.id = u.program_id AND o.type = " << NM_object_program << ") "
        << "LEFT JOIN objects o2 ON (o2.id = u.remote_id AND o2.type = " << NM_object_remote << ") "
        << "WHERE u.date > (SELECT date FROM optimize_date WHERE type = 'compare') AND o.master_id IS NOT NULL AND o2.master_id IS NOT NULL "
        << "GROUP BY o.master_id, o2.master_id; ";
    
    // dane compare dla użytkowników
            
    query << "INSERT OR REPLACE INTO objects_compare SELECT o.master_id, o.type, o2.master_id as compare_master_id, o2.type as compare_type, "
        << "SUM(u.rx_size) + ifnull((SELECT rx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as rx_size, "
        << "SUM(u.tx_size) + ifnull((SELECT tx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as tx_size "
        << "FROM net_usage u "
        << "LEFT JOIN objects o ON(o.id = u.user_id AND o.type = " << NM_object_user << ") "
        << "LEFT JOIN objects o2 ON (o2.id = u.device_id AND o2.type = " << NM_object_device << ") "
        << "WHERE u.date > (SELECT date FROM optimize_date WHERE type = 'compare') AND o.master_id IS NOT NULL AND o2.master_id IS NOT NULL "
        << "GROUP BY o.master_id, o2.master_id; ";
    
    query << "INSERT OR REPLACE INTO objects_compare SELECT o.master_id, o.type, o2.master_id as compare_master_id, o2.type as compare_type, "
        << "SUM(u.rx_size) + ifnull((SELECT rx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as rx_size, "
        << "SUM(u.tx_size) + ifnull((SELECT tx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as tx_size "
        << "FROM net_usage u "
        << "LEFT JOIN objects o ON(o.id = u.user_id AND o.type = " << NM_object_user << ") "
        << "LEFT JOIN objects o2 ON (o2.id = u.remote_id AND o2.type = " << NM_object_remote << ") "
        << "WHERE u.date > (SELECT date FROM optimize_date WHERE type = 'compare') AND o.master_id IS NOT NULL AND o2.master_id IS NOT NULL "
        << "GROUP BY o.master_id, o2.master_id; ";
    // dane compare dla interfejsów
    
    query << "INSERT OR REPLACE INTO objects_compare SELECT o.master_id, o.type, o2.master_id as compare_master_id, o2.type as compare_type, "
        << "SUM(u.rx_size) + ifnull((SELECT rx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as rx_size, "
        << "SUM(u.tx_size) + ifnull((SELECT tx_size FROM objects_compare WHERE master_id = o.master_id AND type = o.type AND compare_master_id = o2.master_id AND compare_type = o2.type), 0) as tx_size "
        << "FROM net_usage u "
        << "LEFT JOIN objects o ON(o.id = u.device_id AND o.type = " << NM_object_device << ") "
        << "LEFT JOIN objects o2 ON (o2.id = u.remote_id AND o2.type = " << NM_object_remote << ") "
        << "WHERE u.date > (SELECT date FROM optimize_date WHERE type = 'compare') AND o.master_id IS NOT NULL AND o2.master_id IS NOT NULL "
        << "GROUP BY o.master_id, o2.master_id; ";
    
    query << "UPDATE optimize_date SET date = ifnull((SELECT date FROM net_usage ORDER BY date DESC LIMIT 1), 0); ";
    
    query << "END TRANSACTION;";
    
    try{
        db->query(query.str()); 
    }
    catch(NM_exception e)
    {
        NM_trigger_warning(e.get_message());
    }
}