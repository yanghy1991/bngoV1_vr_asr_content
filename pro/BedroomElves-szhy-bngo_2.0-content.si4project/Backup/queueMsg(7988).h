#ifndef _QUEUEMSG_H_
#define _QUEUEMSG_H_

struct msg_st
{
	long int msg_type;
	int flag;
	char text[4096];
};

//发送电信请求到mozart
int msq_send(int msg_key,int flag,char *buf);
//获取mozart的返回响应
int msq_recv(int *resultCode,char *resultMsg);

#endif // _QUEUEMSG_H_