#��ִ���ļ�
target = szhy_content
CFLAGS= -Wall -O2
#Դ�ļ�
SRCS = main.c szhyPacket.c telecom.c cJSON.c queueMsg.c content_http.c
#������
COMMAND=-L/yuan/BedroomElves/BedroomElves-szhy/bngo_2.0/https/lib -lcurl -lssl -lcrypto -lz -lm -lbngo_info -lbngo_aes -lini -lpthread -lszhy_get
#����Ŀ��
objects = $(SRCS:.c=.o)
#ָ�������
CC = mips-linux-gnu-gcc
#CC = gcc
$(target):$(objects)
	mips-linux-gnu-g++ -o $(target) $(objects) $(COMMAND)
#	g++ -o $(target) $(objects)
$(objects): szhyPacket.h
	$(CC) $(CFLAGS) -c $(SRCS)
.PHONY:clean
clean:
	rm -rf $(target) $(objects)
