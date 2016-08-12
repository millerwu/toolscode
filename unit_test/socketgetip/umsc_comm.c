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
#include "umsc_comm.h"

/*
 *			P I N G . C
 *
 * Using the InterNet Control Message Protocol (ICMP) "ECHO" facility,
 * measure round-trip-delays and packet loss across network paths.
 *
 * Author -
 *	Mike Muuss
 *	U. S. Army Ballistic Research Laboratory
 *	December, 1983
 * Modified at Uc Berkeley
 *
 * Changed argument to inet_ntoa() to be struct in_addr instead of u_long
 * DFM BRL 1992
 *
 * Status -
 *	Public Domain.  Distribution Unlimited.
 *
 * Bugs -
 *	More statistics could always be gathered.
 *	This program has to run SUID to ROOT to access the ICMP socket.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <sys/ioctl.h> 

#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <signal.h>
#include <strings.h>
#define	MAXWAIT		10	/* max time to wait for response, sec. */
#define	MAXPACKET	4096	/* max packet size */
#define VERBOSE		1	/* verbose flag */
#define QUIET		2	/* quiet flag */
#define FLOOD		4	/* floodping flag */
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN	64
#endif
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BUFSIZE 8192
#include <linux/rtnetlink.h>
struct route_info
{
	struct in_addr dstAddr;
	struct in_addr srcAddr;
	struct in_addr gateWay;
	char ifName[IF_NAMESIZE];
};

u_char	packet[MAXPACKET];
int	i, pingflags, options;
extern	int errno;

int s;			/* Socket file descriptor */
struct hostent *hp;	/* Pointer to host info */
struct timezone tz;	/* leftover */

struct sockaddr whereto;/* Who to ping */
int datalen = 56;		/* How much data */

char usage[] =
"Usage:  ping [-dfqrv] host [packetsize [count [preload]]]\n";

char *hostname;
char hnamebuf[MAXHOSTNAMELEN];

int npackets;
int preload = 0;		/* number of packets to "preload" */
int ntransmitted = 0;		/* sequence # for outbound packets = #sent */
int ident;

int nreceived = 0;		/* # of packets we got back */
int timing = 0;
int tmin = 999999999;
int tmax = 0;
int tsum = 0;			/* sum of all times, for doing average */
int catcher();
void finish(int sig);
char *inet_ntoa();


/*
 * 			M A I N
 */
int try_connect(char *dest)
{
	struct sockaddr_in from;
	struct sockaddr_in *to = (struct sockaddr_in *) &whereto;
	int on = 1;
	struct protoent *proto;

	bzero((char *)&whereto, sizeof(struct sockaddr) );
	to->sin_family = AF_INET;
	to->sin_addr.s_addr = inet_addr(dest);

	if(to->sin_addr.s_addr != (unsigned)-1) {
		strcpy(hnamebuf, dest);
		hostname = hnamebuf;
	} else {
		hp = gethostbyname(dest);
		if (hp) {
			to->sin_family = hp->h_addrtype;
			bcopy(hp->h_addr, (caddr_t)&to->sin_addr, hp->h_length);
			hostname = hp->h_name;
		} else {
			return -1;
		}
	}

	ident = getpid() & 0xFFFF;
		printf("DEBUG========ON===%d\n", __LINE__);
#if 0
	if ((proto = getprotobyname("icmp")) == NULL) {
		perror("");
		fprintf(stderr, "icmp: unknown protocol\n");
		return -1;
	}
#endif
	if ((s = socket(AF_INET, SOCK_RAW, 1)) < 0) {
		perror("ping: socket");
		return -1;
	}
	if (options & SO_DEBUG) {
		if(pingflags & VERBOSE)
			printf("...debug on.\n");
		setsockopt(s, SOL_SOCKET, SO_DEBUG|SO_BROADCAST, &on, sizeof(on));
		printf("DEBUG========ON\n");
	}
	if (options & SO_DONTROUTE) {
		if(pingflags & VERBOSE)
			printf("...no routing.\n");
		setsockopt(s, SOL_SOCKET, SO_DONTROUTE, &on, sizeof(on));
		printf("DEBUG========DONTROUTE\n");
	}

	setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	if(to->sin_family == AF_INET) {
	//	printf("PING %s (%s): %d data bytes\n", hostname,
	//			inet_ntoa(to->sin_addr), datalen);	/* DFM */
	} else {
	//	printf("PING %s: %d data bytes\n", hostname, datalen );
	}
	setlinebuf( stdout );

	/* fire off them quickies */
	for(i=0; i < preload; i++)
		pinger();
	if(!(pingflags & FLOOD))
		catcher();	/* start things going */
	for(i=0;i<4;i++)
	{
		int len = sizeof (packet);
		socklen_t fromlen = sizeof (from);
		int cc;
		struct timeval timeout;
		fd_set fdmask ;
		FD_ZERO(&fdmask);
		FD_SET(s, &fdmask);

		timeout.tv_sec = 0;
		timeout.tv_usec = 10000;

		if(pingflags & FLOOD) {
			pinger();
			if( select(32, &fdmask, 0, 0, &timeout) == 0)
				continue;
		}
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout) );
		cc = 0;
		if ( (cc=recvfrom(s, packet, len, 0, (struct sockaddr*)&from, &fromlen)) < 0) {
			if( errno == EINTR || errno ==  EAGAIN || errno == EWOULDBLOCK)
				continue;
			perror("ping: recvfrom");
			close(s);
			return -1;
		}

		printf("DEBUG========ON===%s\n", inet_ntoa(from.sin_addr));
		if (cc >= 76) {			/* ip + icmp */
			struct iphdr *iphdr = (struct iphdr *) packet;

			struct icmp *pkt = (struct icmp *) (packet + (iphdr->ihl << 2));	/* skip ip hdr */
			if (pkt->icmp_type == ICMP_ECHOREPLY)
			{
				break;
			}else
			{
			}
		}
	}

	close(s);

	if(i >= 4)
	{
		return -1;
	}
	return 0;
}

