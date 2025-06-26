CXX      := g++
CXXFLAGS := -std=c++17 -Iinclude -Wall -Wextra -pedantic
TARGET   := main

SRC := \
    main.cpp \
    src/perceptron.cpp \
    src/neural_network.cpp \
    src/activation_functions/step.cpp \
    src/activation_functions/relu.cpp \
    src/activation_functions/softmax.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)

.PHONY: all clean
all: $(TARGET)