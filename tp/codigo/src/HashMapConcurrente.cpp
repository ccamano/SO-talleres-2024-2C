#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include "HashMapConcurrente.hpp"

using namespace std;

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(string clave) {
    return (unsigned int) (tolower(clave[0]) - 'a');
}

void HashMapConcurrente::incrementar(string clave) {
  /*   Si clave existe en la tabla (en la lista
del bucket correspondiente), se debe incrementar su valor en uno. Si no
existe, se debe crear el par <clave, 1> y agregarlo a la tabla. Se debe
garantizar que solo haya contencion en caso de colision de hash; es
decir, si dos o mas threads intentan incrementar concurrentemente claves
que no colisionan, deben poder hacerlo sin inconvenientes */


    // (Ejercicio 2)
    unsigned int indice = HashMapConcurrente::hashIndex(clave);
    
    this->_buckets_lock[indice].lock();

    unsigned int valor = HashMapConcurrente::valor(clave);

    if (valor == 0) {

        hashMapPair nuevo_valor = make_pair(clave, 1);
        tabla[indice]->insertar(nuevo_valor);

    } else {

        ListaAtomica<hashMapPair> *bucket = tabla[indice];         
        for(hashMapPair &elemento : *bucket){
            if (elemento.first == clave) elemento.second++;
        }

    }

    this->_buckets_lock[indice].unlock();         

}

vector<string> HashMapConcurrente::claves() {

  /*   Devuelve todas las claves existentes en la tabla. 
        Esta operacion debe ser no bloqueante y libre de inanicion */

    // Completar (Ejercicio 2)

    vector<string> claves_a_retornar = {};

    for (int i = 0; i < HashMapConcurrente::cantLetras; i++){
        ListaAtomica<hashMapPair> *bucket = tabla[i]; 
        for(hashMapPair elemento : *bucket){
            claves_a_retornar.push_back(elemento.first);
        } 
    }

    return claves_a_retornar;
}

unsigned int HashMapConcurrente::valor(string clave) {

/* Devuelve el valor de clave en la tabla, o 0 si la clave no existe en la tabla. 
Esta operacion debe ser no bloqueante y libre de inanicion. */

    // Completar (Ejercicio 2)
    unsigned int indice = HashMapConcurrente::hashIndex(clave);
    unsigned int valor = 0;
    
    ListaAtomica<hashMapPair> *bucket = tabla[indice]; 
    for(hashMapPair elemento : *bucket){
        if (elemento.first == clave) valor = elemento.second;
    }

    return valor;
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
