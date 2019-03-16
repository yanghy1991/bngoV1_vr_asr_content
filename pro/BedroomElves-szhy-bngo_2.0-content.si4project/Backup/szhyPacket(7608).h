 
#ifndef _SZHYPACKET_H_
#define _SZHYPACKET_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>    
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include<dirent.h>
#include <assert.h>

//如果是正式服务器 注释该行
//#define __CESHI__

#ifdef __CESHI__
	//测试
	//#define SERVER_PORT 8706
	//#define SERVER_IP "47.106.157.206"

	#define DEBUG(format, ...) printf("[%s : %s : %d] ",__FILE__,__func__,__LINE__); printf(format, ##__VA_ARGS__);
#else
	//正式
	//#define SERVER_PORT 8706
	//#define SERVER_IP "47.107.25.1"

	#define DEBUG(format, ...)
#endif

enum errors
{
	MQTTPACKET_BUFFER_TOO_SHORT = -2,
	MQTTPACKET_READ_ERROR = -1,
	MQTTPACKET_READ_COMPLETE
};

enum msgTypes
{
	RESERVED, LOGINREQ, LOGINESP, PINGREQ, PINGRESP, PUBLISHREQ,
	PUBLISHESP, SINGLEREQ, CLINETREQ, CONNACK
};


typedef struct
{
	unsigned char crcCode[4];
	unsigned long long sessionID;
	char type;
	char priority;
	int buflen;
}__attribute__((packed)) szhy_header_t;

void start_ping_timer(void);
int time_to_ping(void);

//大小端转换
void endian_swap(void* data, int size);


int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen);
int transport_getdata(int sock, unsigned char* buf, int count);


#endif //
