#可执行文件
target = szhy_content
CFLAGS= -Wall -O2
#源文件
SRCS = main.c szhyPacket.c telecom.c cJSON.c queueMsg.c content_http.c
#依赖库
COMMAND=-L/yuan/BedroomElves/BedroomElves-szhy/bngo_2.0/https/lib -lcurl -lssl -lcrypto -lz -lm -lbngo_info -lbngo_aes -lini -lpthread -lszhy_get
#依赖目标
objects = $(SRCS:.c=.o)
#指令编译器
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
