#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cJSON.h"
#include "telecom.h"
#include "szhyPacket.h"
#include "queueMsg.h"
#include "bngo_info.h"
#include "telecom_get.h"

#define RESPONSE_SIZE 200*1024

typedef struct telecomEsp{
	char seq[128];
	char bngoid[128];

}telecomEsp_t;

static telecomEsp_t telecomEsp_msg;;


int get_idxBngo(char *idx,char *idxBngo){
	//去掉特殊字符
	char *p = NULL;
	char *m = NULL;
	p = idxBngo;
	m = idx;
	
	while(*m != '\0'){
		if(*m >= 'a' && *m <= 'z'){
			*p++ = *m++;
		}else if(*m >= 'A' && *m <= 'Z'){
			*p++ = *m++;
		}else if(*m >= '0' && *m <= '9'){
			*p++ = *m++;
		}else{
			++m;	
		}
	}
	*p = '\0';
	m = NULL;

	return 0;
}


int get_keyBngo(char *keyBngo)
{
	/*
		1.机顶盒: 	 power\poweroff\0\1\2\3\4\5\6\7\8\9\ch-\ch+\vol-\vol+\up\down\left\right\exit\menu\mute\ok\signal\back\boot
		2.电视: 	 power\poweroff\0\1\2\3\4\5\6\7\8\9\ch-\ch+\vol-\vol+\up\down\left\right\exit\menu\mute\ok\signal\back\boot
		6.风扇: 	 power\poweroff\mode\oscillation\timer\fanspeed\
		7.空调: 	 on\off\aa\ad\aw\ar16..ar30\ah16..ah30
		10.电视盒子: power\poweroff\0\1\2\3\4\5\6\7\8\9\ch-\ch+\vol-\vol+\up\down\left\right\exit\menu\mute\ok\signal\back\boot
	*/

	if(!strcmp(keyBngo,"Power")){
		strcpy(keyBngo,"power");
	} else if(!strcmp(keyBngo,"Source")){
		strcpy(keyBngo,"signal");
	} else if(!strcmp(keyBngo,"Menu")){
		strcpy(keyBngo,"menu");
	} else if(!strcmp(keyBngo,"Back")){
		strcpy(keyBngo,"back");
	} else if(!strcmp(keyBngo,"Homepage")){
		strcpy(keyBngo,"boot");
	} else if(!strcmp(keyBngo,"Mute")){
		strcpy(keyBngo,"mute");
	} else if(!strcmp(keyBngo,"Volume+")){
		strcpy(keyBngo,"vol+");
	} else if(!strcmp(keyBngo,"Volume-")){
		strcpy(keyBngo,"vol-");
	} else if(!strcmp(keyBngo,"Channel+")){
		strcpy(keyBngo,"ch+");
	} else if(!strcmp(keyBngo,"Channel-")){
		strcpy(keyBngo,"ch-");	
	} else if(!strcmp(keyBngo,"Up")){
		strcpy(keyBngo,"up");
	} else if(!strcmp(keyBngo,"Down")){
		strcpy(keyBngo,"down");
	} else if(!strcmp(keyBngo,"Left")){
		strcpy(keyBngo,"left");
	} else if(!strcmp(keyBngo,"Right")){
		strcpy(keyBngo,"right");
	} else if(!strcmp(keyBngo,"OK")){
		strcpy(keyBngo,"ok");
	
	//jidinghe
	} else if(!strcmp(keyBngo,"Audio")){
		strcpy(keyBngo,"track");
	} else if(!strcmp(keyBngo,"Play")){
		strcpy(keyBngo,"play");
	} else if(!strcmp(keyBngo,"Stop")){
		strcpy(keyBngo,"stop");
	} else if(!strcmp(keyBngo,"Forward")){
		strcpy(keyBngo,"ff");
	} else if(!strcmp(keyBngo,"Backward")){
		strcpy(keyBngo,"rew");
	} else if(!strcmp(keyBngo,"Prev")){
		strcpy(keyBngo,"previous");
	} else if(!strcmp(keyBngo,"Next")){
		strcpy(keyBngo,"next");

	} else if(!strcmp(keyBngo,"Setup")){
		strcpy(keyBngo,"set");
	} else if(!strcmp(keyBngo,"Mode")){
		
	} else if(!strcmp(keyBngo,"TV")){
		strcpy(keyBngo,"live");
	} else if(!strcmp(keyBngo,"Review")){
		strcpy(keyBngo,"recall");

	} else if(!strcmp(keyBngo,"Demand")){	

	} else if(!strcmp(keyBngo,"Info")){
		strcpy(keyBngo,"info");
	} else if(!strcmp(keyBngo,"Red")){
		strcpy(keyBngo,"red");
	} else if(!strcmp(keyBngo,"Green")){
		strcpy(keyBngo,"green");
	} else if(!strcmp(keyBngo,"Yellow")){
		strcpy(keyBngo,"yellow");
	} else if(!strcmp(keyBngo,"Blue")){
		strcpy(keyBngo,"blue");

	//	
	} else if(!strcmp(keyBngo,"Return")){
		strcpy(keyBngo,"back");

	} else if(!strcmp(keyBngo,"Mode")){
		strcpy(keyBngo,"mode");
	} else if(!strcmp(keyBngo,"Timer")){
		strcpy(keyBngo,"timer");
	} else if(!strcmp(keyBngo,"Wind")){
		strcpy(keyBngo,"fanspeed");
	} else if(!strcmp(keyBngo,"Sway")){
		strcpy(keyBngo,"oscillation");
	}

	
	
	return 0;
}

