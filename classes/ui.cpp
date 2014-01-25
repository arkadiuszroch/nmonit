#include <math.h>

#include "ui.hpp"

using namespace std;

NM_ui * NM_ui::instance = NULL;

NM_ui::NM_ui()
{
    NM_debug * debug = NM_debug::get_instance("NM_ui::constructor");
    debug->info("create instance of NM_ui");
            
    NM_config * config = NM_config::get_config();
    
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    
    start_color();
    
    if(config->get_flag(NM_FLAG_DEBUG))
    {
        init_pair(NM_UI_COLOR_TITLE, COLOR_BLACK, COLOR_WHITE);
        init_pair(NM_UI_COLOR_PROGRAM, COLOR_WHITE, COLOR_YELLOW);
        init_pair(NM_UI_COLOR_USER, COLOR_WHITE, COLOR_BLUE);
        init_pair(NM_UI_COLOR_UPLOAD, COLOR_WHITE, COLOR_CYAN);
        init_pair(NM_UI_COLOR_SUM_UPLOAD, COLOR_WHITE, COLOR_GREEN);
        init_pair(NM_UI_COLOR_DOWNLOAD, COLOR_WHITE, COLOR_CYAN);
        init_pair(NM_UI_COLOR_SUM_DOWNLOAD, COLOR_WHITE, COLOR_GREEN);
    }
    else
    {
        init_pair(NM_UI_COLOR_TITLE, COLOR_BLACK, COLOR_WHITE);
        init_pair(NM_UI_COLOR_PROGRAM, COLOR_WHITE, COLOR_BLACK);
        init_pair(NM_UI_COLOR_USER, COLOR_WHITE, COLOR_BLACK);
        init_pair(NM_UI_COLOR_UPLOAD, COLOR_WHITE, COLOR_BLACK);
        init_pair(NM_UI_COLOR_SUM_UPLOAD, COLOR_WHITE, COLOR_BLACK);
        init_pair(NM_UI_COLOR_DOWNLOAD, COLOR_WHITE, COLOR_BLACK);
        init_pair(NM_UI_COLOR_SUM_DOWNLOAD, COLOR_WHITE, COLOR_BLACK);
    }
    
    config->set_flag(NM_FLAG_SORT_PROGRAM, true);
    config->set_flag(NM_FLAG_SORT_PROGRAM_DESC, false);
    config->set_flag(NM_FLAG_SORT_USER, false);
    config->set_flag(NM_FLAG_SORT_USER_DESC, false);
    config->set_flag(NM_FLAG_SORT_DOWNLOAD, false);
    config->set_flag(NM_FLAG_SORT_SUM_DOWNLOAD, false);
    config->set_flag(NM_FLAG_SORT_DOWNLOAD_DESC, false);
    config->set_flag(NM_FLAG_SORT_SUM_DOWNLOAD_DESC, false);
    config->set_flag(NM_FLAG_SORT_UPLOAD, false);
    config->set_flag(NM_FLAG_SORT_SUM_UPLOAD, false);
    config->set_flag(NM_FLAG_SORT_UPLOAD_DESC, false);
    config->set_flag(NM_FLAG_SORT_SUM_UPLOAD_DESC, false);
    
    
    this->width = 0;
    this->height = 0;
    
    this->init();
    
    refresh();
    debug->info("NM_ui instance created");
}

