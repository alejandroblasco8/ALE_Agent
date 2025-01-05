#include "neuronal_network.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <algorithm>

Neuron::Neuron(int index) {
    //Inicializar pesos teniendo en cuenta el número de pesos (n_weights)
    this->initWeights();

    //Atributo para facilitar la identificación de cada neurona (se quitará si no ayuda)
    this->myIndex = index;
}

void
Neuron::initWeights(){
    for(int i = 0; i < N_WEIGHT; i++){
        //Habrá que modificar el 0 por pesos random
        weights.push_back(0);
    }
}

Layer::Layer(int n_neurons) {
    this->initNeurons(n_neurons);
}

void
Layer::initNeurons(int n_neurons) {
    for(int i = 0; i < n_neurons; i++){
        neurons.push_back(Neuron(i));
        bias.push_back(0); //Añadir número aleatorio
    }
}

Network::Network(int n_inputs, int n_hidden, int n_outputs, vector<unsigned> data) {
    this->n_layers = 0;
    //Añadimos la capa de entrada.
    this->add_layer(Layer(n_inputs, n_inputs));
    //Añadimos la capa oculta. Añadimos 1 al segundo argumento para el bias.
    this->add_layer(Layer(n_hidden, n_inputs+1));
    //Añadimos capa de salida. Si es, por ejemplo, binaria, n_outputs será 2
    this->add_layer(Layer(n_outputs, n_hidden+1));
}

void
Network::add_layer(Layer new_layer) {
    layers.push_back(new_layer);
    this->n_layers++;
}


vector<double>
Netowork::softmax(const vector<double>& inputs) {
    vector<double> outputs(inputs.size());
    double maxInput = *max_element(inputs.begin(), inputs.end());
    double sumExp = 0.0;

    for (double val : inputs) {
        sumExp += exp(val - maxInput);
    }

    for (size_t i = 0; i < inputs.size(); i++) {
        outputs[i] = exp(inputs[i] - maxInput) / sumExp;
    }

    return outputs;
}

double
Network::sigmoid(double x){
    return 1.0 / (1.0 + exp(-x));
}

double
Network::sigmoid_derivative(double x){
    return sigmoid(x) * (1 - sigmoid(x));
}

double
Network::relu(double x){
    return x > 0 ? x : 0;
}


void
Network::forward_propagate(vector<double> input){
    //Propagación hacia adelante de la capa de entrada
    for(int j = 0; j < layers[0].neurons.size(); j++){
        layers[0].neurons[j].outputValue = input[j];
    }

    //Propagación hacia adelante de las capas ocultas
    for(int j = 1; j < layers.size() - 1; j++){
        vector<Neuron>& prev_neurons = layers[j - 1].neurons;
        for (int n = 0; n < layers[j].neurons.size(); n++) {
            double sum = 0.0;
            for (int i = 0; i < prev_neurons.size(); i++) {
                sum += prev_neurons[i].getOutputValue() * prev_neurons[i].weights[n];
            }
            sum += layers[j].bias[n];
            layers[j].neurons[n].outputValue = sigmoid(sum);
        }
    }

    //Propagación hacia adelante de la capa de salida (softmax)
    vector<double> outputs;
    for (Neuron& neuron : layers.back().neurons) {
        outputs.push_back(neuron.outputValue);
    }
    vector<double> softmaxOutputs = softmax(outputs);

    for (int i = 0; i < layers.back().neurons.size(); i++) {
        layers.back().neurons[i].outputValue = softmaxOutputs[i];
    }

}

void
Network::back_propagate(vector<double> target){

    Layer& outputLayer = layers.back();
    Layer& prevLayer = layers[layers.size() - 2];

    // Cálculo del gradiente de salida = y*(output-target)
    for (int i = 0; i < outputLayer.neurons.size(); i++) {
        double output = outputLayer.neurons[i].getOutputValue();
        double delta = output - target[i]; // Gradiente simplificado para softmax + cross-entropy

        // Actualizar pesos y bias
        for (int j = 0; j < prevLayer.neurons.size(); j++) {
            prevLayer.neurons[j].weights[i] -= learning_rate * delta * prevLayer.neurons[j].getOutputValue();
        }
        outputLayer.bias[i] -= learning_rate * delta;
    }

    // Propagar el error hacia atrás en las capas ocultas
    for (int l = layers.size() - 2; l > 0; l--) {
        Layer& hiddenLayer = layers[l];
        Layer& prevHiddenLayer = layers[l - 1];

        for (int i = 0; i < hiddenLayer.neurons.size(); i++) {
            double error = 0.0;
            for (Neuron& nextNeuron : layers[l + 1].neurons) {
                error += nextNeuron.weights[i] * (nextNeuron.outputValue - target[i]);
            }
            double gradient = error * sigmoid_derivative(hiddenLayer.neurons[i].getOutputValue());

            // Actualizar pesos y bias
            for (int j = 0; j < prevHiddenLayer.neurons.size(); j++) {
                prevHiddenLayer.neurons[j].weights[i] -= learning_rate * gradient * prevHiddenLayer.neurons[j].getOutputValue();
            }
            hiddenLayer.bias[i] -= learning_rate * gradient;
        }
    }
}

void
Network::train(int epochs){
    double error;
    int size = inputData.size();
    for(int i = 0; i < epochs; i++){
        int index = rand() % size;
        forward_propagate(inputData[index]);
        back_propagate(outputData[index]);
    }
}