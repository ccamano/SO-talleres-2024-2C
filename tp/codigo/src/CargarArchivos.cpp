#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include "CargarArchivos.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        // Completar (Ejercicio 4)
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

void cargarArchivosDesde(unsigned int &start, 
                         std::mutex &start_mutex,
                         std::vector<std::string> &filePaths,
                         HashMapConcurrente &hashMap){
    
    
    unsigned int cur_dir_num = 0;
    start_mutex.lock();
    while(start < filePaths.size()){

        cur_dir_num = start;
        start++;
        start_mutex.unlock();

        cargarArchivo(hashMap,filePaths[cur_dir_num]);

        start_mutex.lock(); 
    }
    
    start_mutex.unlock();
  
}

void cargarMultiplesArchivos( HashMapConcurrente &hashMap,unsigned int cantThreads,
    std::vector<std::string> filePaths) {
        
    vector<thread> thread_v;
    unsigned int start = 0;
    mutex start_mutex;
    unsigned int realThreads = 0;

    for(unsigned int i = 0; i < cantThreads; i++){
        start_mutex.lock();
        if(start < filePaths.size()){
            start_mutex.unlock();
            
            realThreads++;

            thread_v.emplace_back(cargarArchivosDesde,std::ref(start),    
                                                      std::ref(start_mutex), 
                                                      std::ref(filePaths), 
                                                      std::ref(hashMap));
            
            start_mutex.lock();
        }
        start_mutex.unlock();
    }

    for(unsigned int i=0; i < realThreads; i++){
        thread_v[i].join();
    }

}

#endif
