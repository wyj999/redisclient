TARGET = libredisclient.a

CXXFLAGS = -O0 -Wall
LDFLAGS =  -lhiredis
AR = ar
CC = g++

OBJS = RedisClient.o \
	   RedisPool.o \
	   RedisException.o

all: $(TARGET)
$(TARGET): $(OBJS)
	$(AR) rcs $(TARGET) $(OBJS)

%.o:%.cc
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
