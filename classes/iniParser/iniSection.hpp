#ifndef NM_INI_SECTION

#include "../../functions/global_h.hpp"
#include "iniVarList.hpp"

class NM_iniSection
{
  private:
    NM_iniSection * next, * prev;
    NM_iniVarList * child;
    std::string name;
    std::string comment;
  public:
    NM_iniSection(std::string = "" /*name*/, NM_iniVarList * = NULL /*var_list*/, std::string = "" /*comment*/);

    ~NM_iniSection();

    void set_next(NM_iniSection * /*next*/);
    void set_prev(NM_iniSection * /*prev*/);

    NM_iniSection * get_next();
    NM_iniSection * get_prev();

    NM_iniVarList * get_child();

    std::string get_name();

    void add_comment(std::string /*comment*/);
    std::string get_comment();
    std::string get_var_comment(std::string /*var_name*/);

    void add_var(NM_iniVar * /*var*/);
    void add_var(std::string /*var_name*/, std::string /*var_value*/, std::string = "" /*comment*/);
    NM_iniVar * find_var(std::string /*var_ name*/);

    void add_comment_to_var(std::string /*comment*/, std::string /*var_name*/);

    void change_comment(std::string /*comment*/);
    void change_var(NM_iniVar * /*var*/);
    void change_comment_in_var(std::string /*comment*/, std::string /*var_name*/);

    void remove_comment();
    void remove_var(std::string /*var_name*/);
    void remove_comment_from_var(std::string /*var_name*/);

    std::string get_var_value(std::string /*var_name*/);
};

#endif
