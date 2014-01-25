#ifndef NM_EXCEPTION
#define NM_EXCEPTION

#include "../functions/global_h.hpp"

enum NM_exception_code
{
    NM_ex_code_file_perm_denied = 1,
    NM_ex_code_file_del_error,
    NM_ex_code_dir_doesnt_exists,
    NM_ex_code_ini_section_exists,
    NM_ex_code_ini_section_doesnt_exists,
    NM_ex_code_ini_root_section_exists,
    NM_ex_code_ini_var_exists,
    NM_ex_code_ini_var_doesnt_exists,
    NM_ex_code_ini_parse_file_error,
    NM_ex_code_flag_doesnt_exists,
    NM_ex_code_inodes_remap_proc_dir_err,
    NM_ex_code_mutex_init_filed,
    NM_ex_code_stream_file_empty,
    NM_ex_code_stream_file_not_open,
    NM_ex_code_db_error
};

class NM_exception
{
  private:
    std::string message;
    NM_exception_code code;
  public:
    NM_exception(NM_exception_code /*code*/, std::string /*message*/);
    NM_exception_code get_code();
    std::string get_message();

  private:
    void init(NM_exception_code /*code*/, std::string /*message*/);
};

#endif
