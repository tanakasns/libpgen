

#include "arptable.h"
#include "protocols.h"
#include "unknown.h"

#include <thread>
#include <iostream>



template<typename T>
static void remove(std::vector<T>& vector, unsigned int index) {
    vector.erase(vector.begin() + index);
}




arptable::arptable(const char* dev){
	strncpy(ifname, dev, sizeof(ifname)-1);
	return;	
}





int arptable::add(ipaddr ip, macaddr mac){
	for(int i=0; i<entry.size(); i++){
		if(entry[i].ip == ip){
			entry[i].mac = mac;
			return 1;
		}
	}
	arpent_t buf;
	buf.ip = ip;
	buf.mac = mac;
	entry.push_back(buf);
	return 1;
}




int arptable::del(ipaddr ip){
	for(int i=0; i<entry.size(); i++){
		if(ip == entry[i].ip){
			remove(entry, i);
			return 1;
		}
	}
	return -1;	
}



macaddr arptable::find(ipaddr ip){
	for(int i=0; i<entry.size(); i++){
		if(ip == entry[i].ip)
			return entry[i].mac;
	}
	
	this->get(ip);

	for(int i=0; i<entry.size(); i++){
		if(ip == entry[i].ip)
			return entry[i].mac;
	}

	return NULL;
}







void arptable::show(){
	for(int i=0; i<this->entry.size(); i++){
		printf("%s %s \n", entry[i].ip.c_str(), entry[i].mac.c_str());	
	}
	printf("%d entries \n", (int)entry.size());
}






int arptable::get(ipaddr ip){
	int sock = initRawSocket(this->ifname, 0, 0);
	if(sock < 0){
		perror("socket");
		return -1;
	}

	int len = 0;
	u_char data[1000];
	pgen_arp pack;
	pgen_unknown unknown;
	struct timeval tv;
	
	tv.tv_sec  = 1;
	tv.tv_usec = 0;

	pack.ETH.src.setmacbydev(ifname);
	pack.ETH.dst.setmacbroadcast();
	pack.ARP.operation = 1;
	pack.ARP.srcEth.setmacbydev(ifname);
	pack.ARP.srcIp.setipbydev(ifname);
	pack.ARP.dstEth.setmacbroadcast();
	pack.ARP.dstIp = ip;
	pack.compile();

	pgen_send_to_netif(sock, pack.data, pack.len);
	
	for(int i=0; i<3; i++){
		len = pgen_recv_from_netif_to(sock, data, sizeof(data), tv);
		if(len == 0){
			continue;
		}else if(len < 0){
			pgen_perror("pgen_recv_from_netif");
			close(sock);
			return -1;
		}
		unknown.cast(data, len);
		if(unknown.isARP){
			pack.cast(data, len);
			if(pack.ARP.operation==2 && pack.ARP.srcIp==ip){
				this->add(ip, pack.ARP.srcEth);
				close(sock);
				return 1;
			}
		}
	}

	close(sock);
	return -1;
}




