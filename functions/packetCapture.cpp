#include "packetCapture.hpp"

using namespace std;

void * NM_capture_packets(void * ptr)
{
    NM_debug * debug = NM_debug::get_instance("NM_capture_packets");
    NM_device * device = NULL;
    device = static_cast<NM_device *> (ptr);
    pcap_t * handle = NULL;
    struct bpf_program fp;
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 mask;
    bpf_u_int32 net;
    int device_id = device->get_device_id();

    ::NM_threads[device_id].state = NM_thread_state_running;

    pthread_cleanup_push(NM_finish_thread, &device_id);
    debug->info("Getting mask of device: " + device->get_device_name());
    if( pcap_lookupnet(device->get_device_name().c_str(), &net, &mask, errbuf) == -1 )
    {
        NM_trigger_warning("Couldn't get netmask for device " + device->get_device_name());
        net = 0;
        mask = 0;
        sleep(5);
        return 0;
    }

    debug->info("Opening device " + device->get_device_name() + " for capturing");
    handle = pcap_open_live(device->get_device_name().c_str(), SNAP_LEN, 1, 1000, errbuf);
    if( handle == NULL )
    {
        NM_trigger_warning("Couldn't open interface " + device->get_device_name() + " to sniffing: " + string(errbuf));
        sleep(5);
        return 0;
    }

    /*debug->info("Compile filter for device: " + device->get_device_name());
    if( pcap_compile(handle, &fp, "tcp", 0, net) == -1 )
    {
        NM_trigger_warning("Couldn't parse filter");
        sleep(5);
        return 0;
    }

    debug->info("Setting filter for device: " + device->get_device_name());
    if( pcap_setfilter(handle, &fp) == -1 )
    {
        NM_trigger_warning("Couldn't install filter");
        sleep(5);
        return 0;
    }
*/
    debug->info("Starting pcap loop for device: " + device->get_device_name());
    pcap_loop(handle, 0, NM_get_packet, NULL);

    debug->info("Cleaning thread pointers");
    pcap_close(handle);
    //@TODO zmiana funkcji
    pcap_freecode(&fp);
    handle = NULL;
    debug->info("Starting new thread for device: " + device->get_device_name());
    pthread_cleanup_pop(1);
}

void NM_get_packet(u_char * args, const struct pcap_pkthdr * header, const u_char * packet)
{
    NM_debug * debug = NM_debug::get_instance("NM_get_packet");
    struct NM_ethernet * ethernet = NULL;
    struct NM_header_ip * ip = NULL;
    ostringstream sstream;
    NM_protocol * protocol = NULL;
    static int i = 0;
    int size_ip = 0;

    debug->info("Start get_packet");

    debug->info("Getting ethernet struct");
    ethernet = (struct NM_ethernet *) packet;

    debug->info("Checking ether type");
    if( ntohs(ethernet->ether_type) != ETHER_TYPE_IP )
        return;

    debug->info("Getting IP struct");
    ip = (struct NM_header_ip *) (packet + SIZE_ETHERNET);

    size_ip = IP_HL(ip) * 4;

    if( size_ip < 20 )
    {
        sstream << "Invalid IP header length: " << size_ip;
        NM_trigger_warning(sstream.str());
        return;
    }
    
    if(ip == NULL)
    {
        debug->info("IP header is NULL");
        NM_bug("IP header is NULL");
        return;
    }

    debug->info("Checking ip protocol");
    switch( ip->ip_p )
    {
        case IPPROTO_TCP:
            debug->info("TCP protocol");
            protocol = new NM_protocolTcp((struct NM_header_tcp *) (packet + SIZE_ETHERNET + size_ip), ip->ip_src, ip->ip_dst, ip->ip_len, size_ip);
            break;
        case IPPROTO_UDP:
            debug->info("UDP protocol");

            //protocol = new NM_protocolUdp((struct NM_header_udp *)(packet + SIZE_ETHERNET + size_ip), ip->ip_src, ip->ip_dst);
            //break;
            return;
        case IPPROTO_ICMP: 
            debug->info("ICMP protocol");
            return;
        case IPPROTO_IP:
            debug->info("IP protocol");
            return;
        default:
            debug->info("unknown protocol");
            return;
    }

    sstream.str("");

    sstream << "Zlapalem pakiet : " << endl;
    sstream << "Adres lokalny: " << ip_to_string(protocol->get_local_ip());
    sstream << ":" << protocol->get_local_port() << endl;
    sstream << "Adres lokalny: " << ip_to_string(protocol->get_rem_ip());
    sstream << ":" << protocol->get_rem_port() << endl;
    sstream << "Rozmiar pakietu: " << protocol->get_size() << endl;

    debug->info(sstream.str());

    debug->info("cleaning");
    delete protocol; 
    debug->info("cleaned");
}

void NM_finish_thread(void * ptr)
{
    NM_debug * debug = NM_debug::get_instance("NM_finish_thread");
    pthread_attr_t attr;
    pthread_t thread_id;

    debug->info("Starting function finish thread");
    int * int_ptr = NULL;
    int_ptr = static_cast<int *> (ptr);

    int device_id = *int_ptr;
    debug->info("Changing state of thread to stopped");
    ::NM_threads[device_id].state = NM_thread_state_stopped;

    debug->info("Creating new thread");
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_id, &attr, NM_capture_packets, NM_deviceList::get_instance()->find_by_id(device_id));

    debug->info("Changing state of thread to running");
    ::NM_threads[device_id].thread = thread_id;
    ::NM_threads[device_id].state = NM_thread_state_running;
}
