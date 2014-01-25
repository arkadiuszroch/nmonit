#ifndef NM_UI
#define	NM_UI

#include <iomanip> // std::precision
#include <cmath> // std::pow
#include <ncurses.h>
#include "../functions/global_h.hpp"
#include "config.hpp"
#include "debug.hpp"

#define NM_UI_SORT_PROGRAM 'p'//80
#define NM_UI_SORT_PROGRAM_C 'P'
#define NM_UI_SORT_USER 'u'
#define NM_UI_SORT_USER_C 'U'
#define NM_UI_SORT_DOWNLOAD 'd'
#define NM_UI_SORT_DOWNLOAD_C 'D'
#define NM_UI_SORT_UPLOAD 'u'
#define NM_UI_SORT_UPLOAD_C 'U'
#define NM_UI_SORT_SUM_DOWNLOAD 'n'
#define NM_UI_SORT_SUM_DOWNLOAD_C 'N'
#define NM_UI_SORT_SUM_UPLOAD 'm'
#define NM_UI_SORT_SUM_UPLOAD_C 'M'
#define NM_UI_QUIT 'q'
#define NM_UI_QUIT_C 'Q'

// sort flags
#define NM_FLAG_SORT_PROGRAM "program"
#define NM_FLAG_SORT_PROGRAM_DESC "program-desc"
#define NM_FLAG_SORT_USER "user"
#define NM_FLAG_SORT_USER_DESC "user-desc"
#define NM_FLAG_SORT_DOWNLOAD "download"
#define NM_FLAG_SORT_DOWNLOAD_DESC "download-desc"
#define NM_FLAG_SORT_UPLOAD "upload"
#define NM_FLAG_SORT_UPLOAD_DESC "upload-desc"
#define NM_FLAG_SORT_SUM_DOWNLOAD "sum_download"
#define NM_FLAG_SORT_SUM_DOWNLOAD_DESC "sum_download-desc"
#define NM_FLAG_SORT_SUM_UPLOAD "sum_upload"
#define NM_FLAG_SORT_SUM_UPLOAD_DESC "sum_upload-desc"

// fields size
#define NM_UI_SIZE_MIN 70
#define NM_UI_SIZE_PROGRAM 14
#define NM_UI_RATIO_PROGRAM NM_UI_SIZE_PROGRAM / NM_UI_SIZE_MIN
#define NM_UI_SIZE_USER 13
#define NM_UI_RATIO_USER NM_UI_SIZE_USER / NM_UI_SIZE_MIN
#define NM_UI_SIZE_UPLOAD 26
#define NM_UI_RATIO_UPLOAD NM_UI_SIZE_UPLOAD / NM_UI_SIZE_MIN
#define NM_UI_SIZE_DOWNLOAD 26
#define NM_UI_RATIO_DOWNLOAD NM_UI_SIZE_DOWNLOAD / NM_UI_SIZE_MIN

// color pairs
#define NM_UI_COLOR_TITLE 1
#define NM_UI_COLOR_PROGRAM 2
#define NM_UI_COLOR_USER 3
#define NM_UI_COLOR_UPLOAD 4
#define NM_UI_COLOR_SUM_UPLOAD 5
#define NM_UI_COLOR_DOWNLOAD 6
#define NM_UI_COLOR_SUM_DOWNLOAD 7

std::string NM_size_type_to_string(NM_size_type);

struct NM_ui_net_usage
{
    std::string program_name;
    std::string user_name;
    double upload;
    double sum_upload;
    double download;
    double sum_download;
    NM_size_type current_size_type;
    NM_size_type sum_size_type;
};

typedef std::vector<NM_ui_net_usage /*net_usage*/> NM_ui_net_usage_list ;

class NM_ui
{
  private:
    unsigned short int  width,
                        height,
                        s_middle,
                        // field width
                        w_program_title,
                        w_program_content,
                        w_user_title,
                        w_user_content,
                        w_upload_title,
                        w_upload_content,
                        w_upload_actual_content,
                        w_upload_sum_content,
                        w_download_title,
                        w_download_content,
                        w_download_actual_content,
                        w_download_sum_content,
                        
                        // field start positions
                        s_program_title,
                        s_program_content,
                        s_user_title,
                        s_user_content,
                        s_upload_title,
                        s_upload_content,
                        s_upload_sum_content,
                        s_download_title,
                        s_download_content,
                        s_download_sum_content,
                        command_line,
                        current_line;
    NM_ui_net_usage_list net_usage;
    static NM_ui * instance;
    
    
  public:
    static NM_ui * get_instance();
    ~NM_ui();
    void add(std::string /*program_name*/, std::string /*user_name*/, double /*upload*/, double /*download*/, NM_size_type /*size_type*/);
    void tick();
    static bool initialized();
    
  private: 
    NM_ui();
    NM_ui(const NM_ui &);
    NM_ui & operator =(const NM_ui &);
    
    void init();
    
    bool add_line(std::string /*program_name*/, std::string /*user_name*/, double /*upload*/, double /*download*/, std::string /*download_type*/, double /*sum_upload*/, double /*sum_download*/, std::string /*sum_size_type*/);
    
    void disable_flags(std::string /*skip*/);
    
    void sort_by_program();
    void sort_by_user();
    void sort_by_upload();
    void sort_by_download();
    void sort_by_total_upload();
    void sort_by_total_download();
    
};

#endif
