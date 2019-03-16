#ifndef __SZHY_GET_H_
#define __SZHY_GET_H_

//bngo与服务器的通信接口
//cmd: del_alarm  data:闹钟id
//cmd: del_cozy   data:定时id
//cmd: del_code   data:码库id
int szhy_get(char *cmd,char *data);

//从服务器获取时间
int get_time_server(char *rsp);

//BNGO注册
int register_bngo(char *bngo_aes,char *city_urlencode,char *rsp);

//获取服务器ip 端口
int getTcpIP(char *rsp);

#endif //__SZHY_GET_H_