#OBJS define specifies which files to compile
OBJS = TA.cpp

#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -Wall enables all warnings
# -g enables debugging
COMPILER_FLAGS = -Wall -g -lpthread

#OBJ_NAME specifies the name of our  program executable
OBJ_NAME = TASync

#This is the target that compiles our executable
all: $(OBJS)
		$(CC) $(OBJS) $(COMPILER_FLAGS) -o $(OBJ_NAME)