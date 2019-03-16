#ifndef _QUEUEMSG_H_
#define _QUEUEMSG_H_

struct msg_st
{
	long int msg_type;
	int flag;
	char text[4096];
};

#define CONTENT_QUEUE 9998
#define MOZART_QUEUE 4781
#define ASR_FLAG 3
#define WEATHER_FLAG 4

//发送电信请求到mozart
int msq_send(int msg_key,int flag,char *buf);
//获取mozart的返回响应
int msq_recv(struct msg_st *MessagesUart);

#endif // _QUEUEMSG_H_