#ifndef NEURONAL_NETWORK_H
#define NEURONAL_NETWORK_H

int N_WEIGHT = 6;

#include <vector>

using namespace std;

class Neuron {
    public:
        Neuron(int);
        void initWeights();
        double getOutputValue() const { return outputValue; }
        vector<double>& getOutputWeights() { return weights; }
        double outputValue;
        vector<double> weights;
        int myIndex;

};

class Layer {
    public:
        Layer(int);
        void initNeurons(int);
        vector<Neuron>& getNeurons() { return neurons; }
        vector<Neuron> neurons;
        vector<double> bias;
};

class Network {
    public:
        Network(int, int, int, vector<unsigned>);
        void add_layer(Layer);
        void train(int epochs);
        void forward_propagate(vector<double>);
        void back_propagate(vector<double>);
        vector<double> softmax(const vector<double>&);
        double sigmoid(double);
        double sigmoid_derivative(double);
        double relu(double);


    private:
        double learning_rate;
        vector<Layer> layers;
        int n_layers;
        vector<vector<double>> inputData;
        vector<vector<double>> outputData;
};

#endif