#ifndef NM_GLOBAL
#define NM_GLOBAL

#include <dirent.h> // dir_exists and NM_remap_inodes
#include <pthread.h>
#include "global_h.hpp"
#include "../classes/exception.hpp"
#include "../classes/iniParser.hpp"
#include "../classes/config.hpp"
#include "../classes/stream.hpp"
#include "../classes/ui.hpp"

enum NM_ip_type
{
    NM_ip_v4 = 1,
    NM_ip_v6
};

extern std::string NM_config_filename;
extern const std::string NM_log_name;
extern const std::string NM_debug_name;
extern std::string NM_debug_filename;
extern std::string NM_pc_info_filename;
extern std::string NM_data_dir;
extern const std::string NM_nmonit_dir;
extern const std::string NM_config_name;
extern const std::string NM_tmp_dir;

extern NM_stream NM_out;

NM_string_list explode_to_vector(std::string /*delimiter*/, std::string /*text*/);

std::string NM_log_prepare_date();

void NM_trigger_error(std::string /*msg*/);
void NM_trigger_warning(std::string /*msg*/, bool = false /*exit_program*/);
void NM_trigger_notice(std::string /*msg*/);

//void NM_exit(int /*status*/, bool = false/*restart*/);

void NM_bug(std::string /*msg*/);
//void NM_bug(const char * /*msg*/);

bool NM_check_installed();

bool dir_exists(std::string /*path*/);
bool file_exists(std::string /*file*/);

void clear_dir(std::string /*path*/);

void remove_dir(std::string /*path*/);

NM_double round(NM_double /*number*/, NM_int /*precision*/);

std::string NM_bold(std::string /*text*/);

std::string ip_to_string(in_addr /*ip*/);
std::string ip_to_string(in6_addr /*ip*/);
in_addr ip_to_in_addr(std::string /*ip*/);
in6_addr ip_to_in6_addr(std::string /*ip*/);

#endif