/*
 * 			C A T C H E R
 * 
 * This routine causes another PING to be transmitted, and then
 * schedules another SIGALRM for 1 second from now.
 * 
 * Bug -
 * 	Our sense of time will slowly skew (ie, packets will not be launched
 * 	exactly at 1-second intervals).  This does not affect the quality
 *	of the delay and loss statistics.
 */
catcher()
{
	int waittime;

	pinger();
	if (npackets == 0 || ntransmitted < npackets)
	{
	}
	//	alarm(5);
	else {
		if (nreceived) {
			waittime = 2 * tmax / 1000;
			if (waittime == 0)
				waittime = 1;
		} else
			waittime = MAXWAIT;
	//	signal(SIGALRM, finish);
	//	alarm(waittime);
	}
}

/*
 * 			P I N G E R
 * 
 * Compose and transmit an ICMP ECHO REQUEST packet.  The IP packet
 * will be added on by the kernel.  The ID field is our UNIX process ID,
 * and the sequence number is an ascending integer.  The first 8 bytes
 * of the data portion are used to hold a UNIX "timeval" struct in VAX
 * byte-order, to compute the round-trip time.
 */
pinger()
{
	static u_char outpack[MAXPACKET];
	register struct icmp *icp = (struct icmp *) outpack;
	int i, cc;
	register struct timeval *tp = (struct timeval *) &outpack[8];
	register u_char *datap = &outpack[8+sizeof(struct timeval)];

	icp->icmp_type = ICMP_ECHO;
	icp->icmp_code = 0;
	icp->icmp_cksum = 0;
	icp->icmp_seq = ntransmitted++;
	icp->icmp_id = ident;		/* ID */

	cc = datalen+8;			/* skips ICMP portion */

	if (timing)
		gettimeofday( tp, &tz );

	for( i=8; i<datalen; i++)	/* skip 8 for time */
		*datap++ = i;

	/* Compute ICMP checksum here */
	icp->icmp_cksum = in_cksum( icp, cc );

	/* cc = sendto(s, msg, len, flags, to, tolen) */
	i = sendto( s, outpack, cc, 0, &whereto, sizeof(struct sockaddr) );

	if( i < 0 || i != cc )  {
		if( i<0 )  perror("sendto");
		printf("ping: wrote %s %d chars, ret=%d\n",
				hostname, cc, i );
		fflush(stdout);
	}
	if(pingflags == FLOOD) {
		putchar('.');
		fflush(stdout);
	}
}

