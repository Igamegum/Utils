#include <string.h>
#include <socketpack.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>


bool validate_connect(const std::string & address,
                      const int & port,
					  const int & send_timeout_long,
					  const int & recv_timeout_long) {

	int server_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (server_sock < 0) {
		return false;
	}

	struct sockaddr_in server_address;
	memset(&server_address,0,sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(address.c_str());
	server_address.sin_port = htons(port);

	if (send_timeout_long > 0) {
		struct timeval timeout = {send_timeout_long / 1000,(send_timeout_long % 1000) * 1000};
		setsockopt(server_sock,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(struct timeval));
	}


	if (recv_timeout_long > 0) {
		struct timeval timeout = {recv_timeout_long / 1000,(recv_timeout_long % 1000) * 1000};
		setsockopt(server_sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
	}

	int rc = connect(server_sock,(struct sockaddr *)&server_address,sizeof(server_address));
	return rc == 0;
}

bool ioctl_pack(const std::string & ne_card,
                const int & behaviour,
				std::string & result) {

	int local_sock = socket(AF_INET,SOCK_STREAM,0);
	if (local_sock < 0) {
		return false;
	}

	struct ifreq local_ifr;
	memset(&local_ifr,0,sizeof(local_ifr));
	strncpy(local_ifr.ifr_name,ne_card.c_str(),sizeof(local_ifr.ifr_name) - 1);

	if ( ioctl(local_sock,behaviour,&local_ifr) < 0) {
		return false;
	}

	struct sockaddr_in * local_net = (struct sockaddr_in *)&(local_ifr.ifr_addr);

	char buffer[128];
	strcpy(buffer,inet_ntoa(local_net->sin_addr));

	result = std::string(buffer);
	close(local_sock);

	return true;
}

bool local_ip(const std::string & ne_card,std::string & result) {
	return ioctl_pack(ne_card,SIOCGIFADDR,result);
}


bool local_mask(const std::string & ne_card,std::string & result) {
	return ioctl_pack(ne_card,SIOCGIFNETMASK,result);
}

std::vector<int> format_afinet(const std::string & net) {

	std::vector<int> number;
	int sum = 0;
	for (int i = 0; i < net.length(); ++i) {
		if (net[i] == '.') {
			number.push_back(sum);
			sum = 0;
		} else if (net[i] >= '0' && net[i] <= '9') {
			sum *= 10;
			sum += net[i] -'0';
		}
	}
	number.push_back(sum);
	return std::move(number);
}

std::vector< std::pair<int,int> > calc_net_seg(const std::string & ip,const std::string mask) {

	std::vector<int> ip4 = format_afinet(ip);
	std::vector<int> mk4 = format_afinet(mask);

	if (ip4.size() != mk4.size()) {
		return {};
	}

	std::vector< std::pair<int,int> > scope;

	for (int i = 0; i < ip4.size(); ++i) {
		if (mk4[i] == 255) {
			scope.push_back( std::make_pair(ip4[i],ip4[i]) );
		} else {
			//flite all 0 and all 1 ip
			if (i == ip4.size() - 1) {
				scope.push_back( std::make_pair((mk4[i] & 255) + 1, 255 - 1) );
			} else {
				scope.push_back( std::make_pair(mk4[i] & 255,255) );
			}
		}
	}

	return scope;
}
