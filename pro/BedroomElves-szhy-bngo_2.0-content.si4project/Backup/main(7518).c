
#include "szhyPacket.h"
#include "telecom.h"
#include "queueMsg.h"
#include "content_http.h"
#include <pthread.h>
#include <errno.h>

static pthread_t msq_thread; 
int socket_fd = -1;;//socket打开文件符

void *msq_func(void *params)
{	
	struct msg_st Messages;

	while(1)
	{

		memset(&Messages,0,sizeof(Messages));
		msq_recv(&Messages);
		DEBUG("Messages->flag:%d  text:%s \n",Messages.flag,Messages.text);
		if(Messages.flag == 1){
			send_szhyServerReq(socket_fd,Messages.text);
		} else if(Messages.flag == 2){
			send_weatherReq(socket_fd,Messages.text);
		}
	}

}

int main(int argc,char *argv[])
{
	#if 0
	if(argc != 3)
		return -1;
	int port = atoi(argv[2]);
	printf("[%s] [%d] ip:%s port:%d \n",__func__,__LINE__,argv[1],port);
	#endif
	
	int pret = pthread_create(&msq_thread, NULL, msq_func, NULL);
	if (pret) {
		printf("create msq_thread error: %s\n", strerror(errno));
		return -1;
	}
	
#if 1
	int flag = 1;
	unsigned char buffer[1024] = {0};
	int buflen = sizeof(buffer);
	
	int rc=-1,ret=0;
	//int ping_cnt = 0;
	//1.建立与服务器连接的socket
	//int socket_fd;//socket打开文件符
    struct sockaddr_in socaddr;

	//select 监听是否有数据
	 fd_set readfd; //读文件描述符集合
	 struct timeval timeout;
	
	//设置socket发送、接收超时
	struct timeval time_out;

	//获取mozart返回值
	int resultCode = 0;
	//获取mozart返回信息
	char resultMsg[1024] = {0};

	int port;
	char ip[50] = {0};

socket_reboot:
	memset(ip,0,sizeof(ip));
	if(get_contentIp(ip,&port) != 0){
		printf("[%s] [%d] get_contentIp \n",__func__,__LINE__);
		goto exi;
	}
	
	//创建socket连接
	socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == socket_fd)
    {
        perror("create socket failed :");
		goto exit1;
    }

	time_out.tv_sec = 10;
	time_out.tv_usec = 0;		//设置超时时间5秒
	setsockopt(socket_fd,SOL_SOCKET,SO_RCVTIMEO,&time_out,sizeof(time_out));	//设置接收数据超时
	setsockopt(socket_fd,SOL_SOCKET,SO_SNDTIMEO,&time_out,sizeof(time_out));	//设置发送数据超时
 
	//设置IP地址结构
    bzero(&socaddr,sizeof(struct sockaddr_in));
    socaddr.sin_family = AF_INET;
    socaddr.sin_addr.s_addr = inet_addr(ip);
    socaddr.sin_port = htons(port);
    //inet_pton(AF_INET, argv[1], &socaddr.sin_addr.s_addr);
    //socaddr.sin_port = htons(port);
	//连接服务器
	ret = connect(socket_fd,(struct sockaddr*)&socaddr,(socklen_t)sizeof(socaddr));
    if(-1 == ret)
	{
        perror("connect error:");
        goto exit;
    }
	
	if(szhy_send_loginReq(socket_fd) != 0){
		goto exit;
	}

	memset(buffer,0,sizeof(buffer));
	if(szhy_read_loginEsp(socket_fd,buffer,buflen) != 0){
		goto exit;
	}
	
	start_ping_timer();
	
	 while(flag){
	 	//到达ping的时间
	 	if(time_to_ping()){
			//发送心跳请求
			if(szhy_send_pingReq(socket_fd) < 0)
				goto exit;

			if(szhy_read_pingEsp(socket_fd,buffer,buflen) < 0)
				goto exit;	
			start_ping_timer();
		}
		
		memset(buffer,0,sizeof(buffer));
		timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        //文件描述符集合清0
        FD_ZERO(&readfd);

        //将套接字描述符加入到文件描述符集合
        FD_SET(socket_fd, &readfd);

        //select侦听是否有数据到来
        ret = select(socket_fd + 1, &readfd, NULL, NULL, &timeout); //侦听是否可读
        switch (ret){
			case -1: //发生错误
				perror("select error:");
				break;
			case 0: //超时
				
				break;
			default:

				if (FD_ISSET(socket_fd,&readfd)){
					memset(buffer,0,buflen);
					//读取电信请求
					rc = szhy_read_msg(socket_fd,buffer,buflen); 

					if(rc < 0){
						goto exit;
					} else if(rc == 0){
						printf("[%s] [%d] rc:%d\n",__func__,__LINE__,rc);
						
						//读不到数据，发送心跳检测连接。
						//发送心跳请求
						if(szhy_send_pingReq(socket_fd) < 0)
							goto exit;
						
						if(szhy_read_pingEsp(socket_fd,buffer,buflen) < 0)
							goto exit;
						
					} else {
							resultCode = get_szhyServerEsp((char *)buffer);
							
							if(resultCode != 0){
								//返回错误消息 给服务器
								strcpy(resultMsg,"失败");
								resultCode = 999;
								DEBUG("解析失败\n");
							}
					}
					

				}
				
				break;
		}
		 
		 
	 }

	 
	 close(socket_fd);
	 return 0;
exi:
	sleep(10);
	if(get_contentIp(ip,&port) != 0){
		printf("[%s] [%d] get_contentIp faild\n",__func__,__LINE__);
		goto exi;
	}
	
exit:
	if(socket_fd != -1){
		close(socket_fd);
		socket_fd= -1;
	}

exit1:
	sleep(10);
	DEBUG("socket root! ##########\n");
	goto socket_reboot;
#endif

	return -1;

}
