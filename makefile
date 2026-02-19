# Makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I.
LDFLAGS = -lmysqlclient -lpthread -ljsoncpp -lcrypt -lssl -lcrypto

# 源文件
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = server

# 默认目标
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)