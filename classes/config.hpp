#ifndef NM_CONFIG
#define NM_CONFIG

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm> // std::find
#include "../functions/global.hpp"

//extern const std::string NM_config_filename;

typedef std::map<std::string /*flag_name*/, bool /*flag_val*/> NM_type_flags;

class NM_config
{
  private:
    NM_iniParser * conf;
    static NM_config * config_instance;
    NM_config();
    NM_config(const NM_config &);
    NM_config & operator =(const NM_config &);

    NM_type_flags flags;

    bool debug_sections_all;
    NM_string_list debug_sections;

    NM_string_list programs_permanent;
    bool programs_permanent_check;
    NM_string_list programs_allowed;
    bool programs_allowed_check;
    NM_string_list programs_ignored;
    bool programs_ignored_check;

    NM_string_list interfaces_permanent;
    bool interfaces_permanent_check;
    NM_string_list interfaces_allowed;
    bool interfaces_allowed_check;
    NM_string_list interfaces_ignored;
    bool interfaces_ignored_check;
    
    NM_string_list users_permanent;
    bool users_permanent_check;
    NM_string_list users_allowed;
    bool users_allowed_check;
    NM_string_list users_ignored;
    bool users_ignored_check;

  public:
    static NM_config * get_config();
    ~NM_config();

    void set_flag(std::string /*flag_name*/, bool /*val*/);
    bool get_flag(std::string /*flag_name*/);

    void set_monitored_section(std::string = "ALL" /*section_name*/);
    bool check_monitored_section(std::string /*secton_name*/);

    // if vector list empty all programs will be monitored
    void set_programs_permanent(NM_string_list /*program_list*/);
    void set_programs_allowed(NM_string_list /*program_list*/);
    void set_programs_ignored(NM_string_list /*program_list*/);
    NM_monitored_state check_program(std::string /*program_name*/);
    NM_monitored_state check_program(unsigned int /*inode*/);

    void set_interfaces_permanent(NM_string_list /*interface_list*/);
    void set_interfaces_allowed(NM_string_list /*interface_list*/);
    void set_interfaces_ignored(NM_string_list /*interface_list*/);
    NM_monitored_state check_interface(std::string /*interface_name*/);
    
    void set_users_permanent(NM_string_list /*user_list*/);
    void set_users_allowed(NM_string_list /*user_list*/);
    void set_users_ignored(NM_string_list /*user_list*/);
    NM_monitored_state check_user(std::string /*user_name*/);
};

#endif
