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

void cargarArchivosDesde(unsigned int start, std::mutex &visited_mutex,std::vector<bool> &visited,
std::vector<std::string> &filePaths,HashMapConcurrente &hashMap){
    while(start < visited.size()){
        visited_mutex.lock();
            if(!visited[start]){
                visited[start] = true;
                visited_mutex.unlock();
                cargarArchivo(hashMap,filePaths[start]);
            }
            else{
                visited_mutex.unlock();
            }
        start++;
    }
}

void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    std::mutex visited_mutex;
    std::vector<bool> visited = std::vector<bool>(filePaths.size(),false);
    std::vector<std::thread> thread_v;
    for(unsigned int i = 0; i < cantThreads; i++){
        thread_v.emplace_back(cargarArchivosDesde,i,std::ref(visited_mutex),std::ref(visited),std::ref(filePaths),std::ref(hashMap));
    }

    for(unsigned int i=0; i < cantThreads; i++){
        thread_v[i].join();
    }

    // Completar (Ejercicio 4)
}

#endif
