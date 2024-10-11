#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include "HashMapConcurrente.hpp"
#include "Lightswitch.cpp"
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

    this->barrera[indice].lock();
    
    this->_buckets_lock[indice].lock();

    // Sección crítica
    unsigned int valor = 0;
    
    ListaAtomica<hashMapPair> *bucket = tabla[indice]; 
    for(hashMapPair elemento : *bucket){
        if (elemento.first == clave) valor = elemento.second;
    }

    if (valor == 0) {

        hashMapPair nuevo_valor = make_pair(clave, 1);
        tabla[indice]->insertar(nuevo_valor);

    } else {

        ListaAtomica<hashMapPair> *bucket = tabla[indice];         
        for(hashMapPair &elemento : *bucket){
            if (elemento.first == clave) elemento.second++;
        }

    }

    this->barrera[indice].unlock();
    this->_buckets_lock[indice].unlock();         

}

vector<string> HashMapConcurrente::claves() {

  /*   Devuelve todas las claves existentes en la tabla. 
        Esta operacion debe ser no bloqueante y libre de inanicion */

    // Completar (Ejercicio 2)
    vector<string> claves_a_retornar = {};

    for (int i = 0; i < HashMapConcurrente::cantLetras; i++){
        this->barrera[i].lock();
        this->barrera[i].unlock();

        this->lightswitch.lock(this->_buckets_lock[i]);
        ListaAtomica<hashMapPair> *bucket = tabla[i]; 
        for(hashMapPair elemento : *bucket){
            claves_a_retornar.push_back(elemento.first);
        } 
        this->lightswitch.unlock(this->_buckets_lock[i]);
    }
    
    return claves_a_retornar;
}

unsigned int HashMapConcurrente::valor(string clave) {

/* Devuelve el valor de clave en la tabla, o 0 si la clave no existe en la tabla. 
Esta operacion debe ser no bloqueante y libre de inanicion. */

    // Completar (Ejercicio 2)
    unsigned int indice = HashMapConcurrente::hashIndex(clave);
    unsigned int valor = 0;
    
    this->barrera[indice].lock();
    this->barrera[indice].unlock();

    this->lightswitch.lock(this->_buckets_lock[indice]);

    ListaAtomica<hashMapPair> *bucket = tabla[indice]; 
    for(hashMapPair elemento : *bucket){
        if (elemento.first == clave) valor = elemento.second;
    }
    this->lightswitch.unlock(this->_buckets_lock[indice]);
    return valor;
}

float HashMapConcurrente::promedio() {
    /* 
    a) La implementacion provista de promedio puede ejecutarse concurrentemente con incrementar, 
    lo cual podria traer problemas. Modifiquen la implementacion para que estos 
    problemas ya no puedan suceder.
    b) Implementen el metodo pair<string, unsigned int>
    promedioParalelo(unsigned int cantThreads), que realiza lo mismo
    que promedio pero repartiendo el trabajo entre la cantidad de threads in-
    dicada. Cada thread procesara una fila de la tabla a la vez. Se
    debe maximizar la concurrencia: en ningun momento debe haber threads inactivos,
    y los mismos deben terminar su ejecucion si y solo si ya
    no quedan filas por procesar. No olvidar que debe poder ejecutarse concurrentemente
    con incrementar */

    for (int i = 0; i < HashMapConcurrente::cantLetras; i++)
        this->_buckets_lock[i].lock();

    float sum = 0.0;
    unsigned int count = 0;
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (const auto& p : *tabla[index]) {
            sum += p.second;
            count++;
        }
        this->_buckets_lock[index].unlock();

    }
    if (count > 0) {
        return sum / count;
    }

    return 0;        
}

pair<unsigned int, unsigned int> HashMapConcurrente::sumaYCantidadPorFila(unsigned int fila) {
    unsigned int sum = 0;
    unsigned int count = 0;
    for (const auto& p : *tabla[fila]) {
        sum += p.second;
        count++;
    }
    return make_pair(sum, count);
}

void HashMapConcurrente::promedioParaleloDesde(unsigned int &start, mutex &start_mutex,
                                               pair<unsigned int, unsigned int> &resultado, mutex &resultado_mutex){
    pair<unsigned int, unsigned int> res = make_pair<unsigned int, unsigned int>(0,0);
    unsigned int fila = 0;
    start_mutex.lock();
    while(start < HashMapConcurrente::cantLetras){
        fila = start;
        start++;
        start_mutex.unlock();

        pair<unsigned int, unsigned int> cur_row = sumaYCantidadPorFila(fila);
        this->_buckets_lock[fila].unlock();
        // first: suma, second: cantidad de elems de la fila
        res.first += cur_row.first;
        res.second += cur_row.second;
        
        start_mutex.lock(); 
    }
    start_mutex.unlock();
    resultado_mutex.lock();
    resultado.first += res.first;
    resultado.second += res.second;
    resultado_mutex.unlock();
}


float HashMapConcurrente::promedioParalelo(unsigned int cantThreads) {
    
    pair<unsigned int, unsigned int> resultado;
    mutex resultado_mutex;

    for (int i = 0; i < HashMapConcurrente::cantLetras; i++)
        this->_buckets_lock[i].lock();

    vector<thread> thread_v;
    unsigned int start = 0;
    mutex start_mutex;
    unsigned int realThreads = 0;
    for(unsigned int i = 0; i < cantThreads; i++){
        start_mutex.lock();
        if(start < HashMapConcurrente::cantLetras){
            start_mutex.unlock();
            
            realThreads++;
            // TODO: ver como arreglar
           // thread_v.emplace_back(promedioParaleloDesde,ref(start), ref(start_mutex), ref(resultado), ref(resultado_mutex));
            
            start_mutex.lock();
        }
        start_mutex.unlock();
    }

    for(unsigned int i=0; i < realThreads; i++){
        thread_v[i].join();
    }

    // promedio
    if (resultado.second > 0) {
        return resultado.first / resultado.second;
    }

    return 0.0;
}



#endif
