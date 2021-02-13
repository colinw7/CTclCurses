CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
BIN_DIR = ../bin

all: dirs $(BIN_DIR)/CTclCurses

dirs:
	@if [ ! -e ../obj ]; then mkdir ../obj; fi
	@if [ ! -e ../bin ]; then mkdir ../bin; fi

SRC = \
CTclCurses.cpp \
CEscape.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

LIBS =

CPPFLAGS = \
-std=c++14 \
-I$(INC_DIR) \
-I. \
-I../../CReadLine/include \
-I../../CHistory/include \
-I../../CEscape/include \
-I../../CFile/include \
-I../../CStrParse/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I../../COS/include \
-I/usr/include/tcl \

LFLAGS = \
-L../../CReadLine/lib \
-L../../CFile/lib \
-L../../CStrUtil/lib \
-L../../COS/lib \
-lCReadLine \
-lCFile \
-lCStrUtil \
-lCOS \
-ltcl -lreadline -lcurses

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CTclCurses

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(BIN_DIR)/CTclCurses: $(OBJS)
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CTclCurses $(OBJS) $(LFLAGS)