/**
*功能:获取码库版本
*参数:
*file_path:码库名称
*v:返回的码库版本
*/
int get_code_v(char *json_str,int *v)
{
	cJSON * root = NULL;
	cJSON * error = NULL;
	cJSON * c_v = NULL;

	root = cJSON_Parse(json_str);
	if (!root) 
    {
        //printf("Error before: [%s]\n",cJSON_GetErrorPtr());
		printf("%s : parse json error.\n",__func__);  
		return 5;
    }	

	//获取 error 错误数据，返回原因
	error = cJSON_GetObjectItem(root, "error");
	if(error != NULL)
	{
		printf("%s : get error failed![%s]\n",__func__,error->valuestring);
		cJSON_Delete(root);
		return 18;//获取err_no失败
	}

	//获取 rid
	c_v = cJSON_GetObjectItem(root, "v");
	if(NULL == c_v)
	{
		printf("%s : get c_v failed!\n",__func__);
		cJSON_Delete(root);
		return 5;//获取err_msg失败
	}
	*v = c_v->valueint;

	cJSON_Delete(root);
	return 0;
}

//组装成json格式，然后发送到mozart
int sendCmd_to_mozart(char *buffer)
{
	//printf("[%s] [%d] buffer:%s\n",__func__,__LINE__,buffer);

	cJSON *root = NULL;
	root = cJSON_CreateObject();

	if(!root){
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
        return -1; 
	}

	// errId
    cJSON_AddItemToObject(root, "errId", cJSON_CreateNumber(2000)); 
	// errMsg
	cJSON_AddItemToObject(root, "errMsg", cJSON_CreateString("success")); 
	// input
	cJSON_AddItemToObject(root, "input", cJSON_CreateString("电信请求")); 

	cJSON *results = NULL;
	results = cJSON_CreateObject();
	
	cJSON_AddItemToObject(root, "results", results); 
	cJSON_AddItemToObject(results, "theme", cJSON_CreateString("电信请求")); 
	cJSON_AddItemToObject(results, "input", cJSON_CreateString("电信请求"));

	cJSON *sds = NULL;
	sds = cJSON_CreateObject();

	cJSON_AddItemToObject(results, "sds", sds); 
	cJSON_AddItemToObject(sds, "domain", cJSON_CreateString("command"));
	cJSON_AddItemToObject(sds, "output", cJSON_CreateString("null"));

	cJSON *semantics = NULL;
	semantics = cJSON_CreateObject();

	cJSON_AddItemToObject(results, "semantics", semantics); 

	cJSON *request = NULL;
	request = cJSON_CreateObject();

	cJSON_AddItemToObject(semantics, "request", request); 
	cJSON_AddItemToObject(request, "domain", cJSON_CreateString("中控"));

	cJSON *param = NULL;
	param = cJSON_CreateObject();

	cJSON_AddItemToObject(request, "param", param); 
	cJSON_AddItemToObject(param, "设备名称", cJSON_CreateString(buffer));

	char *p = cJSON_PrintUnformatted(root);
	printf("[%s] [%d] p:%s\n",__func__,__LINE__,p);
	//发送最终的json格式到mozart
	msq_send(4781,2,p);
	free(p);
	
	cJSON_Delete(root);
	return 0;
}