void NM_ui::init()
{
    NM_debug * debug = NM_debug::get_instance("NM_ui::init");
    debug->info("Start init NM_ui");
    curs_set(0);
    
    NM_config * config = NM_config::get_config();
    string header_buf;
    string title_buf;
    int title_size;
    int welcome_position = 0, line = 0, width = 0;
    char up = '^', down = '.';
    
    getmaxyx(stdscr, this->height, width);
    
    if(this->width != width)
    {
        int max_content;
        this->width = width;
        
        if(this->width <= NM_UI_SIZE_MIN)
        {
            this->width = NM_UI_SIZE_MIN;
            this->w_program_title = NM_UI_SIZE_PROGRAM;
            //this->w_user_title = NM_UI_SIZE_USER;
            this->w_upload_title = NM_UI_SIZE_UPLOAD;
            this->w_download_title = NM_UI_SIZE_DOWNLOAD;
        }
        else if(this->width > NM_UI_SIZE_MIN)
        {
            int diff = 0; 
            this->w_program_title = floor(this->width * NM_UI_RATIO_PROGRAM);
            //this->w_user_title = floor(this->width * NM_UI_RATIO_USER);
            this->w_upload_title = floor(this->width * NM_UI_RATIO_UPLOAD);
            this->w_download_title = floor(this->width * NM_UI_RATIO_DOWNLOAD);
            
            diff = (this->w_program_title /*+ this->w_user_title */ + this->w_upload_title + this->w_download_title) - this->width;
            if(diff > 0)
                this->w_program_title += diff;
            else if(diff < 0)
                this->w_program_title -= diff;
        }
        
        this->w_program_content = this->w_program_title;
        this->w_program_title += 1;
        this->s_program_title = this->s_program_content = 0;
        
        
        this->w_user_content = this->w_user_title - 1;
        this->s_user_title = this->s_user_content = this->w_program_title; // we don't remove subtract 1 becase start screen is 0 and min height is 1
        
        
        this->w_upload_content = this->w_upload_title - 1;
        this->s_upload_title = this->s_program_title + this->w_program_title;
        
        max_content = ((this->w_upload_title - 1 ) % 2 ) ? this->w_upload_title - 2 : this->w_upload_title - 1;

        this->w_upload_actual_content = this->w_upload_sum_content = floor((max_content - 1) / 2);

        this->s_upload_content = this->s_upload_title;
        this->s_upload_sum_content = this->s_upload_content + this->w_upload_actual_content + 1;
            
        
        this->w_download_title -= 1;
        this->s_download_title = this->s_upload_title + this->w_upload_title;
        
        this->s_download_content = this->s_download_title ;
        this->w_download_content = this->w_upload_content;
        this->w_download_actual_content = this->w_upload_actual_content;
        this->w_download_sum_content = this->w_upload_sum_content;
        
        this->s_download_sum_content = this->s_download_content + this->w_download_actual_content + 1;
    }
    
    this->s_middle = this->width / 2;
    
    header_buf = "NMonit v%s - Welcome";
    welcome_position = static_cast<int>(this->s_middle - (header_buf.length() / 2));
    
    mvprintw(line++, welcome_position, header_buf.c_str(), NM_VERSION);
    header_buf = "NMonit running with pid: %d";
    if(config->get_flag(NM_FLAG_DEBUG))
        header_buf += " ( with debug mode )";
    mvprintw(line++, 0, header_buf.c_str(), getpid());
    
    mvprintw(line++, 0, "%c - exit program", NM_UI_QUIT);
    
    mvprintw(line++, 0, "Sort: ");
    mvprintw(line++, 0, "%c - by program name", NM_UI_SORT_PROGRAM);
    //mvprintw(line++, this->s_middle, "%c - sort by user name", NM_UI_SORT_USER);
    
    mvprintw(line, 0, "%c - by download size", NM_UI_SORT_DOWNLOAD);
    mvprintw(line++, this->s_middle, "%c - by upload size", NM_UI_SORT_UPLOAD);
    
    mvprintw(line, 0, "%c - by total download size", NM_UI_SORT_SUM_DOWNLOAD);
    mvprintw(line++, this->s_middle, "%c - by total upload size", NM_UI_SORT_SUM_UPLOAD);
    
    this->command_line = line++;
    this->current_line = line + 1;
    
    attron(COLOR_PAIR(1));
    
    mvprintw(line, 0, string(this->width, ' ').c_str());
    
    
    title_buf = "Program";
    title_size = title_buf.length();
    title_buf.resize(this->w_program_title, ' ');
    
    mvprintw(line, this->s_program_title, "%s", title_buf.c_str());
    if(config->get_flag(NM_FLAG_SORT_PROGRAM))
        mvprintw(line, this->s_program_title + title_size, " %c", up);
    else if(config->get_flag(NM_FLAG_SORT_PROGRAM_DESC))
        mvprintw(line, this->s_program_title + title_size, " %c", down);
    
    
    /*title_buf = "User";
    title_size = title_buf.length();
    title_buf.resize(this->w_user_title, ' ');
    
    mvprintw(line, this->s_user_title, "%s", title_buf.c_str());
    if(config->get_flag(NM_FLAG_SORT_USER))
        mvprintw(line, this->s_user_title + title_size, " %c", up);
    else if(config->get_flag(NM_FLAG_SORT_USER_DESC))
        mvprintw(line, this->s_user_title + title_size, " %c", down);
    */
    
    if(config->get_flag(NM_FLAG_SORT_UPLOAD)){
        title_buf = "Upload actual%c / total";
        if(this->w_upload_title > title_buf.length())
            title_buf.resize(this->w_upload_title, ' ');
        mvprintw(line, this->s_upload_title, title_buf.c_str(), up);
    }
    else if(config->get_flag(NM_FLAG_SORT_UPLOAD_DESC))
    {
        title_buf = "Upload actual%c / total";
        if(this->w_upload_title > title_buf.length())
            title_buf.resize(this->w_upload_title, ' ');
        mvprintw(line, this->s_upload_title, title_buf.c_str(), down);
    }
    else if(config->get_flag(NM_FLAG_SORT_SUM_UPLOAD)){
        title_buf = "Upload actual / total%c";
        if(this->w_upload_title > title_buf.length())
            title_buf.resize(this->w_upload_title, ' ');
        mvprintw(line, this->s_upload_title, title_buf.c_str(), up);
    }
    else if(config->get_flag(NM_FLAG_SORT_SUM_UPLOAD_DESC))
    {
        title_buf = "Upload actual / total%c";
        if(this->w_upload_title > title_buf.length())
            title_buf.resize(this->w_upload_title, ' ');
        mvprintw(line, this->s_upload_title, title_buf.c_str(), down);
    }
    else
    {
        title_buf = "Upload actual / total";
        if(this->w_upload_title > title_buf.length())
            title_buf.resize(this->w_upload_title, ' ');
        mvprintw(line, this->s_upload_title, title_buf.c_str());
    }
    
    
    if(config->get_flag(NM_FLAG_SORT_DOWNLOAD)){
        title_buf = "Download actual%c / total";
        if(this->w_download_title > title_buf.length())
            title_buf.resize(this->w_download_title, ' ');
        mvprintw(line, this->s_download_title, title_buf.c_str(), up);
    }
    else if(config->get_flag(NM_FLAG_SORT_DOWNLOAD_DESC))
    {
        title_buf = "Download actual%c / total";
        if(this->w_download_title > title_buf.length())
            title_buf.resize(this->w_download_title, ' ');
        mvprintw(line, this->s_download_title, title_buf.c_str(), down);
    }
    else if(config->get_flag(NM_FLAG_SORT_SUM_DOWNLOAD)){
        title_buf = "Download actual / total%c";
        if(this->w_download_title > title_buf.length())
            title_buf.resize(this->w_download_title, ' ');
        mvprintw(line, this->s_download_title, title_buf.c_str(), up);
    }
    else if(config->get_flag(NM_FLAG_SORT_SUM_DOWNLOAD_DESC))
    {
        title_buf = "Download actual / total%c";
        if(this->w_download_title > title_buf.length())
            title_buf.resize(this->w_download_title, ' ');
        mvprintw(line, this->s_download_title, title_buf.c_str(), down);
    }
    else
    {
        title_buf = "Download actual / total";
        if(this->w_download_title > title_buf.length())
            title_buf.resize(this->w_download_title, ' ');
        mvprintw(line, this->s_download_title, title_buf.c_str());
    }
    
    attroff(COLOR_PAIR(1));
    debug->info("NM_ui init end");
}