/*
 * 			P R _ T Y P E
 *
 * Convert an ICMP "type" field to a printable string.
 */
	char *
pr_type( t )
	register int t;
{
	static char *ttab[] = {
		"Echo Reply",
		"ICMP 1",
		"ICMP 2",
		"Dest Unreachable",
		"Source Quench",
		"Redirect",
		"ICMP 6",
		"ICMP 7",
		"Echo",
		"ICMP 9",
		"ICMP 10",
		"Time Exceeded",
		"Parameter Problem",
		"Timestamp",
		"Timestamp Reply",
		"Info Request",
		"Info Reply"
	};

	if( t < 0 || t > 16 )
		return("OUT-OF-RANGE");

	return(ttab[t]);
}


/*
 *			I N _ C K S U M
 *
 * Checksum routine for Internet Protocol family headers (C Version)
 *
 */
in_cksum(addr, len)
	u_short *addr;
	int len;
{
	register int nleft = len;
	register u_short *w = addr;
	register u_short answer;
	register int sum = 0;

	/*
	 *  Our algorithm is simple, using a 32 bit accumulator (sum),
	 *  we add sequential 16 bit words to it, and at the end, fold
	 *  back all the carry bits from the top 16 bits into the lower
	 *  16 bits.
	 */
	while( nleft > 1 )  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if( nleft == 1 ) {
		u_short	u = 0;

		*(u_char *)(&u) = *(u_char *)w ;
		sum += u;
	}

	/*
	 * add back carry outs from top 16 bits to low 16 bits
	 */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return (answer);
}

/*
 * 			T V S U B
 * 
 * Subtract 2 timeval structs:  out = out - in.
 * 
 * Out is assumed to be >= in.
 */
tvsub( out, in )
	register struct timeval *out, *in;
{
	if( (out->tv_usec -= in->tv_usec) < 0 )   {
		out->tv_sec--;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}

/*
 *			F I N I S H
 *
 * Print out statistics, and give up.
 * Heavily buffered STDIO is used here, so that all the statistics
 * will be written with 1 sys-write call.  This is nice when more
 * than one copy of the program is running on a terminal;  it prevents
 * the statistics output from becomming intermingled.
 */
void finish(int sig)
{
	putchar('\n');
	fflush(stdout);
	printf("\n----%s PING Statistics----\n", hostname );
	printf("%d packets transmitted, ", ntransmitted );
	printf("%d packets received, ", nreceived );
	if (ntransmitted)
		if( nreceived > ntransmitted)
			printf("-- somebody's printing up packets!");
		else
			printf("%d%% packet loss", 
					(int) (((ntransmitted-nreceived)*100) /
						ntransmitted));
	printf("\n");
	if (nreceived && timing)
		printf("round-trip (ms)  min/avg/max = %d/%d/%d\n",
				tmin,
				tsum / nreceived,
				tmax );
	fflush(stdout);
	exit(0);
}
int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
	struct nlmsghdr *nlHdr;
	int readLen = 0, msgLen = 0;

	do
	{
		/* Recieve response from the kernel */
		if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)
		{
			perror("SOCK READ: ");
			return -1;
		}

		nlHdr = (struct nlmsghdr *)bufPtr;

		/* Check if the header is valid */
		if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
		{
			perror("Error in recieved packet");
			return -1;
		}

		/* Check if the its the last message */
		if(nlHdr->nlmsg_type == NLMSG_DONE)
		{
			break;
		}
		else
		{
			/* Else move the pointer to buffer appropriately */
			bufPtr += readLen;
			msgLen += readLen;
		}

		/* Check if its a multi part message */
		if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
		{
			/* return if its not */
			break;
		}
	} while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));

	return msgLen;
}

/* parse the route info returned */
void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo)
{
	struct rtmsg *rtMsg;
	struct rtattr *rtAttr;
	int rtLen;

	rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);

	/* If the route is not for AF_INET or does not belong to main routing table then return. */
	if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
		return;

	/* get the rtattr field */
	rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
	rtLen = RTM_PAYLOAD(nlHdr);

	for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen))
	{
		switch(rtAttr->rta_type)
		{
			case RTA_OIF:
				if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
				break;

			case RTA_GATEWAY:
				memcpy(&rtInfo->gateWay, RTA_DATA(rtAttr), sizeof(rtInfo->gateWay));
				break;

			case RTA_PREFSRC:
				memcpy(&rtInfo->srcAddr, RTA_DATA(rtAttr), sizeof(rtInfo->srcAddr));
				break;

			case RTA_DST:
				memcpy(&rtInfo->dstAddr, RTA_DATA(rtAttr), sizeof(rtInfo->dstAddr));
				break;
		}
	}

	return;
}

