#ifndef NM_MAIN_FUNCTIONS
#define NM_MAIN_FUNCTIONS

#include <pwd.h> // user info in NM_remap_inodes
#include <ctime> // actual time and date NM_write_date_to_info_pc_file
#include <math.h> // round - in calculating net usage
// #include <cmath> - pow - included in classes/ui.hpp
#include <netinet/in.h> // NM_remap_devices, capture packet functions, in_*, in6_* structures
#include <arpa/inet.h> // NM_remap_devices, inet_* functions
#include <ifaddrs.h> // NM_remap_devices
#include <netdb.h> // get host name from addr
#define _OPEN_SYS // directory / file info - using in NM_remap_inodes
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#undef _OPEN_SYS
#include <unistd.h> // - sleep()
#include "main_h.hpp"
#include "../classes/debug.hpp"
#include "../classes/connection.hpp"
#include "../classes/database.hpp"

typedef std::map<std::string, NM_connection * > NM_connection_list;
typedef std::map<NM_uint /*program_id*/, struct program_net_usage> NM_net_usage_list;

extern int NM_refresh_time;

extern NM_inode_list NM_inodes;
extern struct NM_date NM_current_date;
extern NM_thread_list NM_threads;
extern NM_connection_list NM_connections_v4;

extern pthread_mutex_t NM_mutex_remap_inodes, NM_mutex_remap_devices, NM_mutex_remap_remotes;

struct program_net_usage
{
    std::string program_name;
    std::string user_name;
    NM_uint user_id;
    std::string device_name;
    NM_uint device_id;
    NM_double tx_size;
    NM_size_type tx_size_type;
    NM_double rx_size;
    NM_size_type rx_size_type;
};

struct write_usage_info
{
    NM_id program_id;
    NM_id program_master_id;
    NM_id user_id;
    NM_id user_master_id;
    NM_id device_id;
    NM_id device_master_id;
    NM_id remote_id;
    NM_id remote_master_id;
    NM_double rx_size;
    NM_double tx_size;
};

struct usage_summary_info
{
    NM_double rx_size;
    NM_double tx_size;
};

typedef std::map<std::string /*name*/, write_usage_info /*address*/> NM_write_usage_info;
typedef std::map<NM_id /*master_id*/, usage_summary_info /*size*/> NM_summary_usage;

void NM_create_default_config(std::string /*file_name*/);

void NM_create_default_webservice_config(std::string /*file_name*/);

void NM_create_config_help();

void NM_display_help();

void NM_load_config();

void NM_load_objects();

void NM_remap_devices();
void NM_add_device(std::string /*name*/, in_addr /*address*/, in6_addr /*address6*/);
NM_remote * NM_add_remote(in_addr /*address*/);
//void NM_add_remote(std::string /*name*/, in_addr /*address*/);

void NM_remap_inodes();

std::string prepare_date(bool = true /*with_seconds*/);

void NM_save_object(NM_device * /*device*/);

void NM_save_object(NM_remote * /*remote*/);

void NM_save_object(NM_program * /*program*/);

void NM_save_object(NM_user * /*user*/);

void NM_write_net_usage();

void NM_show_net_usage(std::ostringstream & /*out*/);

void NM_calculate_net_size(NM_double & /*actual_size*/, NM_size_type & /*actual_size_type*/, NM_double /*adding_size*/, NM_size_type /*adding_size_type*/);

std::string NM_size_type_to_string(NM_size_type /*size_type*/);

#endif

