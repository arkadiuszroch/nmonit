#include "connection.hpp"

using namespace std;

NM_connection::NM_connection()
{
    this->local_ip = ip_to_in_addr("0.0.0.0");
    this->local_port = 0;

    this->rem_ip = ip_to_in_addr("0.0.0.0");
    this->rem_port = 0;

    this->inode_set = false;
    
    this->remote = NULL;
    
    if( pthread_mutex_init(&this->mutex, NULL) != 0 )
        throw NM_exception(NM_ex_code_mutex_init_filed, "Couldn't init mutex");

    this->reset_size();
    this->reset_counter();
}

NM_connection::NM_connection(in_addr local_ip, unsigned int local_port, in_addr rem_ip, unsigned int rem_port, NM_inode_list::iterator inode, NM_ip_type ip_type, NM_protocol_type protocol_type)
{
    this->local_ip = local_ip;
    this->local_port = local_port;

    this->rem_ip = rem_ip;
    this->rem_port = rem_port;

    this->inode = inode;
    this->inode_set = true;
    this->monitored = true;
    if(this->inode->first > 0)
    {
        if(this->inode->second.program->is_monitored() == NM_monitored_permanent 
                || this->inode->second.user->is_monitored() == NM_monitored_permanent
                || this->inode->second.device->is_monitored() == NM_monitored_permanent)
            this->monitored = true;
        else if(!this->inode->second.program->is_monitored() || !this->inode->second.user->is_monitored() || !this->inode->second.device->is_monitored())
            this->monitored = false;
    }
    
    this->remote = NULL;

    if( pthread_mutex_init(&this->mutex, NULL) != 0 )
        throw NM_exception(NM_ex_code_mutex_init_filed, "Couldn't init mutex");

    this->reset_size();
    this->reset_counter();
    
    this->ip_type = ip_type;
    this->protocol_type = protocol_type;
}

NM_connection::~NM_connection()
{
    NM_debug * debug = NM_debug::get_instance("NM_connection::destructor");
    /* @TODO
     * Sprawdzić czy dwa lub więcej połączeń mogą prowadzić co tego samego gniazda (wstępne rozeznanie wskazuje na to, że tak).
     * Na chwile obecną jeden inode jest przypisany do dwóch lub więcej połączeń i usunięcie go
     * powoduje naruszenie ochrony pamięci w przypadku kasowania innego połączenia z tym inodem.
     */
    /*debug->info("Lock connection mutex");
    this->lock();
    debug->info("Check that connection has inode");
    if( this->inode_set )
    {
        debug->info("Connection has inode so we delete it");
        debug->info("Inode number");
        debug->info(this->inode->first);
        
        pthread_mutex_lock(&NM_mutex_remap_inodes);
        ::NM_inodes.erase(this->inode);
        pthread_mutex_unlock(&NM_mutex_remap_inodes);
        this->inode_set = false;
        debug->info("Inode deleted");
    }
    debug->info("Unlock mutex");
    this->unlock();*/
    debug->info("Destroing mutex");
    pthread_mutex_destroy(&this->mutex);
}

void NM_connection::add_tx_size(int size)
{
    this->add_size(size, false);
}

void NM_connection::add_rx_size(int size)
{
    this->add_size(size, true);
}

void NM_connection::add_size(int add, bool rx)
{
    if(!this->monitored)
        return;
    // @TODO sprawdzić czy faktycznie będzie podawany int bez koniecznosci rzutowania. 
    // jesli nie zmienic parametr na double i usunac ponizsze rzutowanie
    NM_double bufor = static_cast<NM_double> (add);
    NM_size_type size_type;
    NM_double size = 0.0;

    this->lock();

    if( rx )
    {
        size_type = this->rx_size_type;
        size = this->rx_size;
    }
    else
    {
        size_type = this->tx_size_type;
        size = this->tx_size;
    }

    switch( size_type )
    {
        case NM_tb:
            bufor /= 1024;
        case NM_gb:
            bufor /= 1024;
        case NM_mb:
            bufor /= 1024;
        case NM_kb:
            bufor /= 1024;

    }

    size += bufor;

    if( size > 1024 && size_type < NM_tb )
    {
        size /= 1024;
        switch( size_type )
        {
            case NM_gb:
                size_type = NM_tb;
                break;
            case NM_mb:
                size_type = NM_gb;
                break;
            case NM_kb:
                size_type = NM_mb;
                break;
            case NM_b:
                size_type = NM_kb;
                break;
        }
    }

    if( rx )
    {
        this->rx_size_type = size_type;
        this->rx_size = size;
    }
    else
    {
        this->tx_size_type = size_type;
        this->tx_size = size;
    }

    this->unlock();
}

