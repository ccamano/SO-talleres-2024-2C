#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
        
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a' < 0 ? clave[0] - 'A' : clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    // Completar (Ejercicio 2)
    if(!(clave[0] > 'A' && clave[0] < 'Z') && !(clave[0] > 'a' && clave[0] < 'z')){
        std::cout << "Producto inválido: "  << clave << std::endl;
    }
    else{
        unsigned int hash = hashIndex(clave);
        hashMapPair new_key = hashMapPair(clave,1);
       
        writer_mutex[hash].lock();
            auto it = tabla[hash]->begin();
            while(it != tabla[hash]->end() && std::get<0>(*it) != clave){
                it++;
            }
            if(it!=tabla[hash]->end()){
                *it = hashMapPair(std::get<0>(*it),std::get<1>(*it)+1);
            }
            else{
                tabla[hash]->insertar(new_key);
            }
        writer_mutex[hash].unlock();

    }
}

std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2)
    std::vector<std::string> res = {};
    return res;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // Completar (Ejercicio 2)
    return 0;
}

float HashMapConcurrente::promedio() {

    float sum = 0.0;
    unsigned int count = 0;
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (const auto& p : *tabla[index]) {
            sum += p.second;
            count++;
        }
    }
    if (count > 0) {
        return sum / count;
    }
    return 0;        
}



#endif
