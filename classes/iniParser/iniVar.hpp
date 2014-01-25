#ifndef NM_INI_VAR
#define NM_INI_VAR

#include "../../functions/global_h.hpp"

class NM_iniVar
{
  private:
    NM_iniVar * next, * prev;
    std::string name, value, comment;
  public:
    NM_iniVar(std::string /*name*/, std::string /*value*/, std::string = ""/*comment*/);
    ~NM_iniVar();

    void set_next(NM_iniVar * /*var*/);
    void set_prev(NM_iniVar * /*var*/);

    NM_iniVar * get_next();
    NM_iniVar * get_prev();

    void add_comment(std::string /*comment*/);

    std::string get_name();
    std::string get_value();

    std::string get_comment();

    void change_comment(std::string /*comment*/);
    void remove_comment();
};

#endif
