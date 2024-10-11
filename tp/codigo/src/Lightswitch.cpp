#ifndef LIGHTSWITCH_CPP
#define LIGHTSWITCH_CPP

#include <mutex>
#include <iostream>
#include <stdio.h>

#include "Lightswitch.hpp";

using namespace std;

Lightswitch::Lightswitch() : counter(0), l_mutex() {};


void Lightswitch::lock(mutex &semaforo) {
    this->l_mutex.lock();
    this->counter++;
    if (this->counter == 1) semaforo.lock();
    this->l_mutex.unlock();
}

void Lightswitch::unlock(mutex &semaforo) {
    this->l_mutex.lock();
    this->counter--;
    if (this->counter == 0) semaforo.unlock();
    this->l_mutex.unlock();
}

#endif