#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include "HashMapConcurrente.hpp"
#include <mutex>


HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
        sem_init(&writer_free[i],0,1);
        sem_init(&reader_free[i],0,1);
        readers[i] = 0;
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
        table_mutex.lock();
        sem_wait(&writer_free[hash]);//Espera a que termine el escritor anterior, si existe, y bloquea el turnstile para los readers siguientes
        sem_wait(&reader_free[hash]);//Espera a que no hayan lectores en la seccion crítica
        writer_mutex[hash].lock();
            auto it = tabla[hash]->begin();
            auto end = tabla[hash]->end();
            while(it != end && std::get<0>(*it) != clave){
                ++it;
            }
            if(it!=end){
                *it = hashMapPair(std::get<0>(*it),std::get<1>(*it)+1);
            }
            else{
                tabla[hash]->insertar(new_key);
            }
        writer_mutex[hash].unlock();
        sem_post(&reader_free[hash]); //Habilita al siguiente batch de threads leyendo datos
        sem_post(&writer_free[hash]); //Recien a partir de esta instruccion pueden entrar lectores y escritores. Lo toma quien llega primero, ya que el siguiente escritor estaria esperando aca, y todos los lectores que estaban adentro terminaron su codigo antes de que este escritor entre
        table_mutex.unlock();
    }
}

std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2)

    std::vector<std::string> res = {};
    for(int i = 0; i < cantLetras;i++){
        sem_wait(&writer_free[i]);
            incrementarLectoresAtomic(i);
        sem_post(&writer_free[i]);
        auto it = tabla[i]->begin();
        auto end = tabla[i]->end();
        while(it != end){
            std::string clave = std::get<0>(*it);
            res.push_back(clave);
            it++;
        }
            decrementarLectoresAtomic(i);
    }
    return res;
}
//Pre: no pueden haber writers en la seccion crítica mientras se ejecuta el código: son bloqueados por writer_free o reader_free
void HashMapConcurrente::incrementarLectoresAtomic(unsigned int bucket){ 
    reader_mutex[bucket].lock();
        if(readers[bucket] == 0){
            sem_wait(&reader_free[bucket]); //Bloquea acceso a writers hasta que no haya readers
        }
        readers[bucket]++;
    reader_mutex[bucket].unlock();
}

//Pre: no pueden haber writers en la seccion crítica mientras se ejecuta el código: son bloqueados por writer_free o reader_free
void HashMapConcurrente::decrementarLectoresAtomic(unsigned int bucket){
    reader_mutex[bucket].lock();
        readers[bucket]--;
        if(readers[bucket] == 0){
            sem_post(&reader_free[bucket]); //Una vez que no haya readers, desbloqueo entrada para los writers
        }
    reader_mutex[bucket].unlock();
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // Completar (Ejercicio 2)
    unsigned int res = 0;
    if(!(clave[0] > 'A' && clave[0] < 'Z') && !(clave[0] > 'a' && clave[0] < 'z')){
        std::cout << "Producto inválido: "  << clave << std::endl;
    }
    else{
        unsigned int hash = hashIndex(clave);
        hashMapPair new_key = hashMapPair(clave,1);
       
        sem_wait(&writer_free[hash]);
            incrementarLectoresAtomic(hash);
        sem_post(&writer_free[hash]);
        auto it = tabla[hash]->begin();
        auto end = tabla[hash]->end();
        while(it != end && std::get<0>(*it) != clave){
             ++it;
        }
        if(it!=end){
            res = std::get<1>(*it);
        }
        decrementarLectoresAtomic(hash);
    }
    return res;
}



float HashMapConcurrente::promedio() {

    float sum = 0.0;
    unsigned int count = 0;
    table_mutex.lock();
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        //Correccion del ej 3a: bloqueo toda la tabla para escrituras
       
        for (const auto& p : *tabla[index]) {
            sum += p.second;
            count++;
        }
    }
    table_mutex.unlock();
    if (count > 0) {
        return sum / count;
    }
    return 0;        
}

/*std::pair<std::string, unsigned int> HashMapConcurrente::promedioParalelo(unsigned int cantThreads) {

    float sum = 0.0;
    unsigned int count = 0;
    table_mutex.lock();
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        //Corrección del 3a: bloqueo toda la tabla para escrituras
        
        for (const auto& p : *tabla[index]) {
            sum += p.second;
            count++;
        }
        
    }
    table_mutex.unlock();
    if (count > 0) {
        return 
    }
    return        
}*/



#endif
