#include <stdio.h>
#include <string.h>

void getconfigformString(char* msg)
{
	char* req = NULL;
	if (strstr(msg, "P2P-GROUP-STARTED ") != NULL)
	{
		char* temp;
		char msg_temp[strlen(msg)];
		strcpy(msg_temp, msg);
		
		char starttag;
		char msginfo[30];
		char group_interface[20];
		char type[10];
		
		char ssid[40];
		char freq[12];
		char passphare[40];
		
		sscanf(msg_temp, "%s%s%s%s%s%s", msginfo, group_interface, type, ssid, freq, passphare);
		
		if (strstr(ssid, "ssid=")!=NULL)
		{
			char* r_ssid = strstr(ssid, "ssid=") + strlen("ssid=");
			printf("\n\n\n***********ssid**************\n\n   %s   \n\n*****************************\n\n", r_ssid);
		}
		if (strstr(passphare, "passphrase="))
		{
			char* r_passphare = strstr(passphare, "passphrase=") + strlen("passphrase=");
			printf("\n\n\n***********passphare**********\n\n   %s   \n\n*****************************\n\n", r_passphare);
		}
		printf("msginfo = %s \n", msginfo);
		printf("group_interface = %s \n", group_interface);
		printf("type = %s \n", type);
		printf("freq = %s \n", freq);
	}
}

int main() 
{
	char test[] = "P2P-GROUP-STARTED p2p-wfd0-0 GO ssid=\"DIRECT-lN\" freq=2437 passphrase=\"5jQcWTOb\" go_dev_addr=0e:84:dc:c7:80:50";
	getconfigformString(test);
	return 0;
}