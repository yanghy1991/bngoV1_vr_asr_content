#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cJSON.h"
#include "szhyPacket.h"
#include "szhy_get.h"


int get_contentIp(char *ip,int *port)
{
	char rsp[1024] = {0};
	if(getTcpIP(rsp) != 0){
		return -1;
	}

	//获取IP 端口
	char *json_str = NULL;
	json_str = strstr(rsp,"{");

	if(json_str == NULL)
		return -1;
	
	cJSON *root = NULL;

	root = cJSON_Parse(json_str);

	if(!root){
		printf("%s : parse json error.\n",__func__);  
		return -1;
	}
	//获取json_ip
	cJSON *json_ip = NULL;
	json_ip = cJSON_GetObjectItem(root, "ip");
	if(json_ip == NULL)
	{
		printf("%s : get ip failed!\n",__func__);
		goto error_exit;
		//获取err_no失败
	}
	strcpy(ip,json_ip->valuestring);

	//获取port
	cJSON *json_port = NULL;
	json_port = cJSON_GetObjectItem(root, "port");
	if(json_port == NULL)
	{
		printf("%s : get port failed!\n",__func__);
		goto error_exit;
		//获取err_no失败
	}

	*port = atoi(json_port->valuestring);
	
	DEBUG("ip:%s port:%d \n",ip,*port);
	
	cJSON_Delete(root);
	return 0;

error_exit:
		cJSON_Delete(root);
		return -1;
}

