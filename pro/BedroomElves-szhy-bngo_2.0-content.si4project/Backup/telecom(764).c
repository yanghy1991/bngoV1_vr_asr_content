#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>


#include "cJSON.h"
#include "telecom.h"
#include "szhyPacket.h"
#include "queueMsg.h"
#include "bngo_info.h"
#include "telecom_get.h"

typedef struct telecomEsp{
	char seq[128];
	char bngoid[128];

}telecomEsp_t;

static telecomEsp_t telecomEsp_msg = {"0","0"};


int szhy_read_msg(int sock,unsigned char *buffer,int buflen)
{
	int rc = -1;
	
	rc = transport_getdata(sock, buffer, buflen);

	return rc;
}

int szhy_send_msg(int sock,unsigned char *buffer)
{
	int rc = -1;

	rc = transport_sendPacketBuffer(sock,buffer,strlen((char *)buffer));

	DEBUG(" rc:%d\n",rc);

	return rc;
}

int	szhy_send_loginReq(int sock)
{
	#if 1
	char bngoID[30] = {0};
	//获取设备id
	if(get_bngoID_mqttpub(bngoID) != 0){
		printf("[%s] [%d] gei bngoID failed\n",__func__,__LINE__);  
		return -1;
	}
	#endif
	char buffer[512] = {0};
	//生成返回服务器json格式
	cJSON *root = NULL;
	root = cJSON_CreateObject();

	if(!root){
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
        return -1; 
	}

	cJSON_AddItemToObject(root, "headType", cJSON_CreateString("1")); 

	// from
	cJSON_AddItemToObject(root, "bngoid", cJSON_CreateString(bngoID)); 

	char *p = cJSON_PrintUnformatted(root);
	printf("[%s] [%d] p:%s\n",__func__,__LINE__,p);

	strcat(buffer,"$_$");
	strcat(buffer,p);
	strcat(buffer,"$_$");
	free(p);
	
	cJSON_Delete(root);	
	

	szhy_send_msg(sock,(unsigned char *)buffer);

	return 0;
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
		
	} else if(strcmp(headType->valuestring,"6") == 0){
			int resultCode = 0;
			//获取mozart返回信息
			char resultMsg[1024] = {0};
	
			resultCode = get_szhyServerEsp((char *)buffer);
	
			if(resultCode != 0){
				//返回错误消息 给服务器
				strcpy(resultMsg,"失败");
				resultCode = 999;
				printf("[%s] [%d] 解析失败\n",__func__,__LINE__);
			}

	}else {
		printf("[%s] [%d] [headType:%s] [rc:%d] ERROR!\n",__func__,__LINE__,headType->valuestring,rc);
		cJSON_Delete(root);
		return -1;
	}

	cJSON_Delete(root);

	return rc;
}


int	send_szhyServerReq(int sock,char *cmd)
{
	//响应返回数据
	memset(&telecomEsp_msg,0,sizeof(telecomEsp_t));

	#if 1
	char bngoID[30] = {0};
	//获取设备id
	if(get_bngoID_mqttpub(bngoID) != 0){
		printf("[%s] [%d] gei bngoID failed\n",__func__,__LINE__);  
		return -1;
	}
	#endif
	char buffer[512] = {0};
	//生成返回服务器json格式
	cJSON *root = NULL;
	root = cJSON_CreateObject();

	if(!root){
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
        return -1; 
	}

	cJSON_AddItemToObject(root, "headType", cJSON_CreateString("5")); 

	// bngoid
	cJSON_AddItemToObject(root, "bngoid", cJSON_CreateString(bngoID)); 

	//创建会话req 获取时间戳
	struct timeval tv;
    gettimeofday(&tv,NULL);
	sprintf(telecomEsp_msg.seq,"%ld",tv.tv_sec + rand() % 100);
	
	cJSON_AddItemToObject(root, "seq", cJSON_CreateString(telecomEsp_msg.seq)); 

	// cmd
	cJSON_AddItemToObject(root, "cmd", cJSON_CreateString(cmd)); 

	char *p = cJSON_PrintUnformatted(root);
	printf("[%s] [%d] p:%s\n",__func__,__LINE__,p);

	strcat(buffer,"$_$");
	strcat(buffer,p);
	strcat(buffer,"$_$");
	free(p);
	
	cJSON_Delete(root);	
	
	szhy_send_msg(sock,(unsigned char *)buffer);

	return 0;
}


//获取合一服务器响应
int get_szhyServerEsp(char *buffer)
{
	//printf("[%s] [%d] data:%s\n",__func__,__LINE__,buffer);

	//json转化成bngo控制格式
	char *json_str = NULL;
	json_str = strstr(buffer,"{");

	if(json_str == NULL)
		return -1;
	
	cJSON *root = NULL;

	root = cJSON_Parse(json_str);

	if(!root){
		printf("%s : parse json error.\n",__func__);  
		return -1;
	}
	//获取seq
	cJSON *headType = NULL;
	headType = cJSON_GetObjectItem(root, "headType");
	if(headType == NULL)
	{
		printf("%s : get headType failed!\n",__func__);
		goto error_exit;
		//获取err_no失败
	}
	//如果是ping响应，返回成功
	if(strcmp(headType->valuestring,"4") == 0){
		printf("[%s] [%d] get PINGESP success\n",__func__,__LINE__);	
		cJSON_Delete(root);
		return 0;
	}

	
	if(strcmp(headType->valuestring,"6") != 0){
		printf("get MSG_REQ[%s] faild\n",headType->valuestring);
		goto error_exit;
	}

	//获取seq
	cJSON *seq = NULL;
	seq = cJSON_GetObjectItem(root, "seq");
	if(seq == NULL)
	{
		printf("%s : get seq failed!\n",__func__);
		goto error_exit;
		//获取err_no失败
	}

	if(strcmp(telecomEsp_msg.seq,seq->valuestring) != 0){
		printf("get telecomEsp_msg.seq:%s  MSG_ESP[%s] faild\n",telecomEsp_msg.seq,seq->valuestring);
		goto error_exit;
	}

	#if 0
	//获取from
	cJSON *bngoid = NULL;
	bngoid = cJSON_GetObjectItem(root, "bngoid");
	if(bngoid == NULL)
	{
		printf("%s : get bngoid failed!\n",__func__);
		goto error_exit;
		//获取err_no失败
	}
	strcpy(telecomEsp_msg.bngoid,bngoid->valuestring);
	#endif
	
	//获取request
	cJSON *resp = NULL;
	resp = cJSON_GetObjectItem(root, "resp");
	if(resp == NULL)
	{
		printf("%s : get resp failed!\n",__func__);
		goto error_exit;
		//获取err_no失败
	}
	
	//char *p = cJSON_PrintUnformatted(resp);
	//printf("[%s] [%d] p:%s\n",__func__,__LINE__,p);
	//发送最终的json格式到mozart
	//int ret = msq_send(MOZART_QUEUE,ASR_FLAG,p);
	//free(p);

	printf("[%s] [%d] p:%s\n",__func__,__LINE__,resp->valuestring);
	int ret = msq_send(MOZART_QUEUE,ASR_FLAG,resp->valuestring);
	
	cJSON_Delete(root);
	return ret;

error_exit:
	msq_send(MOZART_QUEUE,ASR_FLAG,"{\"error\":\"szhyServerEsp invaild\",\"errId\":0}");
	cJSON_Delete(root);
	return -1;
}







