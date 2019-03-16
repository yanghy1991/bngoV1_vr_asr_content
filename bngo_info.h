#ifndef __BNGO_INFO_H_
#define __BNGO_INFO_H_


//获取设备id aes 加密
int get_bngoID(char *bngoId);
//获取设备id 没有经过加密
int get_bngoID_mqttpub(char *bngoId);

#endif //__BNGO_INFO_H_

