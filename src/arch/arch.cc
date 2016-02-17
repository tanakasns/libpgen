






#include <pgen2/arch/arch.h>
#include <pgen2/exception.h>
#include <pgen2/util.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ifaddrs.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <iostream> 
#include <exception>

#ifdef __PGEN_MAXOS
#include <net/if_dl.h>
#include <net/bpf.h>
#include <fcntl.h>
#endif

#ifdef __PGEN_LINUX
#endif


namespace pgen {
namespace arch {



void getmacbydev(const char* dev, uint8_t mac[6]) {
#ifdef __linux
    if(strlen(dev) >= IFNAMSIZ) {
        throw pgen::exception("pgen::arch::getmacbydev: Interface name size is too large");
    }

    int sockd;
    struct ifreq ifr;
    if ((sockd=socket(AF_INET,SOCK_DGRAM,0)) < 0){
        throw pgen::exception("pgen::arch::getmacbydev:socket: " + strerror(errno));
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strcpy(ifr.ifr_name, dev);
    if(ioctl(sockd, SIOCGIFHWADDR, &ifr) < 0){
        close(sockd);
        throw pgen::exception("pgen::arch::getmacbydev:ioctl: " + strerror(errno));
    }
    close(sockd);
    memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
    
    return ; //success


#else // for bsd
    
    struct ifaddrs *ifap, *ifaptr;
    unsigned char *ptr;
    if(getifaddrs(&ifap) != 0){
        throw pgen::exception("pgen::arch::getmacbydev:getifaddrs: undefined error");
    }
    for(ifaptr = ifap; ifaptr != NULL; ifaptr = (ifaptr)->ifa_next) {
        if (!strcmp((ifaptr)->ifa_name, dev) && 
                (((ifaptr)->ifa_addr)->sa_family == AF_LINK)) {
            ptr = (unsigned char *)LLADDR((struct sockaddr_dl *)(ifaptr)->ifa_addr);
            memcpy(mac, ptr, 6);
            freeifaddrs(ifap);
            return; // success
        }
    }
    

#endif
    throw pgen::exception("pgen::arch::getmacbydev: Unknown error");
}

 

void getipv4bydev(const char* dev, uint8_t ip[4]) {
    if(strlen(dev) >= IFNAMSIZ) {
        throw pgen::exception("pgen::arch::getipv4bydev: Interface name size is too large");
    }

    int sockd;
    struct ifreq ifr;
    struct sockaddr_in *sa;

    if ((sockd=socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        std::string errmsg = "pgen::arch::getipv4bydev:socket: ";
        errmsg += strerror(errno);
        throw pgen::exception(errmsg);
    }
    ifr.ifr_addr.sa_family = AF_INET;
    strcpy(ifr.ifr_name, dev);
    if(ioctl(sockd, SIOCGIFADDR, &ifr) < 0){
        close(sockd);
        std::string errmsg = "pgen::arch::getipv4bydev:ioctl: ";
        errmsg += strerror(errno);
        throw pgen::exception(errmsg);
    }
    close(sockd);
    sa = (struct sockaddr_in*)&ifr.ifr_addr;
    memcpy(ip, &(sa->sin_addr.s_addr), sizeof(uint32_t));
}


/* 
 * Thanks 
 *  http://d.hatena.ne.jp/shammer/20101212/p1 
 */
void getipv6bydev(const char* dev, uint16_t ip[8]) {
    if (strlen(dev) >= IFNAMSIZ) {
        throw pgen::exception("pgen::arch::getipv6bydev: Interface name size is too large");
    }

    struct ifaddrs * if_list;
    void * tmp;

    getifaddrs(&if_list);
    for (struct ifaddrs* ifa = if_list ; ifa != NULL ; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, dev) == 0) { 
            if (!ifa->ifa_addr) {
                continue;
            } else {
                if (ifa->ifa_addr->sa_family == AF_INET6) { 
                    tmp = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
                    memcpy(ip, tmp, sizeof(struct in6_addr));
                    freeifaddrs(if_list);
                    return ;
                }
                else {
                    // For example, reach here the cases
                    // sa_family is AF_PACKET(17).
                    // sa_family is AF_INET6(23).
                }
            }
        }
    }
    freeifaddrs(if_list);
    throw pgen::exception("pgen::getipv6bydev: can't get address");
}


} /* namespace arch */
} /* namespace pgen */