// meat
int get_gatewayip(char *gatewayip, socklen_t size)
{
	int found_gatewayip = 0;

	struct nlmsghdr *nlMsg;
	struct rtmsg *rtMsg;
	struct route_info *rtInfo;
	char msgBuf[BUFSIZE]; // pretty large buffer

	int sock, len, msgSeq = 0;

	printf("get_gatewayip enter\n");
	/* Create Socket */
	if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
	{
		perror("Socket Creation: ");
		return(-1);
	}

	/* Initialize the buffer */
	memset(msgBuf, 0, BUFSIZE);

	/* point the header and the msg structure pointers into the buffer */
	nlMsg = (struct nlmsghdr *)msgBuf;
	rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);

	/* Fill in the nlmsg header*/
	nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
	nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .

	nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
	nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
	nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.

	/* Send the request */
	if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0)
	{
		fprintf(stderr, "Write To Socket Failed...\n");
		close(sock);
		return -1;
	}

	/* Read the response */
	if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0)
	{
		fprintf(stderr, "Read From Socket Failed...\n");
		close(sock);
		return -1;
	}

	/* Parse and print the response */
	rtInfo = (struct route_info *)malloc(sizeof(struct route_info));

	for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len))
	{
		memset(rtInfo, 0, sizeof(struct route_info));
		parseRoutes(nlMsg, rtInfo);

		// Check if default gateway
		if (strstr((char *)inet_ntoa(rtInfo->dstAddr), "0.0.0.0"))
		{
			// copy it over
			inet_ntop(AF_INET, &rtInfo->gateWay, gatewayip, size);
			if(strcmp(gatewayip, "0.0.0.0"))
			found_gatewayip = 1;
			break;
		}
	}

	free(rtInfo);
	close(sock);

	printf("get_gatewayip exit\n");
	return found_gatewayip;
}

int is_disconnect_from_internet(void)
{
	return try_connect("upgrade.uncplay.com");
}

int is_disconnect_from_ap(void)
{
	char gatewayip[20]="";
	int i = 0;
	int size = sizeof(struct sockaddr_in);
	
	if( 1 == get_gatewayip(gatewayip, size))
	{
		for(i=0; i<2; i++)
		{
			if(0 == try_connect(gatewayip))
				return 0;
			usleep(1000000);
		}
		return -1;
	}else
	{
		return -1;
	}
}

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
		printf("line = %d \n", __LINE__);
		if( -1 == ioctl(s, SIOCGIFHWADDR, &ifr))
		{
			printf("line = %d \n", __LINE__);
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
		printf("line = %d \n", __LINE__);
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
		printf("line = %d \n", __LINE__);
		close(s);
		return -1;
	}
	if(broadaddr)
	{
		strcpy(broadaddr, inet_ntoa((*(struct sockaddr_in *)(&ifr.ifr_broadaddr)).sin_addr));
		broad = (*(struct sockaddr_in *)(&ifr.ifr_addr)).sin_addr.s_addr;
		if( -1 == ioctl(s, SIOCGIFNETMASK, &ifr))
		{
			printf("line = %d \n", __LINE__);
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

int main(int argc, char **argv)
{

	char ip[20] = "";
	char broadcastip[20] = "";
	char macaddr[20] = "";

	if (get_net_if_info("eth0", ip, broadcastip, macaddr)==0)
	{
		printf("if = eth0, ip = %s, broadcastip = %s, macaddr = %s", ip, broadcastip, macaddr);
	}
	// printf("if = eth0, ip = %s, broadcastip = %s, macaddr = %s", ip, broadcastip, macaddr);
	try_connect(broadcastip);

	// char gatewayip[20]="";
	// int i = 0;
	// int size = sizeof(struct sockaddr_in);
	
	// get_gatewayip(gatewayip, size);
	// printf("gateip:%s\n", gatewayip);
			
	return 0;
}
