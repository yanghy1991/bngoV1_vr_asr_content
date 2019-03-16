#ifndef __TELECOM_H_
#define __TELECOM_H_
#include "cJSON.h"

enum error_list{
	SECRET_KEY_ERR = 1,
	ACTIVATION_CODE_ERR,
	ELECTRICAL_LIST_ERR,
	BRAND_LIST_ERR,
	MATCH_GCODE_ERR,
	MATCH_SCODE_ERR,
	COMPLETE_GCODE_ERR,
	COMPLETE_SCODE_ERR,
	INVALID_COMMAND,
	CODE_NOT_EXIST,
	OPEN_FILE_ERR,			//打开文件失败  11 
	INVALID_OLD_COMMAND,	//历史命令无效  12 
	READ_IR_LEARN_ERR,		//匹配超时  13
	LEARN_PATH_EMPTY,
	SEND_IR_LEARN_ERR,
	CODE_V_ERR,
	SETIR_FILE_ERR,
	AKEYMATCH_GCODE_ERR,
	IR_LEARN_EXIT,			//退出红外学习模式 19
	SET_ASR_IR,
	ADD_CODE_ERR,
	DEL_CODE_ERR,
	LEARN_MOD_ERR,
	LEARN_SED_ERR,
	JSON_PARESE_ERR,		//码库 json解析出错 25
	JSON_KEY_NOT_EXIST,		//码库没有该按键信息 26
	JSON_CODE_ERR,          //码库存在error 27
	BIND_NOT_EXIST,			//绑定关系不存在error 28
	NOCH_ERR,				//没有添加频道列表 29
};
	
//获取电信请求
int get_telecomReq(char *buffer);
//根据type类型，转成相应的控制命令
int get_telecomReq_command(cJSON *root,int type);
//组装成json格式，然后发送到mozart
int sendCmd_to_mozart(char *buffer);

////获取mozart返回响应
int	get_mozartEsp(int resultCode,char *resultMsg,unsigned char *buffer);

int	send_loginReq(int sock);

int update_to_mozart(char *buffer);

#endif //__TELECOM_H_