#ifndef NM_INI_VAR_LIST
#define NM_INI_VAR_LIST

#include "../../functions/global_h.hpp"
#include "../exception.hpp"
#include "iniVar.hpp"

class NM_iniVarList
{
  private:
    NM_iniVar * first, * current, * last;
  public:
    NM_iniVarList();
    ~NM_iniVarList();
    NM_iniVar * find_var(std::string /*name*/);

    NM_iniVar * get_first();
    NM_iniVar * get_current();
    NM_iniVar * get_last();

    void add_var(NM_iniVar * /*var*/);
    void add_comment_to_var(std::string /*comment*/, std::string /*var_name*/);

    std::string get_var_comment(std::string /*var_name*/);

    void change_var(NM_iniVar * /*var*/);
    void change_comment_in_var(std::string /*comment*/, std::string /*var_name*/);

    void remove_var(std::string /*var_name*/);
    void remove_comment_from_var(std::string /*var_name*/);

    std::string get_var_value(std::string /*var_name*/);
};

#endif
