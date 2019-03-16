
#include "szhyPacket.h"
#include "telecom.h"
#include "queueMsg.h"
#include <time.h>

//15秒ping一次服务器
#define KEEPALIVE_INTERVAL 15

time_t old_t;
void start_ping_timer(void)
{
	time(&old_t);
	old_t += KEEPALIVE_INTERVAL;
}

int time_to_ping(void)
{
	time_t t;

	time(&t);

	//DEBUG("[old_t:%ld] [t:%ld]\n",old_t,t);
	if(t >= old_t)
	  	return 1;
	return 0;
}

void endian_swap(void* data, int size)
{
	assert((size == 2) ||
                (size == 4) ||
                (size == 8));
    unsigned char* bdata = (unsigned char*)data;

	int x;
    for (x = 0; x < size/2; ++x) {
        unsigned char temp = bdata[x];
        bdata[x] = bdata[size-x-1];
        bdata[size-x-1] = temp;
    }
}

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{	
	int rc = 0;	
	rc = write(sock, buf, buflen);	
	return rc;
}

int transport_getdata(int sock, unsigned char* buf, int count)
{
	char *packet_flag = NULL;
	int rc = recv(sock, buf, count, 0);

	if(rc <= 0)
		return rc;

	//判断是否分包
	while((packet_flag = strstr((char *)buf,"$_$")) == NULL && rc <= count){
		rc += recv(sock, buf+rc, count, 0);
	}

	if(packet_flag != NULL)
		memset(packet_flag,0,sizeof("$_$"));
	
	//printf("received %d bytes count %d\n", rc, (int)count);
	return rc;
}