NM_ui::~NM_ui()
{
    resetty();
    endwin();
    NM_ui::instance = NULL;
}

NM_ui * NM_ui::get_instance()
{
    if(NM_ui::instance == NULL)
        NM_ui::instance = new NM_ui();
    return NM_ui::instance;
}

bool NM_ui::initialized()
{
    return (NM_ui::instance != NULL);
}

bool NM_ui::add_line(string program_name, string user_name, double upload, double download, string size_type, double sum_upload, double sum_download, string sum_size_type)
{
    NM_debug * debug = NM_debug::get_instance("NM_ui::add_line");
    debug->info("Start NM_ui add line");
    if( this->current_line <= this->height )
    {
        ostringstream sstream;
        string buf;
        int end_pos;
        if( program_name.length() > this->w_program_content )
        {
            program_name.resize(this->w_program_content - 3);
            program_name += "...";
        }
        else
            program_name.resize(this->w_program_content, ' ');
        
        /*if( user_name.length() > this->w_user_content )
        {
            user_name.resize(this->w_user_content - 3);
            user_name += "...";
        }
        else
            user_name.resize(this->w_user_content, ' ');
        */
        attron(COLOR_PAIR(NM_UI_COLOR_PROGRAM));
        mvprintw(this->current_line, this->s_program_title, "%s", program_name.c_str());
        attroff(COLOR_PAIR(NM_UI_COLOR_PROGRAM));
        /*attron(COLOR_PAIR(NM_UI_COLOR_USER));
        mvprintw(this->current_line, this->s_user_title, "%s", user_name.c_str());
        attroff(COLOR_PAIR(NM_UI_COLOR_USER));
        */
        // printing upload
        sstream.str("");
        sstream << fixed << setprecision(2) << upload << size_type << "/s";
        buf = sstream.str();
        end_pos = this->w_upload_actual_content - buf.length();
        if(size_type.compare(NM_size_type_to_string(NM_b)) == 0)
            end_pos -= 1;
        if(end_pos < 0)
            end_pos = 0;
        debug->info(buf);
        debug->info(end_pos);
        buf.insert(0, end_pos, ' ');
        
        /*end_pos = this->w_upload_title - buf.length() - 1;
        
        buf.insert(0, end_pos, ' ');
        */
        
        attron(COLOR_PAIR(NM_UI_COLOR_UPLOAD));
        mvprintw(this->current_line, this->s_upload_content, "%s", buf.c_str());
        attroff(COLOR_PAIR(NM_UI_COLOR_UPLOAD));
        
        sstream.str("");
        sstream << fixed << setprecision(2) << sum_upload << sum_size_type;
        buf = sstream.str();
        end_pos = this->w_upload_sum_content - buf.length();
        if(size_type.compare(NM_size_type_to_string(NM_b)) == 0)
            end_pos -= 1;
        if(end_pos < 0)
            end_pos = 0;
        debug->info(buf);
        debug->info(end_pos);
        buf.insert(0, end_pos, ' ');
        
        attron(COLOR_PAIR(NM_UI_COLOR_SUM_UPLOAD));
        mvprintw(this->current_line, this->s_upload_sum_content, "%s", buf.c_str());
        attroff(COLOR_PAIR(NM_UI_COLOR_SUM_UPLOAD));
        
        // printing download
        sstream.str("");
        sstream << fixed << setprecision(2) << download << size_type << "/s";
        buf = sstream.str();
        end_pos = this->w_download_actual_content - buf.length();
        if(size_type.compare(NM_size_type_to_string(NM_b)) == 0)
            end_pos -= 1;
        if(end_pos < 0)
            end_pos = 0;
        debug->info(buf);
        debug->info(end_pos);
        buf.insert(0,  end_pos, ' ');
        
        /*end_pos = this->w_upload_title - buf.length() - 1;
        
        buf.insert(0, end_pos, ' ');
        */
        attron(COLOR_PAIR(NM_UI_COLOR_DOWNLOAD));
        mvprintw(this->current_line, this->s_download_content, "%s", buf.c_str());
        attroff(COLOR_PAIR(NM_UI_COLOR_DOWNLOAD));
        
        sstream.str("");
        sstream << fixed << setprecision(2) << sum_download << sum_size_type;
        buf = sstream.str();
        end_pos = this->w_download_sum_content - buf.length();
        if(sum_size_type.compare(NM_size_type_to_string(NM_b)) == 0)
            end_pos -= 1;
        if(end_pos < 0)
            end_pos = 0;
        debug->info(buf);
        debug->info(end_pos);
        buf.insert(0, end_pos, ' ');
        
        attron(COLOR_PAIR(NM_UI_COLOR_SUM_DOWNLOAD));
        mvprintw(this->current_line, this->s_download_sum_content, "%s", buf.c_str());
        attroff(COLOR_PAIR(NM_UI_COLOR_SUM_DOWNLOAD));
        this->current_line++;
    }
    
    
    //refresh();
    //clear();
    debug->info("NM_ui add line end");
    return (this->current_line <= this->height);
}

