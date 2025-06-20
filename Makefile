CXX = g++
CXXFLAGS = -Wall -I./include

TARGET = test_network
SRCS = test_network.cpp src/neuronal_network.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
