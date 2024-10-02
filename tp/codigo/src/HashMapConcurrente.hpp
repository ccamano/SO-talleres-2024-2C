#ifndef HMC_HPP
#define HMC_HPP

#include <mutex>
#include <array>
#include <atomic>
#include <string>
#include <vector>
#include <semaphore.h>
#include "ListaAtomica.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
 public:
    static constexpr int cantLetras = 26;

    HashMapConcurrente();

    void incrementar(std::string clave);
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);

    float promedio();


 private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];

    static unsigned int hashIndex(std::string clave);
    std::mutex writer_mutex[HashMapConcurrente::cantLetras]; //Un mutex para escribir en cada bucket
    int writer_free[HashMapConcurrente::cantLetras]; //Un mutex para leer en cada bucket solo cuando no haya nadie escribiendo
    
};

#endif  /* HMC_HPP */
