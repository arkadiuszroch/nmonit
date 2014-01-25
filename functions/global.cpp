#include "global.hpp"

using namespace std;

NM_string_list explode_to_vector(string delimiter, string text)
{
    NM_string_list v;
    size_t pos1 = 0, pos2 = string::npos;
    if( text.length() < 0 )
        return v;

    do
    {
        pos2 = text.find(delimiter, pos1);
        v.push_back(text.substr(pos1, pos2 - pos1));

        pos1 = pos2 + delimiter.length();

    }
    while( pos2 != string::npos );

    return v;
}

string NM_log_prepare_date()
{
    time_t t = time(0); // get time now
    struct tm * now = localtime(&t);
    ostringstream date;
    
    date << "[" << (now->tm_year + 1900) << "-" << (now->tm_mon + 1) << "-" << now->tm_mday << " ";
    date << (now->tm_hour + 1) << ":" << now->tm_min << ":" << now->tm_sec << "]";
    
    return date.str();
}

void NM_trigger_error(string msg)
{
    NM_out << NM_log_prepare_date() << "[***ERROR***]  " << msg << endl;
    NM_exit(1);
}

void NM_trigger_warning(string msg, bool exit_program)
{
    if( exit_program )
    {
        NM_out << NM_log_prepare_date() << "[**WARNING**]  " << msg << endl;
        NM_exit(1);
    }
    else
        NM_out << NM_log_prepare_date() << "[**WARNING**]  " << msg << endl;
}

void NM_trigger_notice(string msg)
{
    NM_out << NM_log_prepare_date() << "[*NOTICE*]  " << msg << endl;
}

void NM_exit(int status, bool restart)
{
    if( NM_config::get_config()->get_flag(NM_FLAG_DAEMON) )
    {
        fstream file("nmonit.pid", ios::out);
        if( file.is_open() )
        {
            file.close();
            unlink("nmonit.pid");
        }
    }

    if( NM_ui::initialized() )
        delete NM_ui::get_instance();

    if(restart)
        system("/etc/init.d/nmonit start");
    exit(status);
}

void NM_bug(string msg)
{
#ifdef NM_DEVELOP
    NM_out << NM_log_prepare_date() << "[~BUG~] " << msg << endl;
#endif
}

void NM_bug(const char * msg)
{
#ifdef NM_DEVELOP
    NM_out << NM_log_prepare_date() << "[~BUG~] " << msg << endl;
#endif
}

bool NM_check_installed()
{
    return dir_exists(NM_nmonit_dir);
}

bool dir_exists(string path)
{
    DIR * dir = NULL;
    dir = opendir(path.c_str());

    if( dir == NULL )
        return false;

    closedir(dir);

    return true;
}

bool file_exists(std::string file)
{
    ifstream fs(file.c_str(), ios::in);
    
    if(fs.is_open())
    {
        fs.close();
        return true;
    }
    
    fs.close();
    return false;
}

void clear_dir(string path)
{
    NM_debug * debug = NM_debug::get_instance("clear_dir");
    if(path.substr(path.length() - 1, 1).compare("/") == 0)
        path.substr(0, path.length() - 1);
    DIR * dir = NULL;
    dirent * entry = NULL;
    
    if( !dir_exists(path) )
        throw NM_exception(NM_ex_code_dir_doesnt_exists, "Directory: " + path + " doesn't exists");

    dir = opendir(path.c_str());
    while( entry = readdir(dir) )
    {
        if( string(entry->d_name).compare(".") == 0 || string(entry->d_name).compare("..") == 0 )
            continue;
        
        if( entry->d_type == DT_DIR )
            try
            {
                remove_dir(path + "/" + entry->d_name);
            }
            catch( NM_exception e )
            {
                throw NM_exception(e.get_code(), e.get_message());
            }
        else
        {
            debug->info("unlink " + path + "/" + entry->d_name);
        }
    }
    
    closedir(dir);
}

void remove_dir(string path)
{
    NM_debug * debug = NM_debug::get_instance("remove_dir");
    try
    {
        clear_dir(path);
    }
    catch( NM_exception e )
    {
        throw NM_exception(e.get_code(), e.get_message());
        return;
    }

    rmdir(path.c_str());
    debug->info("rmdir " + path);
}

NM_double round(NM_double number, NM_int precision)
{
    if(precision == 0)
    {
        NM_double floor_number = floor(number);
        if((number - floor_number) < 0.5)
            return floor_number;
        else
            return ceil(number);
    }
    else 
    {
        NM_uint precision_buf = 10 * precision;
        if(precision > 0)
            return (round(number * precision_buf, 0)) / precision_buf;
        else
            return (round(number / precision_buf, 0)) * precision_buf;
    }
}

string NM_bold(string text)
{
    int char_length = text.length();
    char str[char_length];
    string format = "%c[1m " + text + " %c[0m";
    sprintf(str, format.c_str(), 27, 27);
    return string(str);
}

string ip_to_string(in_addr ip)
{
    return inet_ntoa(ip);
}

string ip_to_string(in6_addr ip)
{
    char ret[INET6_ADDRSTRLEN];

    inet_ntop(AF_INET6, &ip, ret, INET6_ADDRSTRLEN);

    return string(ret);
}

in_addr ip_to_in_addr(string ip)
{
    struct in_addr address;
    inet_aton(ip.c_str(), &address);

    return address;
}

in6_addr ip_to_in6_addr(string ip)
{
    struct in6_addr address;

    inet_pton(AF_INET6, ip.c_str(), &address);

    return address;
}
