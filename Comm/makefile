# 编译器
CXX = g++
# 编译器选项
CXXFLAGS = -std=c++11 -Wall
# 目标可执行文件
TARGET = test

# 源文件
SRCS = test.cpp
# 头文件
HEADERS = log.hpp util.hpp
# 目标文件（生成的对象文件）
OBJS = $(SRCS:.cpp=.o)

# 生成目标
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# 编译 .cpp 文件为 .o 文件
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理编译生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean