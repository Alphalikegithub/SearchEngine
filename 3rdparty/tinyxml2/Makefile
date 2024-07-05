VER:=release ## 调试选项
OPTM:=elide ## 构造器优化选项
STD:=c++11

SRCS:=$(wildcard *.cpp)
EXES:=main
CPP:=g++

ifeq ($(VER), debug) ## ifeq后要有空格
	OPT+=-g
endif

ifeq ($(OPTM), no-elide)
	OPT+=-fno-elide-constructors
endif

$(EXES):$(SRCS)
	$(CPP) $^ -o $@ -Wall -O0 -llog4cpp -lpthread -std=$(STD) $(OPT) 

.PHONY:clean rebuild
clean:
	$(RM) $(EXES)
rebuild:clean $(EXES)
