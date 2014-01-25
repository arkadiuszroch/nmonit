#include "udp.hpp"

using namespace std;

NM_protocolUdp::NM_protocolUdp()
{
    NM_debug * debug = NM_debug::get_instance("UDP Protocol - Constructor");
    debug->info("New instance of UDP class without parameters");

    this->init();
}

NM_protocolUdp::NM_protocolUdp(NM_header_udp * packet, in_addr source_ip, in_addr dest_ip, int packet_size, int ip_size)
{
    NM_debug * debug = NM_debug::get_instance("UDP Protocol - Constructor");

    debug->info("New instance of UDP class with parameters");
    string key;
    int udp_size;
    bool rx = true;
    NM_device * dev = NULL;

    udp_size = packet->uh_length + sizeof(NM_header_udp);

    dev = NM_deviceList::get_instance()->find_by_ip(source_ip);
    debug->info("Getting device");
    if( dev != NULL )
    {
        debug->info("Device is not null: " + dev->get_device_name() + ". Source addres is local address");
        this->local_ip = source_ip;
        this->local_port = ntohs(packet->uh_sport);

        this->rem_ip = dest_ip;
        this->rem_port = ntohs(packet->uh_dport);

        rx = false;
    }
    else
    {
        dev = NM_deviceList::get_instance()->find_by_ip(ip_to_string(dest_ip));
        debug->info("Device is null so local address is destination address");
        this->local_ip = dest_ip;
        this->local_port = ntohs(packet->uh_dport);

        this->rem_ip = source_ip;
        this->rem_port = ntohs(packet->uh_sport);

        rx = true;
    }

    debug->info("checking that device is monitored");
    // if device isn't monitored the thread not started but for secure we check it
    if( !dev->is_monitored() )
    {
        debug->info("device isn't monitored so we exit");
        return;
    }

    debug->info("Getting size of packet");
    this->size = ntohs(packet_size) - (ip_size + udp_size);

    debug->info("Generate key for this packet");
    key = this->generate_list_key(ip_to_string(this->local_ip), this->local_port, ip_to_string(this->rem_ip), this->rem_port);
    debug->info("Generated key: " + key);
    debug->info("Checking wheather connection with that key exists");
    
    pthread_mutex_lock(&NM_mutex_conn_list);
    
    if( ::NM_connections_v4.count(key) == 0 )
    {
        debug->info("Connection doesn't exist so we scan the tcp connections");
        pthread_mutex_unlock(&NM_mutex_conn_list);
        
        try
        {
            this->load_connections();
        }
        catch( NM_exception e )
        {
            NM_trigger_warning("Blad ladowania polaczen TCP: " + e.get_message());
        }
        
        pthread_mutex_lock(&NM_mutex_conn_list);
        debug->info("Checking that the connection now exist");
        if( ::NM_connections_v4.count(key) == 0 )
        {
            debug->info("Connection not exists so we display info about this bug");
            NM_bug("Connection was closed before we get information about it. Well, it happen so.");
            ::NM_connections_v4[key] = new NM_connection(this->local_ip, this->local_port, this->rem_ip, this->rem_port, ::NM_inodes.find(0), NM_ip_v4, NM_protocol_udp);
        }
    }
    debug->info("Connection exists so we add size of this packet");
    
    ::NM_connections_v4[key]->reset_counter();
    ::NM_connections_v4[key]->add_size(this->size + udp_size + ip_size, rx);
    pthread_mutex_unlock(&NM_mutex_conn_list);
    
}

void NM_protocolUdp::load_connections()
{
    NM_debug * debug = NM_debug::get_instance("UDP Protocol - Loading connections");

    debug->info("Start loading connections");
    fstream file("/proc/net/udp", ios::in);
    string line;

    if( !file.good() )
        throw NM_exception(NM_ex_code_file_perm_denied, "Unable to open /proc/net/udp file");

    getline(file, line); // ingoring first line

    debug->info("Loading UDP connections - starting main loop");
    while( getline(file, line) )
    {
        unsigned int a, b, c, d, local_port, rem_port;
        long int inode;
        char hex_local_ip[128], hex_rem_ip[128];
        struct in_addr local_ip, rem_ip;
        string str_local_ip, str_rem_ip, key;
        ostringstream sstream;
        NM_device * device = NULL;

        debug->info("Scanign line for search ips, ports and inode");
        int matches = sscanf(line.c_str(), "%*d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %*X %*X:%*X %*X:%*X %*X %*d %*d %ld %*512s\n",
                hex_local_ip, &local_port, hex_rem_ip, &rem_port, &inode);

        if( matches != 5 )
            continue;

        sscanf(hex_local_ip, "%2x%2x%2x%2x", &d, &c, &b, &a);

        sstream << a << "." << b << "." << c << "." << d;
        str_local_ip = sstream.str();
        sstream.str("");

        device = NM_deviceList::get_instance()->find_by_ip(str_local_ip);
        if( device == NULL || !device->is_monitored() )
            continue;
        if( ::NM_inodes.count(inode) == 0 )
            NM_remap_inodes();

        ::NM_inodes[inode].device = device;

        sscanf(hex_rem_ip, "%2x%2x%2x%2x", &d, &c, &b, &a);

        sstream << a << "." << b << "." << c << "." << d;
        str_rem_ip = sstream.str();
        sstream.str("");

        local_ip = ip_to_in_addr(str_local_ip);
        rem_ip = ip_to_in_addr(str_rem_ip);

        key = this->generate_list_key(str_local_ip, local_port, str_rem_ip, rem_port);

        if( ::NM_connections_v4.count(key) == 0 )
        {
            try
            {
                pthread_mutex_lock(&::NM_mutex_conn_list);
                ::NM_connections_v4[key] = new NM_connection(local_ip, local_port, rem_ip, rem_port, ::NM_inodes.find(inode), NM_ip_v4, NM_protocol_udp);
                pthread_mutex_unlock(&::NM_mutex_conn_list);
            }
            catch( NM_exception e )
            {
                pthread_mutex_unlock(&::NM_mutex_conn_list);
                NM_trigger_error("Could not create new connection: " + e.get_message());
            }
        }
        else
        {

            if( !NM_connections_v4[key]->has_inode() || ::NM_connections_v4[key]->get_inode()->first != inode );
            ::NM_connections_v4[key]->set_inode(::NM_inodes.find(inode));
        }
    }

    debug->info("Loading TCP connections finished");
}

string NM_protocolUdp::generate_list_key(string local_ip, unsigned int local_port, string rem_ip, unsigned int rem_port)
{
    ostringstream sstream;

    sstream << "udp_v4|" << local_ip << ":" << local_port << "|" << rem_ip << ":" << rem_port;

    return sstream.str();
}