void NM_ui::add(string program_name, string user_name, double upload, double download, NM_size_type size_type)
{
    NM_debug * debug = NM_debug::get_instance("NM_ui:add");
    debug->info("Start add program");
    NM_ui_net_usage actual;
    bool actual_set = false;
    int size = this->net_usage.size(), i = 0;
    
    for(i = 0; i < size; i++)
    {
        if(this->net_usage[i].program_name.compare(program_name) == 0)
        {
            actual = this->net_usage[i];
            actual_set = true;
            break;
        }
    }
    
    if( !actual_set )
    {
        actual.program_name = program_name;
        actual.user_name = user_name;
        actual.upload = 0.0;
        actual.download = 0.0;
        actual.current_size_type = NM_b;
        actual.sum_upload = 0.0;
        actual.sum_download = 0.0;
        actual.sum_size_type = NM_b;
    }
    
    actual.upload = upload;
    actual.download = download;
    actual.current_size_type = size_type;
    
    if(actual.sum_size_type != size_type)
    {
        if(size_type < actual.sum_size_type)
        {
            unsigned short int p = actual.sum_size_type - size_type;
            long long b = 1024;
            upload /= pow(b, p);
            download /= pow(b, p);
            size_type = actual.sum_size_type;
        }
        else
        {
            unsigned short int p = size_type - actual.sum_size_type;
            actual.sum_upload /= pow(1024, p);
            actual.sum_download /= pow(1024, p);
            actual.sum_size_type = size_type;
        }
    }
    
    
    actual.sum_upload += upload;
    actual.sum_download += download;
    
    while(actual.sum_upload > 1024.00 || actual.sum_download > 1024.00)
    {
        actual.sum_upload /= 1024.00;
        actual.sum_download /= 1024.00;
        
        switch(actual.sum_size_type)
        {
            case NM_b:
                actual.sum_size_type = NM_kb;
            break;
            case NM_kb:
                actual.sum_size_type = NM_mb;
            break;
            case NM_mb:
                actual.sum_size_type = NM_gb;
            break;
            case NM_gb:
                actual.sum_size_type = NM_tb;
            break;
        }
    }
    
    if(actual_set)
        this->net_usage[i] = actual;
    else
        this->net_usage.push_back(actual);
    
    debug->info("End add program");
}

