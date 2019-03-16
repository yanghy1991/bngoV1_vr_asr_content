#ifndef __TELECOM_H_
#define __TELECOM_H_
#include "cJSON.h"

//发送数据
int szhy_send_msg(int sock,unsigned char *buffer);
//接收数据
int szhy_read_msg(int sock,unsigned char *buffer,int buflen);

//发送心跳请求
int szhy_send_pingReq(int sock);
//接收心跳响应
int szhy_read_pingEsp(int sock,unsigned char *buffer,int buflen);

//发送注册请求
int	szhy_send_loginReq(int sock);
//接收注册响应
int szhy_read_loginEsp(int sock,unsigned char *buffer,int buflen);


//发送cmd到合一服务器
int	send_szhyServerReq(int sock,char *cmd);
//获取合一服务器响应
int get_szhyServerEsp(char *buffer);

#endif //__TELECOM_H_