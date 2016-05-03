#include <stdio.h>
#include <string.h>

#define MAX_LIZE_SIZE 200
static int getip(char* ipaddr)
{
	const static char shell_cmd[] = "ifconfig";
	const static char addr_tag[] = "inet addr:";
	//to add evt2's p2p interface here and use to strstr API;
	//const static char w_interface[];
	FILE* p_cmd = NULL;
	char buff[200];
	if ((p_cmd = popen(shell_cmd, "r")) != NULL) {
		while (fgets(buff, 200, p_cmd) != NULL)
		{
			if (strstr(buff, "eth0") != NULL)
			{
				if (fgets(buff, 200, p_cmd) != NULL)
				{
					char* start = NULL;
					if ((start = strstr(buff, addr_tag)) != NULL)
					{
						start = start+strlen(addr_tag);
						int i = 0;
						while (*start != ' ')
						{
							ipaddr[i++] = *(start++);
						}
						fclose(p_cmd);
						return 0;
					}
				}
			}
		}
		fclose(p_cmd);
	} else {
		return -1;
	}
		
	return 0;
}

int getWFDInferfaceMacAddr(char* macAddr)
{
	const static char shell_cmd[] = "ifconfig -a";
	const static char mac_tag[] = "HWaddr ";

	FILE* p_cmd = NULL;

	char buff[200];
	if ((p_cmd = popen(shell_cmd, "r")) != NULL)
	{
		while (fgets(buff, 200, p_cmd) != NULL)
		{
			
			if ((strstr(buff, "eth0")) != NULL)
			{
				char* start = NULL;
				if ((start = strstr(buff, mac_tag)) != NULL)
				{
					start += strlen(mac_tag);
					while (*start != '\0')
					{
						*(macAddr++) = *(start++); 
					}
					fclose(p_cmd);
					return 0;
				}
				
			}
		}
		fclose(p_cmd);
	} 
	return -1;
}


int main() {
	int res;
	char ipaddr[20] = {0};
	res = getip(ipaddr);
	printf("ipaddr = %s \n", ipaddr);
	char macAddr[20] = {1};
	res = getWFDInferfaceMacAddr(macAddr);
	printf("macAddr = %s, res = %d\n", macAddr, res);
	return 0;
}