void NM_ui::tick()
{
    int line = this->current_line;
    NM_config * config = NM_config::get_config();
    NM_debug * debug = NM_debug::get_instance("NM_ui::tick");
    debug->info("start tick");
    int c = getch();
    //mvprintw(this->command_line, 0, "%d - %c", c, (char)c);
    //sleep(3);
    if(c != ERR)
    {
        switch(c)
        {
            case NM_UI_SORT_PROGRAM:
            case NM_UI_SORT_PROGRAM_C:
            {
                this->disable_flags(NM_FLAG_SORT_PROGRAM);
                if( config->get_flag(NM_FLAG_SORT_PROGRAM) )
                {
                    config->set_flag(NM_FLAG_SORT_PROGRAM, false);
                    config->set_flag(NM_FLAG_SORT_PROGRAM_DESC, true);
                }
                else
                {
                    config->set_flag(NM_FLAG_SORT_PROGRAM, true);
                    config->set_flag(NM_FLAG_SORT_PROGRAM_DESC, false);
                }    
            }
            break;
            case NM_UI_SORT_USER:
            case NM_UI_SORT_USER_C:
            {
                this->disable_flags(NM_FLAG_SORT_USER);
                if( config->get_flag(NM_FLAG_SORT_USER) )
                {
                    config->set_flag(NM_FLAG_SORT_USER, false);
                    config->set_flag(NM_FLAG_SORT_USER_DESC, true);
                }
                else
                {
                    config->set_flag(NM_FLAG_SORT_USER, true);
                    config->set_flag(NM_FLAG_SORT_USER_DESC, false);
                }    
            }
            break;
            case NM_UI_SORT_DOWNLOAD:
            case NM_UI_SORT_DOWNLOAD_C:
            {
                this->disable_flags(NM_FLAG_SORT_DOWNLOAD);
                if( config->get_flag(NM_FLAG_SORT_DOWNLOAD) )
                {
                    config->set_flag(NM_FLAG_SORT_DOWNLOAD, false);
                    config->set_flag(NM_FLAG_SORT_DOWNLOAD_DESC, true);
                }
                else
                {
                    config->set_flag(NM_FLAG_SORT_DOWNLOAD, true);
                    config->set_flag(NM_FLAG_SORT_DOWNLOAD_DESC, false);
                }    
            }
            break;
            case NM_UI_SORT_SUM_DOWNLOAD:
            case NM_UI_SORT_SUM_DOWNLOAD_C:
            {
                this->disable_flags(NM_FLAG_SORT_SUM_DOWNLOAD);
                if( config->get_flag(NM_FLAG_SORT_SUM_DOWNLOAD) )
                {
                    config->set_flag(NM_FLAG_SORT_SUM_DOWNLOAD, false);
                    config->set_flag(NM_FLAG_SORT_SUM_DOWNLOAD_DESC, true);
                }
                else
                {
                    config->set_flag(NM_FLAG_SORT_SUM_DOWNLOAD, true);
                    config->set_flag(NM_FLAG_SORT_SUM_DOWNLOAD_DESC, false);
                }    
            }
            break;
            /*case NM_UI_SORT_UPLOAD:
            case NM_UI_SORT_UPLOAD_C:
            {
                this->disable_flags(NM_FLAG_SORT_UPLOAD);
                if( config->get_flag(NM_FLAG_SORT_UPLOAD) )
                {
                    config->set_flag(NM_FLAG_SORT_UPLOAD, false);
                    config->set_flag(NM_FLAG_SORT_UPLOAD_DESC, true);
                }
                else
                {
                    config->set_flag(NM_FLAG_SORT_UPLOAD, true);
                    config->set_flag(NM_FLAG_SORT_UPLOAD_DESC, false);
                }    
            };
            break;*/
            case NM_UI_SORT_SUM_UPLOAD:
            case NM_UI_SORT_SUM_UPLOAD_C:
            {
                this->disable_flags(NM_FLAG_SORT_SUM_UPLOAD);
                if( config->get_flag(NM_FLAG_SORT_SUM_UPLOAD) )
                {
                    config->set_flag(NM_FLAG_SORT_SUM_UPLOAD, false);
                    config->set_flag(NM_FLAG_SORT_SUM_UPLOAD_DESC, true);
                }
                else
                {
                    config->set_flag(NM_FLAG_SORT_SUM_UPLOAD, true);
                    config->set_flag(NM_FLAG_SORT_SUM_UPLOAD_DESC, false);
                }    
            };
            break;
            case NM_UI_QUIT:
            case NM_UI_QUIT_C:
                NM_exit(0);
            break;
            default:
                mvprintw(this->command_line, 0, "Unrecognized option. Please use one of listed below.");
        }
        // clearing getch buffer
        while(getch() != ERR);
    }
    
    this->init();
    
    if( line < this->height )
    {
        while(line <= this->height)
        {
            mvprintw(line++, 0, "%s", string(this->width, ' ').c_str());
        }
    }
    
    this->current_line = this->command_line + 2;
    
    if(config->get_flag(NM_FLAG_SORT_USER) || config->get_flag(NM_FLAG_SORT_USER_DESC))
        this->sort_by_user();
    else if(config->get_flag(NM_FLAG_SORT_UPLOAD) || config->get_flag(NM_FLAG_SORT_UPLOAD_DESC))
        this->sort_by_upload();
    else if(config->get_flag(NM_FLAG_SORT_DOWNLOAD) || config->get_flag(NM_FLAG_SORT_DOWNLOAD_DESC))
        this->sort_by_download();
    else if(config->get_flag(NM_FLAG_SORT_SUM_UPLOAD) || config->get_flag(NM_FLAG_SORT_SUM_UPLOAD_DESC))
        this->sort_by_total_upload();
    else if(config->get_flag(NM_FLAG_SORT_SUM_DOWNLOAD) || config->get_flag(NM_FLAG_SORT_SUM_DOWNLOAD_DESC))
        this->sort_by_total_download();
    else
        this->sort_by_program();

    
    for(int i = 0;  i < this->net_usage.size(); i++)
    {
        this->net_usage[i].download = 0.0;
        this->net_usage[i].upload = 0.0;
        this->net_usage[i].current_size_type = NM_b;
    }
    
    refresh();
    clear();
    
    debug->info("end tick");
}

