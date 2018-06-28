//
//  neural_network.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/20/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include "neural_network.h"


NeuralNetwork::NeuralNetwork(Robot &rob, int numHidden, int dimHidden){
    rob_ptr = &rob;
    const int numLayers = 2 + numHidden;
    const int numWeights = 1 + numHidden;
    weights = std::vector<Eigen::MatrixXd>(numWeights);
    layers = std::vector<std::vector<glm::vec3>>(numLayers);
    
    //std::cout << "weights.size(): " << weights.size() << std::endl;
    for (int i=0; i<weights.size(); i++){
        int previousSize = dimHidden;
        int nextSize = dimHidden;
        if (i == 0)
            previousSize = rob.getSprings().size();
        if (i == weights.size()-1)
            nextSize = rob.getSprings().size();
        assert(nextSize == dimHidden || nextSize == 81);
        assert(previousSize == dimHidden || previousSize == 81);
        weights[i] = Eigen::MatrixXd::Random(previousSize,nextSize);
       // std::cout << weights[0].cols() << " " << i << std::endl;
    }
    
    for (int i=0; i<numLayers; i++){
        if (i == 0 || i == numLayers-1){
            std::vector<glm::vec3> edgeLayer;
            for (Spring *s : rob.getSprings())
                edgeLayer.push_back(glm::vec3(s->calcCenter()));
            layers[i] = edgeLayer;
        } else {
            std::vector<glm::vec3> layer;
            for (int i=0; i<dimHidden; i++)
                layer.push_back(rob.calcCentroid());
            layers[i] = layer;
        }
    }
    
    //calculateNeuronPositions();
}

NeuralNetwork& NeuralNetwork::operator=(const NeuralNetwork& old_nn){
    weights = old_nn.weights;
    rob_ptr = old_nn.rob_ptr;
    
    const int numLayers = old_nn.layers.size();
    assert(old_nn.layers.size() > 2);
    const int dimHidden = old_nn.layers[1].size();
    layers = std::vector<std::vector<glm::vec3>>(numLayers);
    for (int i=0; i<numLayers; i++){
        if (i == 0 || i == numLayers-1){
            std::vector<glm::vec3> edgeLayer;
            for (Spring *s : rob_ptr->getSprings())
                edgeLayer.push_back(glm::vec3(s->calcCenter()));
            layers[i] = edgeLayer;
        } else {
            std::vector<glm::vec3> layer;
            for (int i=0; i<dimHidden; i++)
                layer.push_back(rob_ptr->calcCentroid());
            layers[i] = layer;
        }
    }
    
    return *this;
}

void NeuralNetwork::calculateNeuronPositions(){
    //Setting positions for layers:
    
    
    for (int k=0; k<6; k++){//arbitrary number of cycles
        for (int r=1; r<layers.size()-1; r++){//for each hidden layer
            assert(layers.size() > 2);
            for (int i=0; i<layers[1].size(); i++){//nodes in current row
                glm::vec3 average;
                float totalWeight = 0.0f;
                //next nodes
                for (int j=0; j<layers[r+1].size(); j++){
                    assert(layers[r+1][j][0] < 100);
                    const float singleWeight = fabsf(float((weights[r])(i,j)));
                    average += layers[r+1][j]*singleWeight;
                    totalWeight += singleWeight;
                }
                
                //previous nodes
                for (int j=0; j<layers[r-1].size(); j++){
                    assert(layers[r-1][j][0] < 100);
                    const float singleWeight = fabsf(float((weights[r-1])(j,i)));
                    average += layers[r-1][j]*singleWeight;
                    totalWeight += singleWeight;
                }
                assert(!isnan(average[0]));
                average *= 1.0f/totalWeight;
                assert(!isnan(average[0]));
                layers[r][i] = average;
            }
        }
    }
    
    
    
    
    //PRINTING NODE LOCS
    for (int i=0; i<layers.size(); i++){//layers
        for (int j=0; j<layers[i].size(); j++)//nodes
            std::cout<<"("<<layers[i][j][0]<<","<<layers[i][j][1]<<","<<layers[i][j][2]<<")  ";
        std::cout << "\n---------------------------------------------\n";
    }

    
}

std::vector<float> NeuralNetwork::evaluate(std::vector<float> input){
    assert(input.size() == weights[0].rows());
    Eigen::MatrixXd data(1,input.size());
    for (int i=0; i<input.size(); i++)
        data(0,i) = input[i];
    
    
    for (int i=0; i<weights.size(); i++){
        assert(data.cols() == weights[i].rows());
        data *= weights[i];
        if (i < weights.size()-1)
            nn_helper::activate(data);
    }
    
    const int outputSize = weights[weights.size()-1].cols();
    std::vector<float> output(outputSize);
    for (int i=0; i<outputSize; i++)
        output[i] = data(0,i);
    return output;
}

NeuralNetwork NeuralNetwork::crossOver(NeuralNetwork &nn){
    assert(layers.size()>2);
    NeuralNetwork output = *this;
    
    
    const int i = rand()%(output.weights.size());
    std::cout << "THIS IS I: " << i << std::endl;
    assert(output.weights[i].rows() >= 5 && output.weights[i].cols()>=5);
    const int r1 = (rand()%(output.weights[i].rows()-4))+1;
    const int r2 = (rand()%(output.weights[i].rows()-r1-3))+r1+2;
    const int c1 = (rand()%(output.weights[i].cols()-4))+1;
    const int c2 = (rand()%(output.weights[i].cols()-c1-3))+c1+2;
    
    assert(r1>0);
    assert(r2<output.weights[i].rows()-1);
    assert(r2-r1 > 1);
    
    const int r_section = rand()%3;
    const int c_section = rand()%3;
    int r_start = 0;
    int r_end = r1;
    int c_start = 0;
    int c_end = c1;
    
    if (r_section == 1){
        r_start = r1;
        r_end = r2;
    } else if (r_section == 2){
        r_start = r2;
        r_end = output.weights[i].rows();
    }
    
    if (c_section == 1){
        c_start = c1;
        c_end = c2;
    } else if (c_section == 2){
        c_start = c2;
        c_end = output.weights[i].cols();
    }
    
    std::cout << "r_start: " << r_start << std::endl;
    std::cout << "c_start: " << c_start << std::endl;
    
    output.weights[i].block(r_start,c_start,r_end-r_start,c_end-c_start) = nn.weights[i].block(r_start,c_start,r_end-r_start,c_end-c_start);
    
    return output;
}

NeuralNetwork &NeuralNetwork::mutate(double a){
    
    const int i = rand()%(weights.size());
    const int r = rand()%(weights[i].rows());
    const int c = rand()%(weights[i].cols());
    
    std::cout << i << std::endl;
    std::cout << r << std::endl;
    std::cout << c << std::endl;
    
    glm::vec3 p2 = layers[i+1][c];
    glm::vec3 p1 = layers[i][r];
    const double distance = glm::distance(p2,p1);
    assert(distance != 0);
    
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0,1.0/pow(distance,a));
    double number = distribution(generator);
    std::cout << "LOOKK: " << (1.0/pow(distance,a)) << std::endl;//THINK ABOUT THIS
    std::cout << "LOOKK: " << number << std::endl;
    weights[i](r,c) = float(number);
    
    return *this;
}

std::ostream &operator<<(std::ostream &os, NeuralNetwork &nn){
    for (int i=0; i<nn.weights.size(); i++)
        os << (nn.weights[i]).transpose() << "\n\n";
    return os;
}
