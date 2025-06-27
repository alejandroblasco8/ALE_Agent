CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Iinclude
LDFLAGS  := -Wl,-rpath=. -LSDL
LIBS     := libale.so

# Objetivo por defecto: compilar todos
all: main perceptronMin NNMin

# Compilar main
main: main.cpp data.cpp preprocess.cpp \
      src/perceptron.cpp src/neural_network.cpp \
      src/activation_functions/step.cpp \
      src/activation_functions/relu.cpp \
      src/activation_functions/softmax.cpp \
      src/weights_initialization/kaiming.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compilar perceptronMin
perceptronMin: src/ML_IA_Perceptron.cpp src/activation_functions/step.cpp
	$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

# Compilar NNMin
NNMin: src/ML_IA_NN.cpp \
       src/neural_network.cpp \
       src/weights_initialization/kaiming.cpp \
       src/activation_functions/relu.cpp \
       src/activation_functions/softmax.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) $(LDFLAGS)

# Limpiar
clean:
	rm -f main perceptronMin NNMin

.PHONY: all clean

