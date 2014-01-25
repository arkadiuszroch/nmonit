#ifndef NMONIT
#define NMONIT

#include <stdexcept> // length_error
#include <csignal> // for signal handlign
//#include "functions/global.hpp"
#include "functions/mainFunctions.hpp"
#include "functions/packetCapture.hpp"

int NM_refresh_time;

#ifdef NM_DEVELOP
const std::string NM_base_dir = "./";
const std::string NM_nmonit_dir = NM_base_dir;
#else 
const std::string NM_base_dir = "/usr/lib/";
const std::string NM_nmonit_dir = NM_base_dir + "nmonit/";
#endif
const std::string NM_nmonit_etc_dir = "/etc/nmonit/";
const std::string NM_tmp_dir = "/var/tmp/";

const std::string NM_config_name = "nmonit.ini";
const std::string NM_log_name = ".log";
const std::string NM_debug_name = ".debug";

std::string NM_config_filename = NM_nmonit_dir + NM_config_name;

const std::string NM_daemon_config_filename = NM_nmonit_etc_dir + "daemon.ini";
const std::string NM_monitor_config_filename = NM_nmonit_etc_dir + "preview.ini";
const std::string NM_webservice_config_filename = NM_nmonit_etc_dir + "presenter.conf";

std::string NM_data_dir = NM_nmonit_dir + "data"; 
std::string NM_pc_info_filename = NM_data_dir + "/.pcInfo";
std::string NM_pid_filename = NM_nmonit_dir + ".nmonit.pid";
std::string NM_debug_filename = NM_nmonit_dir + NM_debug_name;

const std::string NM_database_filename = NM_nmonit_dir + "data/database.sql";

NM_stream NM_out;

NM_inode_list NM_inodes;
NM_connection_list NM_connections_v4;
NM_thread_list NM_threads;

pthread_mutex_t NM_mutex_conn_list, NM_mutex_remap_inodes, NM_mutex_remap_devices, NM_mutex_remap_remotes, NM_mutex_converter;

struct NM_date NM_current_date;

void NM_initialize_env();
void NM_check_root();

void NM_signal_handler(int /*signal*/);

void NM_optimize_data();

#endif
