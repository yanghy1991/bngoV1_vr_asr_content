#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>

#include "queueMsg.h"


int msq_send(int msg_key,int flag,char *buf)
{
	struct msg_st MessagesUart;
	memset(&MessagesUart,0,sizeof(MessagesUart));
	int msgid = -1;
	//建立消息队列
	msgid = msgget((key_t)msg_key, 0666 | IPC_EXCL);
	if(msgid == -1)
	{
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		return -1;
	}

	MessagesUart.msg_type = 1;
	MessagesUart.flag = flag;
	strcpy(MessagesUart.text,buf);

	//向队列发送数据
	if(msgsnd(msgid,&MessagesUart,sizeof(struct msg_st),0) == -1)
	{
		fprintf(stderr, "msgsnd failed\n");
		return -2;
	}

	return 0;
}


//获取mozart的返回响应
int msq_recv(struct msg_st *MessagesUart)
{
	long int msgtype = 0;
	int msqid = -1;

	msqid = msgget(CONTENT_QUEUE,0666 | IPC_CREAT);
	if(msqid < 0){
		printf("msg queue creat failed [%d] [%s]\n",CONTENT_QUEUE,strerror(errno));
		return -1;
	}

	if(msgrcv(msqid, (void*)MessagesUart, sizeof(struct msg_st), msgtype, 0) == -1)
	{
		fprintf(stderr, "msgrcv failed with errno: %d\n", errno);
		return -1;
	}

	return 0;
}

