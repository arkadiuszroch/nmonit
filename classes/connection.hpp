#ifndef NM_CONNECTION
#define NM_CONNECTION

#include "../functions/main_h.hpp"
#include "debug.hpp"

extern NM_inode_list NM_inodes;

class NM_connection
{
  private:
    unsigned short int counter;
    
    NM_ip_type ip_type;
    NM_protocol_type protocol_type;
    
    unsigned int local_port;
    in_addr local_ip;
    
    unsigned int rem_port;
    in_addr rem_ip;
    
    NM_inode_list::iterator inode;
    
    NM_remote * remote; // if inode is 0
    
    bool inode_set;
    
    NM_double tx_size, rx_size;
    NM_size_type tx_size_type, rx_size_type;
    
    pthread_mutex_t mutex;
    
    bool monitored;
  public:
    NM_connection();
    NM_connection(in_addr /*local_ip*/, unsigned int /*local_port*/, in_addr /*rem_ip*/, unsigned int /*rem_port*/, NM_inode_list::iterator /*inode*/, NM_ip_type /*ip_type*/, NM_protocol_type /*protocol_type*/);
    ~NM_connection();

    void set_local_ip(in_addr /*local_ip*/);
    void set_local_port(unsigned int /*local_port*/);

    void set_rem_ip(in_addr /*rem_ip*/);
    void set_rem_port(unsigned int /*rem_port*/);

    void set_inode(NM_inode_list::iterator /*inode*/);
    bool has_inode();

    in_addr get_local_ip();
    unsigned int get_local_port();

    in_addr get_rem_ip();
    unsigned int get_rem_port();

    NM_inode_list::iterator get_inode();

    bool set_remote(NM_remote * /*remote*/);
    NM_remote * get_remote();
    
    NM_size_type get_tx_size_type();
    NM_double get_tx_size();

    NM_size_type get_rx_size_type();
    NM_double get_rx_size();
    
    NM_ip_type get_ip_type();
    NM_protocol_type get_protocol_type();
    
    void add_size(int /*size*/, bool = true /*rx*/);

    void reset_size();
    
    void lock();
    void unlock();
    
    unsigned short int get_counter();
    void reset_counter();
    unsigned short int decrement_counter();
    
    bool is_monitored();

  protected:

    void add_tx_size(int /*size*/);
    void add_rx_size(int /*size*/);
};

#endif
