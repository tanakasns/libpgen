
#include "pgen.h"
#include "packet.h"
#include "address.h"

#include <map>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/ethernet.h> 

#include "netutil.h"



pgen_eth::pgen_eth(){
	pgen_packet::CLEAR();
	CLEAR();	
}

void pgen_eth::CLEAR(){
	ETH.src = 0;
	ETH.dst = 0;
	ETH.type = htons(0);
}

void pgen_eth::SEND(const char* ifname){
	WRAP(ifname);		
	int sock;

	if((sock=initRawSocket(ifname, 2)) < 0){
		exit(PGEN_ERROR);
	}
	if(sendRawPacket(sock, data, len, 2, NULL) < 0)
		exit(PGEN_ERROR);

	close(sock);
}



void pgen_eth::WRAP(const char* ifname){
	packetType = PGEN_PACKETTYPE_ETH;
	memset(data, 0, sizeof data);
	eth.ether_type = htons(0);

	for(int i=0; i< 6; i++){
		eth.ether_shost[i] = ETH.src._addr[i];	
		eth.ether_dhost[i] = ETH.dst._addr[i];	
	}

	u_char* p = data;
	memcpy(data, &eth, sizeof eth);
	p += sizeof(eth);
	len = p - data;
}



void pgen_eth::INFO(){
	WRAP("no use");
	std::map<int , const char*> _ethtype;
	_ethtype[0x0800] = "IPv4";
	_ethtype[0x0806] = "ARP";
	_ethtype[0x8035] = "RARP";
	_ethtype[0x8191] = "NetBios";
	_ethtype[0x86dd] = "IPv6";

	printf(" * Ethernet  %s -> %s \n", ETH.src.bender(), ETH.dst.bender());
	printf("    - Destination     :  %s (%s)   \n", 
			ETH.dst.c_str(), ETH.dst.bender());
	printf("    - Source          :  %s (%s)   \n" , 
			ETH.src.c_str(), ETH.src.bender());
	printf("    - Type            :  %s  (0x%04x)   \n", 
			_ethtype[htons(eth.ether_type)] ,htons(eth.ether_type));
}

