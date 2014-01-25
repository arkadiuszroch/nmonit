#ifndef NM_DEBUG
#define	NM_DEBUG

#include "../functions/global.hpp"

class NM_debug;

typedef std::map<std::string /*section_name*/, NM_debug * /*instance*/> NM_debug_list;

class NM_debug
{
  private:
    static NM_debug_list instances;
    NM_debug(std::string /*section_name*/);
    NM_debug(const NM_debug &);
    NM_debug & operator =(const NM_debug &);
    std::string section_name;
    //static NM_debug_list initialize_instances();
    std::string prepare_out(std::string /*prefix*/);
    bool active;
    static NM_stream out;
    static bool initialized;

  public:
    static NM_debug * get_instance(std::string /*section_name*/);
    ~NM_debug();
    void info(std::string /*msg*/);
    void info(const char * /*msg*/);
    void info(int /*val*/);
    void info(double /*val*/);
    void info(float /*val*/);
    void info(bool /*val*/);

    void warning(std::string /*msg*/);
    void warning(const char * /*msg*/);
    void warning(int /*val*/);
    void warning(double /*val*/);
    void warning(float /*val*/);
    void warning(bool /*val*/);
};

#endif