CXX = g++
CXXFLAGS = -Wall -I./include

TARGET = main
SRCS = main.cpp src/neuronal_network.cpp src/activation_function.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
