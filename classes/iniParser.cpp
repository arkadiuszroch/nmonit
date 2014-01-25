#include "iniParser.hpp"

using namespace std;

/*
 *	Constructors
 */
NM_iniParser::NM_iniParser(string filename)
{
    this->comment = "";
    this->filename = filename;
    this->section_list = new NM_iniSectionList();
}

NM_iniParser::~NM_iniParser()
{
    delete this->section_list;
}

NM_iniSection * NM_iniParser::get_section(string section_name)
{
    return this->section_list->find_section(section_name);
}

NM_iniVar * NM_iniParser::get_var(string var_name, string section_name)
{
    NM_iniSection * section = NULL;

    section = this->section_list->find_section(section_name);

    if( section == NULL )
        return NULL;

    return section->find_var(var_name);
}

NM_iniVarList * NM_iniParser::get_var_list_from_section(string section_name)
{
    NM_iniSection * section = NULL;

    section = this->section_list->find_section(section_name);

    if( section == NULL )
        return NULL;

    return section->get_child();
}

string NM_iniParser::get_var_value(string var_name, string section_name)
{
    NM_iniSection * section = NULL;

    section = this->section_list->find_section(section_name);

    if( section == NULL )
        throw NM_exception(NM_ex_code_ini_section_doesnt_exists, "The section with that name doesn't exists ( " + section_name + " )");

    return section->get_var_value(var_name);
}

string NM_iniParser::get_file_comment()
{
    return this->comment;
}

string NM_iniParser::get_section_comment(string section_name)
{
    NM_iniSection * section = this->section_list->find_section(section_name);
    if( section == NULL )
    {
        throw NM_exception(NM_ex_code_ini_section_doesnt_exists, "The section with that name doesn't exists ( " + section_name + " )");
    }
    else
    {
        return section->get_comment();
    }
}

string NM_iniParser::get_var_comment(string var_name, string section_name)
{
    NM_iniSection * section = this->section_list->find_section(section_name);
    if( section == NULL )
    {
        throw NM_exception(NM_ex_code_ini_section_doesnt_exists, "The section with that name doesn't exists ( " + section_name + " )");
    }
    else
    {
        return section->get_var_comment(var_name);
    }
}

NM_iniVar * NM_iniParser::find_var(string var_name, string section_name)
{
    NM_iniSection * section = this->section_list->find_section(section_name);

    if( section == NULL )
        return NULL;

    return section->find_var(var_name);
}

void NM_iniParser::add_section(string section_name, NM_iniVarList * var_list, string comment)
{
    NM_iniSection * section = new NM_iniSection(section_name, var_list, comment);
    this->section_list->add_section(section);
}

void NM_iniParser::add_section(NM_iniSection * section)
{
    this->section_list->add_section(section);
}

void NM_iniParser::add_var(string var_name, string var_value, string section_name, bool create_section, string comment)
{
    NM_iniVar * var = new NM_iniVar(var_name, var_value, comment);
    this->section_list->add_var(var, section_name);
}

void NM_iniParser::add_var(NM_iniVar * var, string section_name, bool create_section)
{
    this->section_list->add_var(var, section_name);
}

void NM_iniParser::add_comment_to_file(string comment)
{
    if( comment.compare("") == 0 )
        return;
    if( this->comment.length() > 0 )
        this->comment += "\n;";

    this->comment += comment;
}

void NM_iniParser::add_comment_to_section(string comment, string section_name)
{
    this->section_list->add_comment_to_section(comment, section_name);
}

void NM_iniParser::add_comment_to_var(string comment, string var_name, string section_name)
{
    this->section_list->add_comment_to_var(comment, var_name, section_name);
}

void NM_iniParser::change_section(NM_iniSection * section)
{
    this->section_list->change_section(section);
}

void NM_iniParser::change_var(NM_iniVar * var, string section_name)
{
    this->section_list->change_var(var, section_name);
}

void NM_iniParser::change_comment_in_file(string comment)
{
    this->comment = comment;
}

void NM_iniParser::change_comment_in_section(string comment, string section_name)
{
    this->section_list->change_comment_in_section(comment, section_name);
}

void NM_iniParser::change_comment_in_var(string comment, string var_name, string section_name)
{
    this->section_list->change_comment_in_var(comment, var_name, section_name);
}

void NM_iniParser::remove_section(string section_name)
{
    this->section_list->remove_section(section_name);
}

void NM_iniParser::remove_var(string var_name, string section_name)
{
    this->section_list->remove_var(var_name, section_name);
}

void NM_iniParser::remove_comment_from_file()
{
    this->comment = "";
}

void NM_iniParser::remove_comment_from_section(string section_name)
{
    this->section_list->remove_comment_from_section(section_name);
}

void NM_iniParser::remove_comment_from_var(string var_name, string section_name)
{
    this->section_list->remove_comment_from_var(var_name, section_name);
}

void NM_iniParser::create_ini_file(string filename, bool overwrite)
{
    fstream file;
    filename = (filename.compare("") == 0) ? this->filename : filename;

    file.open(filename.c_str(), ios::out);

    if( !file.is_open() )
    {
        string ex_msg = "Can't open a file: ";
        ex_msg += filename;
        throw NM_exception(NM_ex_code_file_perm_denied, ex_msg);
    }
    if( this->comment.compare("") != 0 )
    {
        file << ";;;;;" << "\n";
        file << this->convert_comment_to_file(this->comment);
        file << "\n";
        file << ";;;;;\n\n";
    }

    NM_iniSection * section = this->section_list->get_root_section();

    if( section != NULL )
    {
        write_section(file, section);
    }

    section = this->section_list->get_first();

    while( section != NULL )
    {
        write_section(file, section);
        section = section->get_next();
    }

    file.close();

}

void NM_iniParser::write_section(fstream & file, NM_iniSection * section)
{
    string comment = section->get_comment();
    NM_iniVar * var = NULL;

    if( comment.length() > 0 )
    {
        file << this->convert_comment_to_file(comment);
        file << "\n";
    }

    file << "[" << section->get_name() << "]\n";

    var = section->get_child()->get_first();

    while( var != NULL )
    {
        file << "\n";
        if( var->get_comment().length() > 0 )
            file << this->convert_comment_to_file(var->get_comment()) << "\n";
        file << var->get_name() << "=" << var->get_value() << "\n";
        var = var->get_next();
    }
    file << "\n\n";
}

void NM_iniParser::read_from_ini_file(string filename)
{
    string line, comment = "";
    ostringstream sstream;
    NM_iniVarList * var_list = NULL;
    fstream file;
    
    filename = (filename.compare("") == 0) ? this->filename : filename;

    file.open(filename.c_str(), ios::in);

    if( !file.is_open() )
    {
        sstream.str(string(""));
        sstream << "Can't open a file: " << filename;
        throw NM_exception(NM_ex_code_file_perm_denied, sstream.str());
    }

    int i = 0;

    while( getline(file, line) )
    {
        i++;

        if( line.length() == 0 )
            continue;

        if( line.substr(0, 5).compare(";;;;;") == 0 )
        {
            if( line.length() > 5 )
                this->add_comment_to_file(line.substr(6));
            while( getline(file, line) )
            {
                i++;
                if( line.substr(0, 5).compare(";;;;;") == 0 )
                {
                    if( line.length() > 5 )
                        this->add_comment_to_file(line.substr(6));
                    break;
                }
                if( line.substr(0, 1).compare(";") != 0 )
                {
                    sstream.str(string(""));
                    sstream << "Parse Error: Unexpected character '" << line.substr(0, 1) << "' at line: " << i << " in file: '" << filename << "'. Expected ';'.";
                    throw NM_exception(NM_ex_code_ini_parse_file_error, sstream.str());
                }
                this->add_comment_to_file(line.substr(1));
            }
        }
        else if( line.substr(0, 1).compare(";") == 0 )
        {
            if( comment.length() != 0 )
                comment += "\n";

            comment += line.substr(1);
        }
        else if( line.substr(0, 1).compare("[") == 0 )
        {
            size_t pos = 0, pos_check = 0;
            string section_name;
            if( line.substr(1).find("[") != string::npos )
            {
                sstream.str(string(""));
                sstream << "Parse error: Found multiple section open square brakets at line: " << i << " in file: " << filename;
                throw NM_exception(NM_ex_code_ini_parse_file_error, sstream.str());
            }
            pos = line.find_last_of("]");
            pos_check = line.find("]");
            if( pos != pos_check )
            {
                sstream.str(string(""));
                sstream << "Parse error: Found multiple section ending square brakets at line: " << i << " in file: " << filename;
                throw NM_exception(NM_ex_code_ini_parse_file_error, sstream.str());
            }
            else if( line.length() > pos + 1 )
            {
                sstream.str(string(""));
                sstream << "Parse error: Unexpected character after ']'. Expected end of line at line: " << i << " in file: " << filename;
                throw NM_exception(NM_ex_code_ini_parse_file_error, sstream.str());
            }

            section_name = line.substr(1, line.length() - 2);

            var_list = new NM_iniVarList();
            this->add_section(section_name, var_list, comment);

            comment.clear();

        }
        else
        {
            size_t pos = 0;
            if( (pos = line.find("=")) != string::npos )
            {
                if( line.find_last_of("=") != pos )
                {
                    sstream.str(string(""));
                    sstream << "Parse error: Found multiple equal signs at line: " << i << " in file: " << filename;
                    throw NM_exception(NM_ex_code_ini_parse_file_error, sstream.str());
                }
                if(var_list == NULL)
                   var_list = new NM_iniVarList(); 
                var_list->add_var(new NM_iniVar(line.substr(0, pos), line.substr(pos + 1), comment));
                comment.clear();
            }
            else
            {
                sstream.str(string(""));
                sstream << "Parse error: Unexpected string at line: " << i << " in file: " << filename << ". Expected variable declaration";
                throw NM_exception(NM_ex_code_ini_parse_file_error, sstream.str());
            }
        }
    }

    file.close();
}

string NM_iniParser::convert_comment_to_file(string comment)
{
    size_t pos;
    comment.insert(0, ";");
    while( (pos = comment.find("\n", pos)) != string::npos )
    {
        pos++;
        if( comment.length() > pos && comment.substr(pos + 1, 1).compare(";") != 0 )
        {
            comment.insert(pos, ";");
        }
    }

    return comment;
}