void NM_ui::disable_flags(string skip)
{
    NM_debug * debug = NM_debug::get_instance("NM_ui::disable_flags");
    debug->info("Start disable flags");
    NM_config * config = NM_config::get_config();
    
    if(skip.compare(NM_FLAG_SORT_PROGRAM) != 0)
    {
        config->set_flag(NM_FLAG_SORT_PROGRAM, false);
        config->set_flag(NM_FLAG_SORT_PROGRAM_DESC, false);
    }
    if(skip.compare(NM_FLAG_SORT_USER) != 0)
    {
        config->set_flag(NM_FLAG_SORT_USER, false);
        config->set_flag(NM_FLAG_SORT_USER_DESC, false);
    }
    if(skip.compare(NM_FLAG_SORT_DOWNLOAD) != 0)
    {
        config->set_flag(NM_FLAG_SORT_DOWNLOAD, false);
        config->set_flag(NM_FLAG_SORT_DOWNLOAD_DESC, false);
    }
    if(skip.compare(NM_FLAG_SORT_SUM_DOWNLOAD) != 0)
    {
        config->set_flag(NM_FLAG_SORT_SUM_DOWNLOAD, false);
        config->set_flag(NM_FLAG_SORT_SUM_DOWNLOAD_DESC, false);
    }
    if(skip.compare(NM_FLAG_SORT_UPLOAD) != 0)
    {
        config->set_flag(NM_FLAG_SORT_UPLOAD, false);
        config->set_flag(NM_FLAG_SORT_UPLOAD_DESC, false);
    }
    if(skip.compare(NM_FLAG_SORT_SUM_UPLOAD) != 0)
    {
        config->set_flag(NM_FLAG_SORT_SUM_UPLOAD, false);
        config->set_flag(NM_FLAG_SORT_SUM_UPLOAD_DESC, false);
    }
    
    debug->info("End disable flags");
}

