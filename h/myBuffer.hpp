//
// Created by os on 7/24/22.
//

#ifndef PROJECT_BASE_MYBUFFER_HPP
#define PROJECT_BASE_MYBUFFER_HPP

#include "../h/Sem.hpp"
class myBuffer {
private:
    int cap;
    char *buffer;
    int head, tail;

    Sem* spaceAvailable;
    Sem* itemAvailable;

public:
    myBuffer(int _cap);
    ~myBuffer();

    static myBuffer* createBuffer(int _cap);
    void put(char c);
    char get();

    int getCnt();

};


#endif //PROJECT_BASE_MYBUFFER_HPP
