## 其余变量
VER:=release ## 调试选项
OPTM:=elide ## 构造器优化选项
STD:=c++17 ## C++标准
CPP:=g++

## 头文件
HEADER:=include
## 可执行程序
EXE_RECOMMENDER:=bin/offline1
EXE_SEARCHER:=bin/offline2
EXE_OFFLINE:= $(EXE_RECOMMENDER) $(EXE_SEARCHER)
EXE_SERVER:=bin/server
EXE_CLIENT:=bin/client
EXES:= $(EXE_OFFLINE) $(EXE_SERVER) $(EXE_CLIENT)
## 源程序
SRC_RECOMMENDER:=$(wildcard src/offline/module1/*.cpp)
SRC_SEARCHER:=$(wildcard src/offline/module2/*.cpp)
SRC_SERVER:=$(wildcard src/online/*.cpp) $(wildcard src/online/net/*.cpp)
SRC_CLIENT:=$(wildcard src/client/*.cpp)

## 测试变量
# all:
# 	echo $(SRC_SERVER) 

## 编译参数确定
ifeq ($(VER), debug) ## ifeq后要有空格
	OPT+=-g
	OPT+=-D __DEBUG__
endif

ifeq ($(OPTM), no-elide)
	OPT+=-fno-elide-constructors
endif

## 全编译（默认）
all:$(EXES) 
## 仅编译offline
offline:$(EXE_OFFLINE)
## 仅编译recommender
recommender:$(EXE_RECOMMENDER)
## 仅编译searcher
searcher:$(EXE_SEARCHER)
## 仅编译server
server:$(EXE_SERVER)
## 仅编译client
client:$(EXE_CLIENT)

## recommender的编译与链接
$(EXE_RECOMMENDER):$(SRC_RECOMMENDER)
	$(CPP) $^ -o $@ -Wall -O0 -llog4cpp -lredis++ -lhiredis -lpthread -I $(HEADER) -std=$(STD) $(OPT)
## searcher的编译与链接
$(EXE_SEARCHER):$(SRC_SEARCHER)
	$(CPP) $^ -o $@ -Wall -O0 -llog4cpp -lredis++ -lhiredis -lpthread -I $(HEADER) -std=$(STD) $(OPT)
## server的编译与链接
$(EXE_SERVER):$(SRC_SERVER)
	$(CPP) $^ -o $@ -Wall -O0 -llog4cpp -lredis++ -lhiredis -lpthread -I $(HEADER) -std=$(STD) $(OPT)
## client的编译与链接
$(EXE_CLIENT):$(SRC_CLIENT)
	$(CPP) $^ -o $@ -Wall -O0 -llog4cpp -lredis++ -lhiredis -lpthread -I $(HEADER) -std=$(STD) $(OPT)

## 伪目标
.PHONY:clean rebuild
clean:
	$(RM) $(EXES)
rebuild:clean $(EXES)