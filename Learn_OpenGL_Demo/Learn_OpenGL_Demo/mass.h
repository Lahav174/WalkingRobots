//
//  mass.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/3/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef mass_h
#define mass_h

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "spring.h"
#include <unordered_set>
#include <random>

const float MASS_WEIGHT = 0.2;

//int randInt(int min, int max){
//    return (rand() % (max - min + 1) + min);
//}

class Mass {
        
public:
    std::unordered_set<Spring *> springs;
    double a=0.0;
    double m;
    glm::vec3 v = glm::vec3(0,0,0);
    glm::vec3 pos;
    
    Mass(glm::vec3 position, double mass):pos(position),m(mass){}
    
    glm::vec3 *mp(){
        return &pos;
    }
    
};

#endif /* mass_h */