//根据type类型，转成相应的控制命令
int get_telecomReq_command(cJSON *root,int type)
{

	char new_path[200] = {0};
	char buffer[512] = {0};
	cJSON *categoryId = NULL;
	cJSON *brandId = NULL;
	cJSON *idx = NULL;
	//cJSON *remoteName = NULL;
	//cJSON *remoteId = NULL;
	cJSON *key = NULL;

	//rid 过滤特殊字符
	char rid_mod_str[128] = {0};

	switch(type){
		//添加遥控器
		case 1:	
				//获取设备类型
				categoryId = cJSON_GetObjectItem(root, "categoryId");
				if(categoryId == NULL)
				{
					printf("%s : get categoryId failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}
				
				//获取品牌id  非机顶盒填充此项
				brandId = cJSON_GetObjectItem(root, "brandId");
				if(brandId == NULL)
				{
					printf("%s : get brandId failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}
				
				/*
				//获取品牌id 机顶盒填充此项
				cJSON *operatorId = NULL;
				operatorId = cJSON_GetObjectItem(root, "operatorId");
				if(operatorId == NULL)
				{
					printf("%s : get operatorId failed![%s]\n",__func__);
					goto error_exit;
					//获取err_no失败
				}
				*/
				
				//获取遥控器索引  
				idx = cJSON_GetObjectItem(root, "idx");
				if(idx == NULL)
				{
					printf("%s : get idx failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}

				//去掉特殊字符
				//get_idxBngo(idx->valuestring,rid_mod_str);

				sprintf(buffer,"%s%s%s%s%s%s","ADD=",categoryId->valuestring,"=",brandId->valuestring,"=",idx->valuestring);

				printf("[%s] [%d] telecom_device:%s \n",__func__,__LINE__,buffer);
			break;
			
		//删除遥控器
		case 2:
				//获取设备类型
				
				categoryId = cJSON_GetObjectItem(root, "categoryId");
				if(categoryId == NULL)
				{
					printf("%s : get categoryId failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}
				
				//获取品牌id  非机顶盒填充此项
				brandId = cJSON_GetObjectItem(root, "brandId");
				if(brandId == NULL)
				{
					printf("%s : get brandId failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}

				//获取遥控器索引  
				idx = cJSON_GetObjectItem(root, "idx");
				if(idx == NULL)
				{
					printf("%s : get idx failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}
				
				//去掉特殊字符
				get_idxBngo(idx->valuestring,rid_mod_str);

				sprintf(buffer,"%s%s%s%s%s","DELETE=",categoryId->valuestring,"=",brandId->valuestring,rid_mod_str);

				printf("[%s] [%d] telecom_device:%s \n",__func__,__LINE__,buffer);
			break;
			
		//修改遥控器别名		
		case 108:
			
			break;
			
		//获取遥控器配对规则		
		case 109:
				//获取设备类型
				
				categoryId = cJSON_GetObjectItem(root, "categoryId");
				if(categoryId == NULL)
				{
					printf("%s : get categoryId failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}
				
				//获取品牌id  非机顶盒填充此项
				
				brandId = cJSON_GetObjectItem(root, "brandId");
				if(brandId == NULL)
				{
					printf("%s : get brandId failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}
				/*
				//获取遥控器索引  
				idx = cJSON_GetObjectItem(root, "idx");
				if(idx == NULL)
				{
					printf("%s : get idx failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}	
				*/
				
				sprintf(buffer,"%s%s%s%s%s%s","MATCH=",categoryId->valuestring,"=",brandId->valuestring,"=0=",key->valuestring);

				printf("[%s] [%d] telecom_device:%s \n",__func__,__LINE__,buffer);
			break;

		//发送普通红外遥控器的按键控制命令		
		case 3:
				//获取设备类型
				categoryId = cJSON_GetObjectItem(root, "categoryId");
				if(categoryId == NULL)
				{
					printf("%s : get categoryId failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}
				
				//获取品牌id  非机顶盒填充此项
				
				brandId = cJSON_GetObjectItem(root, "brandId");
				if(brandId == NULL)
				{
					printf("%s : get brandId failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}

				/*
				//获取品牌id 机顶盒填充此项
				cJSON *operatorId = NULL;
				operatorId = cJSON_GetObjectItem(root, "operatorId");
				if(operatorId == NULL)
				{
					printf("%s : get operatorId failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}
				*/
				
				//获取遥控器索引  
				idx = cJSON_GetObjectItem(root, "idx");
				if(idx == NULL)
				{
					printf("%s : get idx failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}

				//获取遥控器索引  
				key = cJSON_GetObjectItem(root, "key");
				if(key == NULL)
				{
					printf("%s : get key failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}
				char keyBngo[20] = {0};
				strcpy(keyBngo,key->valuestring);

				//转成bngo格式的key
				get_keyBngo(keyBngo);

				//去掉特殊字符
				get_idxBngo(idx->valuestring,rid_mod_str);

				strcat(new_path,"/usr/data/");
				strcat(new_path,brandId->valuestring);
				strcat(new_path,rid_mod_str);
				strcat(new_path,".txt");

				/*
				 *  判断本地码库是否存在，如果不存在默认进入配对
				 */
				
				//码库不存在 判断是否配对按键
				if(access(new_path,F_OK) != 0){
					if(!strcmp(keyBngo,"power") || !strcmp(keyBngo,"ok") || !strcmp(keyBngo,"vol+") || !strcmp(keyBngo,"ch+") || !strcmp(keyBngo,"play") || !strcmp(keyBngo,"back")){		
						sprintf(buffer,"%s%s%s%s%s%s%s%s","MATCH=",categoryId->valuestring,"=",brandId->valuestring,"=",keyBngo,"=",idx->valuestring); 
					} else {
						return CODE_NOT_EXIST;
					}
					
				} else {
					sprintf(buffer,"%s%s%s%s%s%s%s","USE=",categoryId->valuestring,"=",brandId->valuestring,rid_mod_str,"=",keyBngo);

				}

				printf("[%s] [%d] telecom_device:%s \n",__func__,__LINE__,buffer);
			break;
			
		//发送空调遥控器控制指令
		case 4:			
				//获取品牌id  非机顶盒填充此项
				
				brandId = cJSON_GetObjectItem(root, "brandId");
				if(brandId == NULL)
				{
					printf("%s : get brandId failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}
				
				//获取遥控器索引  
				
				idx = cJSON_GetObjectItem(root, "idx");
				if(idx == NULL)
				{
					printf("%s : get idx failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}

				//获取开关  power
				cJSON *power = NULL;
				power = cJSON_GetObjectItem(root, "power");
				if(power == NULL)
				{
					printf("%s : get power failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}

				//获取模式  mode
				cJSON *mode = NULL;
				mode = cJSON_GetObjectItem(root, "mode");
				if(mode == NULL)
				{
					printf("%s : get mode failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}

				//获取温度temp
				cJSON *temp = NULL;
				temp = cJSON_GetObjectItem(root, "temp");
				if(temp == NULL)
				{
					printf("%s : get temp failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}

				//获取风速wind
				cJSON *wind = NULL;
				wind = cJSON_GetObjectItem(root, "wind");
				if(wind == NULL)
				{
					printf("%s : get wind failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}

				//获取风向swing
				cJSON *swing = NULL;
				swing = cJSON_GetObjectItem(root, "swing");
				if(swing == NULL)
				{
					printf("%s : get swing failed!\n",__func__);
					goto error_exit;
					//获取err_no失败
				}

				//获取开关、模式、温度、风速、风向
				char bgpower[5] = {0};
				char bgmode[5] = {0};
				char bgtemp[5] = {0};
				char bgwind[5] = {0};
				char bgswing[5] = {0};

				strcpy(bgpower,power->valuestring);
				strcpy(bgmode,mode->valuestring);
				strcpy(bgtemp,temp->valuestring);
				strcpy(bgwind,wind->valuestring);
				strcpy(bgswing,swing->valuestring);


				//去掉特殊字符
				get_idxBngo(idx->valuestring,rid_mod_str);

				strcat(new_path,"/usr/data/");
				strcat(new_path,brandId->valuestring);
				strcat(new_path,rid_mod_str);
				strcat(new_path,".txt");

				/*
				 *  判断本地码库是否存在，如果不存在默认进入配对
				 */
				
				//码库不存在 判断是否配对按键
				if(access(new_path,F_OK) != 0){
					/*
					if(bgpower[0] != '1'){
						return CODE_NOT_EXIST;
					}
					*/
					sprintf(buffer,"%s%s%s%s","MATCH=7=",brandId->valuestring,"=on=",idx->valuestring);	
					
				} else {
					
					char bgcommand[256] = {0};
					//查询码库的版本
					FILE *fd;
					char strMatch[RESPONSE_SIZE] = {0};
					//读取码库
					fd = fopen(new_path,"r+");
					if(fd == NULL){
						printf("%s: open Complete_code.txt error!!!!.\n",__func__);
						return CODE_NOT_EXIST;
					}
					fread(strMatch,RESPONSE_SIZE,1,fd);
					fclose(fd);
					
					char *result_json = NULL;
					result_json = strstr(strMatch,"{");
					
					//判断传过来的码库版本，跟本地数据是否一致
					int code_list_v = 0;
					if(get_code_v(result_json,&code_list_v) != 0)
						return CODE_V_ERR;
					
					int tmpInt = atoi(bgtemp);
					
					if(code_list_v == 3){
							//获取风量
							if(bgwind[0] == '0'){
								strcpy(bgwind,"s0");
							} else if(bgwind[0] == '1'){
								strcpy(bgwind,"s1");
							} else if(bgwind[0] == '2'){
								strcpy(bgwind,"s2");
							} else if(bgwind[0] == '3'){
								strcpy(bgwind,"s3");
							} else {
								return(INVALID_COMMAND);
							}
					
							//获取风量
							if(bgswing[0] == '0'){
								strcpy(bgswing,"u0_l0");
							} else if(bgswing[0] == '1'){
								strcpy(bgswing,"u1_l1");
							} else {
								return(INVALID_COMMAND);
							}
					
					
							if(bgpower[0] == '1'){
								if(bgmode[0] == '0'){
									sprintf(bgcommand,"%s%s%s%s%s","a_",bgwind,"__",bgswing,"_p0");
								
								} else if(bgmode[0] == '1'){
								
									if(tmpInt < 16 || tmpInt > 30)
										return(INVALID_COMMAND);
									
									sprintf(bgcommand,"%s%s%s%s%s%s%s","r_",bgwind,"_",bgtemp,"_",bgswing,"_p0");
									
								} else if(bgmode[0] == '2'){
									sprintf(bgcommand,"%s%s%s","d_s1__",bgswing,"_p0");
					
								}  else if(bgmode[0] == '3'){
					
									if(tmpInt < 16 || tmpInt > 30)
										return(INVALID_COMMAND);
					
									if(strcmp(bgwind,"s0") == 0)
										strcpy(bgwind,"s1");
									
									sprintf(bgcommand,"%s%s%s%s%s%s%s","w_",bgwind,"_",bgtemp,"_",bgswing,"_p0");
					
								}  else if(bgmode[0] == '4'){
									if(tmpInt < 16 || tmpInt > 30)
										return(INVALID_COMMAND);
									
									sprintf(bgcommand,"%s%s%s%s%s%s%s","h_",bgwind,"_",bgtemp,"_",bgswing,"_p0");
									
								} else {
									return(INVALID_COMMAND);
								} 
							} else {
								strcpy(bgcommand,"off");
							}
					
					} else if(code_list_v == 1){
							if(bgpower[0] == '1'){
								if(bgmode[0] == '0'){
									strcpy(bgcommand,"aa");
					
								} else if(bgmode[0] == '1'){
									if(tmpInt < 16 || tmpInt > 30)
										return(INVALID_COMMAND);
									sprintf(bgcommand,"%s%s","ar",bgtemp);
									
								} else if(bgmode[0] == '2'){
									strcpy(bgcommand,"ad");
					
								}  else if(bgmode[0] == '3'){
									strcpy(bgcommand,"aw");
					
								}  else if(bgmode[0] == '4'){
									if(tmpInt < 16 || tmpInt > 30)
										return(INVALID_COMMAND);
					
									sprintf(bgcommand,"%s%s","ah",bgtemp);
									
								} else {
									return(INVALID_COMMAND);
								} 
							} else {
								strcpy(bgcommand,"off");
							}
					} else {
						return CODE_V_ERR;
					}
					
					//USE=7=bngoIR=a_s0__u0_l0_P0
					sprintf(buffer,"%s%s%s%s%s","USE=7=",brandId->valuestring,rid_mod_str,"=",bgcommand);

				}
				
				printf("[%s] [%d] telecom_device:%s \n",__func__,__LINE__,buffer);					
			break;

		//进入学习状态	
		case 5:
			sprintf(buffer,"%s","AKEYMATCH=7=104=on");
			break;

		//bngo升级	
		case 2000:
			sprintf(buffer,"%s","UPDATER");
			update_to_mozart(buffer);
			return 0;
			break;
			
		default:
			printf("[%s] [%d] Invalid type\n",__func__,__LINE__);
			goto error_exit;
			break;
		
	}

	//组装成json格式,然后发送到mozart
	if(sendCmd_to_mozart(buffer) < 0)
		return -1;
	
	return 0;	
	
error_exit:
	return -1;
}

//获取电信请求
int get_telecomReq(char *buffer)
{
	//响应返回数据
	memset(&telecomEsp_msg,0,sizeof(telecomEsp_t));

	printf("[%s] [%d] data:%s\n",__func__,__LINE__,buffer);

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
	
	if(strcmp(headType->valuestring,"4") != 0){
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
	strcpy(telecomEsp_msg.seq,seq->valuestring);

	if(strcmp(telecomEsp_msg.seq,seq->valuestring) != 0){
		printf("get MSG_ESP[%s] faild\n",seq->valuestring);
		goto error_exit;
	}

	//获取from
	cJSON *bngoid = NULL;
	bngoid = cJSON_GetObjectItem(root, "bngoid");
	if(bngoid == NULL)
	{
		printf("%s : get bngoid failed!\n",__func__);
		goto error_exit;
		//获取err_no失败
	}
	strcpy(telecomEsp_msg.from,bngoid->valuestring);

	
	//获取request
	cJSON *resp = NULL;
	resp = cJSON_GetObjectItem(root, "resp");
	if(resp == NULL)
	{
		printf("%s : get resp failed!\n",__func__);
		goto error_exit;
		//获取err_no失败
	}
	
	char *p = cJSON_PrintUnformatted(resp);
	printf("[%s] [%d] p:%s\n",__func__,__LINE__,p);
	//发送最终的json格式到mozart
	int ret = msq_send(4781,2,p);
	free(p);
	

	cJSON_Delete(root);
	return ret;

error_exit:
	cJSON_Delete(root);
	return -1;
}


int	get_mozartEsp(int resultCode,char *resultMsg,unsigned char *buffer)
{
	//生成返回服务器json格式
	cJSON *root = NULL;
	root = cJSON_CreateObject();

	if(!root){
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
        return -1; 
	}

	// seq
    cJSON_AddItemToObject(root, "headType", cJSON_CreateString("6")); 

	// seq
    cJSON_AddItemToObject(root, "seq", cJSON_CreateString(telecomEsp_msg.seq)); 
	// from
	cJSON_AddItemToObject(root, "from", cJSON_CreateString(telecomEsp_msg.to)); 
	// to
	cJSON_AddItemToObject(root, "to", cJSON_CreateString(telecomEsp_msg.from)); 
	//type
	cJSON_AddItemToObject(root, "type", cJSON_CreateNumber(telecomEsp_msg.type)); 

	cJSON *response = NULL;
	response = cJSON_CreateObject();
	
	cJSON_AddItemToObject(root, "response", response); 
	cJSON_AddItemToObject(response, "resultCode", cJSON_CreateNumber(resultCode)); 
	cJSON_AddItemToObject(response, "resultMsg", cJSON_CreateString(resultMsg));

	char *p = cJSON_PrintUnformatted(root);
	//printf("[%s] [%d] p:%s 1111111111\n",__func__,__LINE__,p);
	memcpy(buffer,p,strlen(p));
	strcat((char *)buffer,"$_$");
	
	free(p);
	
	cJSON_Delete(root);	

	return 0;
}


int	send_loginReq(int sock)
{
	//char bngoID[30] = "SZH1-BINGOV1-80c5f2d57d01";
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
	

	szhy_send_loginReq(sock,(unsigned char *)buffer);

	return 0;
}


