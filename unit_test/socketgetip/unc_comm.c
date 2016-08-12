/**
filename: umsc_comm.c
compiled as a common used library for other process modules
#date:2013-08-28
#author:ljyang
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <assert.h>
#include "unc_comm.h"

/**
func: get_net_if_info
param:
	char *net_if:  net interface name
	char *ipaddr:  buffer addr stored ipaddr
	char *broadaddr:  buffer addr stored broadcast addr
	char *macaddr: buffer stored macaddr
return:
	0:success
	-1:fail
note: 根据网络接口的名字获取接口的ip地址、广播地址和mac地址
  */
int get_net_if_info(char *net_if, char *ipaddr, char *broadaddr, char *macaddr)
{
	int s,i;
	struct in_addr addr;
	struct ifreq ifr;

	unsigned int ip,mask,broad;
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if(s < 0)
	{
		perror("socket create error:");
		return -1;
	}
	strcpy(ifr.ifr_name, net_if);
	if(macaddr)
	{
		if( -1 == ioctl(s, SIOCGIFHWADDR, &ifr))
		{
			close(s);
			return -1;
		}
		for (i=0; i<6; i++)
			sprintf(&macaddr[i*3],"%02X:",((unsigned char*)ifr.ifr_hwaddr.sa_data)[i]);
		macaddr[17]='\0';
	}

	if(-1 == ioctl(s, SIOCGIFADDR, &ifr))
	{
		close(s);
		return -1;
	}
	ip = (*(struct sockaddr_in *)(&ifr.ifr_addr)).sin_addr.s_addr;
	if(ipaddr)
	{
		strcpy(ipaddr, inet_ntoa((*(struct sockaddr_in *)(&ifr.ifr_addr)).sin_addr));
	//	PR_DEBUG("ipaddr:%s\n", ipaddr);
	}
	if( -1 == ioctl(s, SIOCGIFBRDADDR, &ifr))
	{
		close(s);
		return -1;
	}
	if(broadaddr)
	{
		strcpy(broadaddr, inet_ntoa((*(struct sockaddr_in *)(&ifr.ifr_broadaddr)).sin_addr));
		broad = (*(struct sockaddr_in *)(&ifr.ifr_addr)).sin_addr.s_addr;
		if( -1 == ioctl(s, SIOCGIFNETMASK, &ifr))
		{
			close(s);
			return -1;
		}
		mask= (*(struct sockaddr_in *)(&ifr.ifr_addr)).sin_addr.s_addr;
		broad = (~mask & broad)|ip;
		addr.s_addr = broad;
		strcpy(broadaddr, inet_ntoa(addr));
	}

	close(s);
	return 0;
}
