#ifndef NM_INI_SECTION_LIST
#define NM_INI_SECTION_LIST

#include "../../functions/global_h.hpp"
#include "../exception.hpp"
#include "iniSection.hpp"

class NM_iniSectionList
{
  private:
    NM_iniSection * first, * current, * last, * root;
  public:
    NM_iniSectionList();
    ~NM_iniSectionList();
    NM_iniSection * find_section(std::string /*name*/);
    NM_iniSection * get_root_section();

    NM_iniSection * get_first();
    NM_iniSection * get_current();
    NM_iniSection * get_last();

    void add_root_section(NM_iniSection * /*root_section*/);
    void add_section(NM_iniSection * /*section*/);
    void add_comment_to_section(std::string /*comment*/, std::string = "" /*section_name*/);
    void add_var(NM_iniVar * /*var*/, std::string = "" /*section_name*/, bool = true /*create_section*/);
    void add_comment_to_var(std::string /*comment*/, std::string /*var_name*/, std::string = "" /*section_name*/);

    void change_root_section(NM_iniSection * /*section*/);
    void change_section(NM_iniSection * /*section*/);
    void change_comment_in_section(std::string /*comment*/, std::string = "" /*section_name*/);
    void change_var(NM_iniVar * /*var*/, std::string /*section_name*/);
    void change_comment_in_var(std::string /*comment*/, std::string /*var_name*/, std::string = "" /*section_name*/);

    void remove_root_section();
    void remove_section(std::string /*section_name*/);
    void remove_comment_from_section(std::string = "" /*section_name*/);
    void remove_var(std::string /*var_name*/, std::string /*section_name*/);
    void remove_comment_from_var(std::string /*var_name*/, std::string = "" /*section_name*/);
};

#endif
