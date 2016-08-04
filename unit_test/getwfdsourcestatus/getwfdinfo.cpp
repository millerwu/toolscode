/*
This test is for get WFD Device information subelement.

WFD Device Information length 2 octets

The means of each bit is as blow.

0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
15<---------------------------0

1:0 WFD Device type bits            0b00: WFD source; 0b01: Primary Sink; 0b10: Secondary Sink; 0b11: dual-role possible;
5:4 WFD Session Availability bits   0b00: Not available for WFD Session; 0b01: Available for WFD Session

so, if we want to know the device is available source, need to check the 0:1 bits is 0b00 and 5:4 bits is 0b01;
*/

#include <stdio.h>
#include <stdlib.h>

bool isAvailableSource(int info)
{
    //get source type
    bool source = info == (info >> 2) << 2;

    if (source)
    {
        // get available bits;
        int tmp = info >> 3;
        if (tmp == (tmp >> 1) << 1)
        {
            tmp = tmp>>1;
            source = tmp != (tmp >> 1) << 1;
        } else {
            source =  false;
        }
    }
    printf("source = %d \n", source);
    return source;

}

int main(int argc, char** argv)
{
    int a = atoi(argv[1]);

    isAvailableSource(a);
    return 0;
}
