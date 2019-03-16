
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
	int rc = recv(sock, buf, count, 0);
	//printf("received %d bytes count %d\n", rc, (int)count);
	return rc;
}


int szhy_send_pingReq(int sock)
{
	int rc = -1;

	char pingReq[512] = "{\"headType\":\"3\"}$_$";
	rc = transport_sendPacketBuffer(sock,(unsigned char *)pingReq,strlen(pingReq));
	
	DEBUG(" [rc:%d]\n",rc);

	return rc;
}

int szhy_read_pingEsp(int sock,unsigned char *buffer,int buflen)
{
	int rc = -1;

	rc = transport_getdata(sock, buffer, buflen);

	
	//解析是否注册成功
	char *json_str = strstr((char *)buffer,"{");
	if(json_str == NULL)
		return -1;

	cJSON * root = NULL;
	cJSON * headType = NULL;
	
	root = cJSON_Parse(json_str);
	if (!root) 
   	 {
		printf("%s : parse json error.\n",__func__);  
		return -1;
   	 }
	headType = cJSON_GetObjectItem(root, "headType");
	if(headType == NULL)
	{
		printf("%s : get headType faild\n",__func__);
		cJSON_Delete(root);
		return -1;//获取err_msg失败
	}

	if(strcmp(headType->valuestring,"4") == 0){
		printf("[%s] [%d] [rc:%d] get PINGESP success\n",__func__,__LINE__,rc);	
		
	} else if(strcmp(headType->valuestring,"4") == 0){
			int resultCode = 0;
			//获取mozart返回信息
			char resultMsg[1024] = {0};
	
			resultCode = get_telecomReq((char *)buffer);
	
			if(resultCode != 0){
				//返回错误消息 给服务器
				strcpy(resultMsg,"失败");
				resultCode = 999;
			} else	{
				//获取mozart的返回响应
				msq_recv(&resultCode,resultMsg);
				if(resultCode == 0)
					resultCode = 1;
			}
	
			memset(buffer,0,sizeof(buffer));
			get_mozartEsp(resultCode,resultMsg,buffer);
			szhy_send_msg(sock,buffer,strlen((char *)buffer));

	}else {
		printf("[%s] [%d] [headType:%s] [rc:%d] ERROR!\n",__func__,__LINE__,headType->valuestring,rc);
		cJSON_Delete(root);
		return -1;
	}

	cJSON_Delete(root);

	return rc;
}

int szhy_send_loginReq(int sock,unsigned char *buffer)
{
	int rc = -1;

	rc = transport_sendPacketBuffer(sock,buffer,strlen((char *)buffer));

	DEBUG(" rc:%d\n",rc);

	return rc;
}

int szhy_read_loginEsp(int sock,unsigned char *buffer,int buflen)
{
	int rc = -1;
	
	rc = transport_getdata(sock, buffer, buflen);
	
	if(rc < 0){
		printf("[%s] [%d] get loginEsp failed\n",__func__,__LINE__);
		return -1;
	}

	//解析是否注册成功
	char *json_str = strstr((char *)buffer,"{");
	if(json_str == NULL)
		return -1;

	cJSON * root = NULL;
	cJSON * headType = NULL;
	
	root = cJSON_Parse(json_str);
	if (!root) 
   	 {
		printf("%s : parse json error.\n",__func__);  
		return -1;
   	 }
	headType = cJSON_GetObjectItem(root, "headType");
	if(headType == NULL)
	{
		printf("%s : get headType faild\n",__func__);
		cJSON_Delete(root);
		return -1;//获取err_msg失败
	}

	if(strcmp(headType->valuestring,"2")){
		printf("%s : headType:%s \n",__func__,headType->valuestring);
		cJSON_Delete(root);
		return -1;//获取err_msg失败
	}

	printf("[%s] [%d] [rc:%d] get LOGINESP success\n",__func__,__LINE__,rc);
	
	cJSON_Delete(root);
	return 0;
}

int szhy_send_msg(int sock,unsigned char *buffer,int buflen)
{
	int rc = -1;

	rc = transport_sendPacketBuffer(sock,buffer,buflen);

	DEBUG("[buffer size:%d]  rc:%d \n",buflen,rc);
	
	return rc;
}

int szhy_read_msg(int sock,unsigned char *buffer,int buflen)
{
	int rc = -1;
	
	rc = transport_getdata(sock, buffer, buflen);

	printf(" [%s] [%d] rc:%d \n",__func__,__LINE__,rc);
	
	return rc;
}
