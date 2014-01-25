#ifndef NM_INI_PARSER
#define NM_INI_PARSER

#include "../functions/global_h.hpp"
#include "exception.hpp"
#include "iniParser/iniSectionList.hpp"

class NM_iniParser
{
  private:
    std::string filename, comment;
    NM_iniSectionList * section_list;

  public:
    NM_iniParser(std::string /*filename*/);
    ~NM_iniParser();
    NM_iniSection * get_section(std::string = "" /*section_name*/);

    NM_iniVar * get_var(std::string /*var_name*/, std::string = "" /*section_name*/);
    NM_iniVarList * get_var_list_from_section(std::string = "" /*section_name*/);
    std::string get_var_value(std::string /*var_name*/, std::string = "" /*section_name*/);

    std::string get_file_comment();
    std::string get_section_comment(std::string /*section_name*/);
    std::string get_var_comment(std::string /*var_name*/, std::string /*section_name*/);

    NM_iniVar * find_var(std::string /*var_name*/, std::string /*section_name*/);

    void add_section(std::string /*section_name*/, NM_iniVarList * = NULL /*var_list*/, std::string = "" /*comment*/);
    void add_section(NM_iniSection * /*section*/);
    void add_var(std::string /*var_name*/, std::string /*var_value*/, std::string = "" /*section_name*/, bool = true /*create_section*/, std::string = "" /*comment*/);
    void add_var(NM_iniVar * /*var*/, std::string = "" /*section_name*/, bool = true /*create_section*/);

    void add_comment_to_file(std::string /*comment*/);
    void add_comment_to_section(std::string /*comment*/, std::string = "" /*section_name*/);
    void add_comment_to_var(std::string /*comment*/, std::string /*var_name*/, std::string = "" /*section_name*/);

    void change_section(NM_iniSection * /*section*/);
    void change_var(NM_iniVar * /*var*/, std::string = "" /*section_name*/);
    void change_var(std::string /*var_value*/, std::string /*var_name*/, std::string = "" /*section_name*/);
    void change_comment_in_file(std::string /*comment*/);
    void change_comment_in_section(std::string /*comment*/, std::string = "" /*section_name*/);
    void change_comment_in_var(std::string /*comment*/, std::string /*var_name*/, std::string = "" /*section_name*/);

    void remove_section(std::string = "" /*section_name*/);
    void remove_var(std::string /*var_name*/, std::string = "" /*section_name*/);
    void remove_comment_from_file();
    void remove_comment_from_section(std::string = "" /*section_name*/);
    void remove_comment_from_var(std::string /*var_name*/, std::string = "" /*section_name*/);

    void create_ini_file(std::string = "" /*filename*/, bool = false /*overwrite*/);
    void read_from_ini_file(std::string = "" /*filename*/);
  private:
    void write_section(std::fstream & /*file*/, NM_iniSection * section);
    std::string convert_comment_to_file(std::string /*comment*/);
};

#endif
