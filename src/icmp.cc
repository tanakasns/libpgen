
#include <map>
#include <string>
#include <iostream>

/* include must!!! */
#include "packet.h"
#include "pgen-variable.h"
#include "pgen-macro.h"
#include "pgen-opcode.h"
#include "pgen-funcs.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>

//#include <net/ethernet.h> 
//#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "netutil.h"



pgen_icmp::pgen_icmp(){
	pgen_ip::clear();
	clear();
}
void pgen_icmp::clear(){
	pgen_packet::clear();
	icmp_option -1;
	icmp_code = -1;
}

void pgen_icmp::sendPack(const char* ifname){
	wrap(ifname);		
	int sock;
	int n;
	
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip_dstIp._addr;
	

	if((sock=socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0){
		perror("socket: ");
		exit(-1);
	}

	if((n=sendto(sock, data, len, 0, (struct sockaddr*)&addr, sizeof addr)) < 0){
		perror("ip::send sendto()");
		exit(PGEN_ERROR);
	}

	


/*	
	if((sock=initRawSocket(ifname, 1, 0)) < 0){
		exit(PGEN_ERROR);
	}

//	if((n=sendto(sock, data, len, 0, (struct sockaddr*)&addr, sizeof addr)) < 0){
	if((n=write(sock, data, len)) < 0){
//	if((n=send(sock, data, len, 0)) < 0){
		perror("ip::send sendto()");
		exit(PGEN_ERROR);
	}
*/	
	
	printf("send size: %d\n", n);

	close(sock);
		
}



void pgen_icmp::wrap(const char* ifname){

	packetType = PGEN_PACKETTYPE_ICMP;
	pgen_ip::wrap(ifname);
	memset(data, 0, sizeof(data));
	ip.protocol = IPPROTO_ICMP;
	ip.tot_len = htons(sizeof(ip) + sizeof(icmp)) ;
	

	memset(&icmp, 0, sizeof icmp);
	icmp.icmp_type = icmp_option;
	icmp.icmp_code = icmp_code;
	icmp.icmp_cksum = 0;
	icmp.icmp_void = 0;
	icmp.icmp_hun.ih_idseq.icd_id = htons(256);
	icmp.icmp_hun.ih_idseq.icd_seq = htons(1);
	icmp.icmp_cksum = checksum(&icmp, sizeof icmp);


	u_char* p = data;
//	memcpy(p, &eth, sizeof(eth));
//	p += sizeof(eth);
	memcpy(p, &ip, sizeof(ip));
	p += sizeof(ip);
	memcpy(p, &icmp, sizeof(icmp));
	p += sizeof(icmp);

	len = p-data;

}





void pgen_icmp::info(){
	pgen_ip::info();

	std::map<int, const char*>  _icmpoption;
	_icmpoption[0] = "Echo Reply";
	_icmpoption[5] = "REdirect"; 
	_icmpoption[8] = "Echo"; 
	std::map<int, const char*> _icmpcode;
	_icmpcode[0]  = "Net Unreachable";
	_icmpcode[1]  = "Host Unreachable";
	_icmpcode[2]  = "Protocol Unreachable";
	_icmpcode[3]  = "Port Unreachable";
	_icmpcode[255]  = "Not Set";

	printf(" * Internet Control Message Protocol \n");
	printf("    - Type            :  %s (%d)\n", _icmpoption[icmp.icmp_type] , icmp.icmp_type);
	printf("    - Code            :  %s (%d)\n",  _icmpcode[icmp.icmp_code], icmp.icmp_code);
	printf("    - id(BE)          :  %d \n", icmp.icmp_hun.ih_idseq.icd_id);
	printf("    - seq num(BE)     :  %d \n", icmp.icmp_hun.ih_idseq.icd_seq);
	printf("    - Header Checksum :  0x%x \n", icmp.icmp_cksum);
}