void NM_connection::set_local_ip(in_addr local_ip)
{
    this->lock();
    this->local_ip = local_ip;
    this->unlock();
}

void NM_connection::set_local_port(unsigned int local_port)
{
    this->lock();
    this->local_port = local_port;
    this->unlock();
}

void NM_connection::set_rem_ip(in_addr rem_ip)
{
    this->lock();
    this->rem_ip = rem_ip;
    this->unlock();
}

void NM_connection::set_rem_port(unsigned int rem_port)
{
    this->lock();
    this->rem_port = rem_port;
    this->unlock();
}

void NM_connection::set_inode(NM_inode_list::iterator inode)
{
    this->lock();
    this->inode_set = ( inode->first != 0 );
    this->inode = inode;
    
    if(this->inode->first > 0)
    {
        this->monitored = true;
        if(!inode->second.program->is_monitored() || !inode->second.user->is_monitored())
            this->monitored = false;
    }
    
    this->unlock();
}

in_addr NM_connection::get_local_ip()
{
    this->lock();
    in_addr ip = this->local_ip;
    this->unlock();
    return ip;
}

unsigned int NM_connection::get_local_port()
{
    this->lock();
    unsigned int port = this->local_port;
    this->unlock();
    return port;
}

in_addr NM_connection::get_rem_ip()
{
    this->lock();
    in_addr ip = this->rem_ip;
    this->unlock();
    return ip;
}

unsigned int NM_connection::get_rem_port()
{
    this->lock();
    unsigned int port = this->rem_port;
    this->unlock();
    return port;
}

NM_inode_list::iterator NM_connection::get_inode()
{
    this->lock();
    NM_inode_list::iterator it = this->inode;
    this->unlock();
    return it;
}

bool NM_connection::has_inode()
{
    this->lock();
    bool inode = this->inode_set;
    this->unlock();
    return inode;
}

NM_size_type NM_connection::get_tx_size_type()
{
    this->lock();
    NM_size_type type = this->tx_size_type;
    this->unlock();
    return type;
}

NM_double NM_connection::get_tx_size()
{
    this->lock();
    NM_double size = this->tx_size;
    this->unlock();
    return size;
}

NM_size_type NM_connection::get_rx_size_type()
{
    this->lock();
    NM_size_type type = this->rx_size_type;
    this->unlock();
    return type;
}

NM_double NM_connection::get_rx_size()
{
    this->lock();
    NM_double size = this->rx_size;
    this->unlock();
    return size;
}

NM_ip_type NM_connection::get_ip_type()
{
    return this->ip_type;
}

bool NM_connection::set_remote(NM_remote * remote)
{
    if(this->remote != NULL)
        return false;
    this->remote = remote;
    return true;
}

NM_remote * NM_connection::get_remote()
{
    return this->remote;
}

NM_protocol_type NM_connection::get_protocol_type()
{
    return this->protocol_type;
}

void NM_connection::reset_size()
{
    this->lock();
    this->tx_size_type = this->rx_size_type = NM_b;
    this->tx_size = this->rx_size = 0;
    this->unlock();
}

void NM_connection::lock()
{
    pthread_mutex_lock(&this->mutex);
}

void NM_connection::unlock()
{
    pthread_mutex_unlock(&this->mutex);
}

unsigned short int NM_connection::get_counter()
{
    unsigned short int ret;
    this->lock();
    ret = this->counter;
    this->unlock();
    return ret;
}

void NM_connection::reset_counter()
{
    this->lock();
    this->counter = 60;
    this->unlock();
}

unsigned short int NM_connection::decrement_counter()
{
    this->lock();
    if( this->counter > 0 )
        this->counter--;
    this->unlock();
    
    return this->get_counter();
}

bool NM_connection::is_monitored()
{
    return this->monitored;
}