void NM_ui::sort_by_program()
{
    NM_debug * debug = NM_debug::get_instance("NM_ui::sort_by_program");
    debug->info("Start sorting by program");
    NM_ui_net_usage_list list = this->net_usage, result;
    int count = list.size();
    bool asc = NM_config::get_config()->get_flag(NM_FLAG_SORT_PROGRAM);
    
    for(int j = 0; j < count ; j++)
    {
        int actual = 0;
        
        for(int i = 1;  i < list.size(); i++)
        {
            if(asc)
            {
                if(list[actual].program_name.compare(list[i].program_name) > 0)
                {
                    actual = i;
                }
            }
            else
            {
                if(list[actual].program_name.compare(list[i].program_name) < 0)
                {
                    actual = i;
                }
            }
        }
        
        this->add_line(list[actual].program_name, list[actual].user_name, list[actual].upload, list[actual].download, NM_size_type_to_string(list[actual].current_size_type), list[actual].sum_upload, list[actual].sum_download, NM_size_type_to_string(list[actual].sum_size_type));
        result.push_back(list[actual]);
        list.erase(list.begin() + actual);
    }
    
    this->net_usage.clear();
    this->net_usage = result;
    result.clear();
    
    debug->info("End sorting by program");
}
void NM_ui::sort_by_user()
{
    NM_debug * debug = NM_debug::get_instance("NM_ui::sort_by_user");
    debug->info("Start sorting by user");
    NM_ui_net_usage_list list = this->net_usage, result;
    int count = list.size();
    bool asc = NM_config::get_config()->get_flag(NM_FLAG_SORT_USER);
    
    for(int j = 0; j < count ; j++)
    {
        int actual = 0;
        
        for(int i = 1;  i < list.size(); i++)
        {
            if(asc)
            {
                if(list[actual].user_name.compare(list[i].user_name) > 0)
                {
                    actual = i;
                }
            }
            else
            {
                if(list[actual].user_name.compare(list[i].user_name) < 0)
                {
                    actual = i;
                }
            }
        }
        
        this->add_line(list[actual].program_name, list[actual].user_name, list[actual].upload, list[actual].download, NM_size_type_to_string(list[actual].current_size_type), list[actual].sum_upload, list[actual].sum_download, NM_size_type_to_string(list[actual].sum_size_type));
        result.push_back(list[actual]);
        list.erase(list.begin() + actual);
    }
    
    this->net_usage.clear();
    this->net_usage = result;
    result.clear();
    
    debug->info("End sorting by user");
}
void NM_ui::sort_by_upload()
{
    NM_debug * debug = NM_debug::get_instance("NM_ui::sort_by_upload");
    debug->info("Start sorting by upload");
    NM_ui_net_usage_list list = this->net_usage, result;
    int count = list.size();
    bool asc = NM_config::get_config()->get_flag(NM_FLAG_SORT_UPLOAD);
    
    for(int j = 0; j < count ; j++)
    {
        int actual = 0;
        
        for(int i = 1;  i < list.size(); i++)
        {
            if(asc)
            {
                if(list[actual].current_size_type < list[i].current_size_type)
                    actual = i;
                else if(list[actual].current_size_type == list[i].current_size_type)
                {
                    if(list[actual].upload < list[i].upload)
                        actual = i;
                }
            }
            else
            {
                if(list[actual].current_size_type > list[i].current_size_type)
                    actual = i;
                else if(list[actual].current_size_type == list[i].current_size_type)
                {
                    if(list[actual].upload > list[i].upload)
                        actual = i;
                }
            }
        }
        
        this->add_line(list[actual].program_name, list[actual].user_name, list[actual].upload, list[actual].download, NM_size_type_to_string(list[actual].current_size_type), list[actual].sum_upload, list[actual].sum_download, NM_size_type_to_string(list[actual].sum_size_type));
        result.push_back(list[actual]);
        list.erase(list.begin() + actual);
    }
    
    this->net_usage.clear();
    this->net_usage = result;
    result.clear();
    
    debug->info("End sorting by upload");
}
void NM_ui::sort_by_download()
{
    NM_debug * debug = NM_debug::get_instance("NM_ui::sort_by_download");
    debug->info("Start sorting by download");
    NM_ui_net_usage_list list = this->net_usage, result;
    int count = list.size();
    bool asc = NM_config::get_config()->get_flag(NM_FLAG_SORT_DOWNLOAD);
    
    for(int j = 0; j < count ; j++)
    {
        int actual = 0;
        
        for(int i = 1;  i < list.size(); i++)
        {
            if(asc)
            {
                if(list[actual].current_size_type < list[i].current_size_type)
                    actual = i;
                else if(list[actual].current_size_type == list[i].current_size_type)
                {
                    if(list[actual].download < list[i].download)
                        actual = i;
                }
            }
            else
            {
                if(list[actual].current_size_type > list[i].current_size_type)
                    actual = i;
                else if(list[actual].current_size_type == list[i].current_size_type)
                {
                    if(list[actual].download > list[i].download)
                        actual = i;
                }
            }
        }
        
        this->add_line(list[actual].program_name, list[actual].user_name, list[actual].upload, list[actual].download, NM_size_type_to_string(list[actual].current_size_type), list[actual].sum_upload, list[actual].sum_download, NM_size_type_to_string(list[actual].sum_size_type));
        result.push_back(list[actual]);
        list.erase(list.begin() + actual);
    }
    
    this->net_usage.clear();
    this->net_usage = result;
    result.clear();
    
    debug->info("End sort by download");
}
void NM_ui::sort_by_total_upload()
{
    NM_debug * debug = NM_debug::get_instance("NM_ui::sort_by_total_upload");
    debug->info("Start sorting by total upload");
    NM_ui_net_usage_list list = this->net_usage, result;
    int count = list.size();
    bool asc = NM_config::get_config()->get_flag(NM_FLAG_SORT_SUM_UPLOAD);
    
    for(int j = 0; j < count ; j++)
    {
        int actual = 0;
        
        for(int i = 1;  i < list.size(); i++)
        {
            if(asc)
            {
                if(list[actual].sum_size_type < list[i].sum_size_type)
                    actual = i;
                else if(list[actual].sum_size_type == list[i].sum_size_type)
                {
                    if(list[actual].sum_upload < list[i].sum_upload)
                        actual = i;
                }
            }
            else
            {
                if(list[actual].sum_size_type > list[i].sum_size_type)
                    actual = i;
                else if(list[actual].sum_size_type == list[i].sum_size_type)
                {
                    if(list[actual].sum_upload > list[i].sum_upload)
                        actual = i;
                }
            }
        }
        
        this->add_line(list[actual].program_name, list[actual].user_name, list[actual].upload, list[actual].download, NM_size_type_to_string(list[actual].current_size_type), list[actual].sum_upload, list[actual].sum_download, NM_size_type_to_string(list[actual].sum_size_type));
        result.push_back(list[actual]);
        list.erase(list.begin() + actual);
    }
    
    this->net_usage.clear();
    this->net_usage = result;
    result.clear();
    
    debug->info("End sorting by total upload");
}
void NM_ui::sort_by_total_download()
{
    NM_debug * debug = NM_debug::get_instance("NM_ui::sort_by_total_download");
    debug->info("Start sorting by total download");
    NM_ui_net_usage_list list = this->net_usage, result;
    int count = list.size();
    bool asc = NM_config::get_config()->get_flag(NM_FLAG_SORT_SUM_DOWNLOAD);
    
    for(int j = 0; j < count ; j++)
    {
        int actual = 0;
        
        for(int i = 1;  i < list.size(); i++)
        {
            if(asc)
            {
                if(list[actual].sum_size_type < list[i].sum_size_type)
                    actual = i;
                else if(list[actual].sum_size_type == list[i].sum_size_type)
                {
                    if(list[actual].sum_download < list[i].sum_download)
                        actual = i;
                }
            }
            else
            {
                if(list[actual].sum_size_type > list[i].sum_size_type)
                    actual = i;
                else if(list[actual].sum_size_type == list[i].sum_size_type)
                {
                    if(list[actual].sum_download > list[i].sum_download)
                        actual = i;
                }
            }
        }
        
        this->add_line(list[actual].program_name, list[actual].user_name, list[actual].upload, list[actual].download, NM_size_type_to_string(list[actual].current_size_type), list[actual].sum_upload, list[actual].sum_download, NM_size_type_to_string(list[actual].sum_size_type));
        result.push_back(list[actual]);
        list.erase(list.begin() + actual);
    }
    
    this->net_usage.clear();
    this->net_usage = result;
    result.clear();
    
    debug->info("End sorting by total download");
}
