CXX      := g++
CXXFLAGS := -std=c++17 -Iinclude -Wall -Wextra -pedantic
TARGET   := main

SRC := \
    main.cpp \
    data.cpp \
    preprocess.cpp \
    src/perceptron.cpp \
    src/neural_network.cpp \
    src/activation_functions/step.cpp \
    src/activation_functions/relu.cpp \
    src/activation_functions/softmax.cpp \
    src/weights_initialization/kaiming.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)

.PHONY: all clean
all: $(TARGET)