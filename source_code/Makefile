CC=g++
CFLAGS=-W -Wall
LDFLAGS=-lrt

DIR_OBJ=../ebtehal/lib/
BINDIR = bin

SRC =  main.cpp\
       maps/Map.cpp\
       maps/OccupancyGridMap.cpp\
       
     
EXEC=main
OBJS = $(SRC:.cpp=.o)
DIR_OBJS=$(addprefix $(DIR_OBJ), $(OBJS))
 
all: $(EXEC)

main: $(DIR_OBJS)
	$(CC) -o $(BINDIR)/$@ $^ $(LDFLAGS)
 
$(DIR_OBJ)%.o: $(DIR)%.cpp
	$(CC) -o $@ -c $< $(CFLAGS)
