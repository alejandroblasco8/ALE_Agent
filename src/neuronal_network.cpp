#include "../include/neuronal_network.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <algorithm>

Neuron::Neuron(int index) {
    this->myIndex = index;
    outputValue = 0.0;
}

void
Neuron::initWeights(int n_outputs){
    weights.clear();
    for(int i = 0; i < n_outputs; i++){
        double w = static_cast<double>(rand()) / RAND_MAX - 0.5;
        weights.push_back(w);
    }
}

Layer::Layer(int n_neurons) {
    this->initNeurons(n_neurons);
}

void
Layer::initNeurons(int n_neurons) {
    for(int i = 0; i < n_neurons; i++){
        neurons.push_back(Neuron(i));
        double b = static_cast<double>(rand()) / RAND_MAX - 0.5;
        bias.push_back(b);
    }
}

Network::Network(int n_inputs, int n_hidden, int n_outputs, double lr) {
    this->n_layers = 0;
    learning_rate = lr;
    srand(time(nullptr));
    // Input layer
    this->add_layer(Layer(n_inputs));
    // Hidden layer
    this->add_layer(Layer(n_hidden));
    // Output layer
    this->add_layer(Layer(n_outputs));
}

void
Network::add_layer(Layer new_layer) {
    if(!layers.empty()) {
        int nextSize = new_layer.neurons.size();
        for(Neuron& n : layers.back().neurons){
            n.initWeights(nextSize);
        }
    }
    layers.push_back(new_layer);
    this->n_layers++;
}


vector<double>
Network::softmax(const vector<double>& inputs) {
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
Network::forward_propagate(const vector<double>& input){
    //Propagación hacia adelante de la capa de entrada
    for(int j = 0; j < layers[0].neurons.size(); j++){
        layers[0].neurons[j].outputValue = input[j];
    }

    for(size_t l = 1; l < layers.size(); ++l){
        Layer& prev = layers[l-1];
        Layer& curr = layers[l];
        vector<double> sums(curr.neurons.size(), 0.0);
        for(size_t n = 0; n < curr.neurons.size(); ++n){
            double sum = curr.bias[n];
            for(size_t p = 0; p < prev.neurons.size(); ++p){
                sum += prev.neurons[p].outputValue * prev.neurons[p].weights[n];
            }
            sums[n] = sum;
        }
        if(l == layers.size()-1){
            vector<double> out = softmax(sums);
            for(size_t n=0; n<curr.neurons.size(); ++n)
                curr.neurons[n].outputValue = out[n];
        }else{
            for(size_t n=0; n<curr.neurons.size(); ++n)
                curr.neurons[n].outputValue = sigmoid(sums[n]);
        }
    }

}

void
Network::back_propagate(const vector<double>& target){

    Layer& outputLayer = layers.back();
    Layer& prevLayer = layers[layers.size() - 2];

    vector<double> next_deltas(outputLayer.neurons.size());

    for (size_t i = 0; i < outputLayer.neurons.size(); ++i) {
        double output = outputLayer.neurons[i].getOutputValue();
        double delta = output - target[i];
        next_deltas[i] = delta;
        for (size_t j = 0; j < prevLayer.neurons.size(); ++j) {
            prevLayer.neurons[j].weights[i] -= learning_rate * delta * prevLayer.neurons[j].getOutputValue();
        }
        outputLayer.bias[i] -= learning_rate * delta;
    }

    for (int l = layers.size() - 2; l > 0; --l) {
        Layer& hiddenLayer = layers[l];
        Layer& prevHiddenLayer = layers[l - 1];
        vector<double> curr_deltas(hiddenLayer.neurons.size(), 0.0);

        for (size_t i = 0; i < hiddenLayer.neurons.size(); ++i) {
            double error = 0.0;
            for (size_t k = 0; k < next_deltas.size(); ++k) {
                error += hiddenLayer.neurons[i].weights[k] * next_deltas[k];
            }
            double delta = error * sigmoid_derivative(hiddenLayer.neurons[i].getOutputValue());
            curr_deltas[i] = delta;

            for (size_t j = 0; j < prevHiddenLayer.neurons.size(); ++j) {
                prevHiddenLayer.neurons[j].weights[i] -= learning_rate * delta * prevHiddenLayer.neurons[j].getOutputValue();
            }
            hiddenLayer.bias[i] -= learning_rate * delta;
        }
        next_deltas = curr_deltas;
    }
}

void
Network::train(const vector<vector<double>>& inputs,
               const vector<vector<double>>& targets,
               int epochs){
    for(int i = 0; i < epochs; i++){
        int index = rand() % inputs.size();
        forward_propagate(inputs[index]);
        back_propagate(targets[index]);
    }
}

vector<vector<double>>
Network::getWeights(int layer_index) const {
    vector<vector<double>> w;
    if(layer_index <= 0 || layer_index >= layers.size())
        return w;
    const Layer& prev = layers[layer_index-1];
    for(const Neuron& n : prev.neurons) {
        w.push_back(n.weights);
    }
    return w;
}
