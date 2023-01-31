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
	@if [ ! -e ../lib ]; then mkdir ../lib; fi
	@if [ ! -e ../bin ]; then mkdir ../bin; fi

SRC = \
CTclCurses.cpp \
CEscape.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

LIBS =

CPPFLAGS = \
-std=c++17 \
-I$(INC_DIR) \
-I. \
-I../../CReadLine/include \
-I../../CHistory/include \
-I../../CImageLib/include \
-I../../CFile/include \
-I../../CStrParse/include \
-I../../CMath/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I../../COS/include \
-I/usr/include/tcl \

LFLAGS = \
-L../../CReadLine/lib \
-L../../CImageLib/lib \
-L../../CFile/lib \
-L../../CFileUtil/lib \
-L../../CRGBName/lib \
-L../../CMath/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../COS/lib \
-lCReadLine \
-lCImageLib \
-lCFile \
-lCFileUtil \
-lCRGBName \
-lCMath \
-lCStrUtil \
-lCRegExp \
-lCOS \
-ltcl -lreadline -lcurses \
-ljpeg -lpng -ltre

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CTclCurses

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(BIN_DIR)/CTclCurses: $(OBJS)
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CTclCurses $(OBJS) $(LFLAGS)
