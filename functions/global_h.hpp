#ifndef NM_GLOBAL_H
#define NM_GLOBAL_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#define NM_VERSION "1.0.0"
//#define NM_DEVELOP 1

#define NM_ip_def_addr "0.0.0.0"
#define NM_ip6_def_addr "::0"

#define NM_PARAM_CREATE_CONFIG "-c"
#define NM_PARAM_CREATE_CONFIG_IGNORE "-ci"
#define NM_PARAM_CREATE_CONFIG_ALL "-a"
#define NM_PARAM_CREATE_CONFIG_ALL_EXT "--all"
#define NM_PARAM_CREATE_CONFIG_LOCAL "-l"
#define NM_PARAM_CREATE_CONFIG_LOCAL_EXT "--local"
#define NM_PARAM_CREATE_CONFIG_DAEMON "-d"
#define NM_PARAM_CREATE_CONFIG_DAEMON_EXT "--daemon"
#define NM_PARAM_CREATE_CONFIG_MONITOR "-s"
#define NM_PARAM_CREATE_CONFIG_MONITOR_EXT "--preview"
#define NM_PARAM_CREATE_CONFIG_WEBSERVICE "-p"
#define NM_PARAM_CREATE_CONFIG_WEBSERVICE_EXT "--presenter"
#define NM_PARAM_DAEMON "--daemon"
#define NM_PARAM_DAEMON_DISABLE "disable"
#define NM_PARAM_DAEMON_ENABLE "enable"
#define NM_PARAM_START_DAEMON "-d"
#define NM_PARAM_SHOW_NETWORK_USAGE "-s"
#define NM_PARAM_SHOW_NETWORK_USAGE_EXT "--preview"
#define NM_PARAM_DEBUG "-debug"
#define NM_PARAM_DEBUG_CONSOLE "-debug-console"
#define NM_PARAM_VERSION "-v"
#define NM_PARAM_VERSION_EXT "--version"
#define NM_PARAM_HELP "-h"
#define NM_PARAM_HELP_EXT "--help"
#define NM_PARAM_CLEAR "-clear"

#define NM_FLAG_CREATE_CONFIG "create-config"
#define NM_FLAG_CREATE_CONFIG_IGNORE "create-config-ignore"
#define NM_FLAG_DAEMON "daemon"
#define NM_FLAG_DEBUG "debug"
#define NM_FLAG_DEBUG_CONSOLE "debug-console"
#define NM_FLAG_SHOW_NET_USAGE "show-net-usage"
#define NM_FLAG_CLEAR "clear-test"
#define NM_FLAG_WRITE_DATA "write-data"
#define NM_FLAG_SIGNAL_SEND "signal-send"

// int jako 4 bajty. Różne kompilatory definują zakres zmiennych int. Od 4 do 6 bajtów co jest problemem przy pliku binarnym
typedef short int NM_int;
typedef unsigned short int NM_uint;
typedef NM_uint NM_id;
typedef double NM_double;
typedef std::vector<std::string> NM_string_list;
typedef std::vector<NM_string_list> NM_2D_string_list;

// we need it here for ui.
enum NM_size_type
{
    NM_b = 1,
    NM_kb,
    NM_mb,
    NM_gb,
    NM_tb
};

enum NM_monitored_state
{
    NM_not_monitored = 0,
    NM_monitored = 1,
    NM_monitored_permanent = 2
};

extern const std::string NM_database_filename;

void NM_exit(int /*status*/, bool = false/*restart*/); // function in global.h
#endif
