#include "include/neuronal_network.h"
#include <iostream>

using namespace std;

int main(){
    Network net(2, 2, 2, 0.5);
    vector<vector<double>> inputs = {
        {0,0}, {0,1}, {1,0}, {1,1}
    };
    vector<vector<double>> targets = {
        {1,0}, {0,1}, {0,1}, {1,0}
    };

    auto before = net.getWeights(1);
    cout << "Weights before training:\n";
    for(const auto& vec : before){
        for(double v : vec) cout << v << ' ';
        cout << '\n';
    }

    net.train(inputs, targets, 1000);

    auto after = net.getWeights(1);
    cout << "Weights after training:\n";
    for(const auto& vec : after){
        for(double v : vec) cout << v << ' ';
        cout << '\n';
    }

    return 0;
}