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
    std::pair<std::string, unsigned int> promedioParalelo(unsigned int cantThreads);
    float promedio();


 private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
    static unsigned int hashIndex(std::string clave);
    std::mutex writer_mutex[HashMapConcurrente::cantLetras]; //Un mutex para escribir en cada bucket
    sem_t writer_free[HashMapConcurrente::cantLetras]; //Permite a los readers entrar cuando no haya un escritor modificando
    sem_t reader_free[HashMapConcurrente::cantLetras]; //Permite a los writers entrar cuando ningun reader esté leyendo datos
    unsigned int readers[HashMapConcurrente::cantLetras]; //Scoreboard
    std::mutex reader_mutex[HashMapConcurrente::cantLetras];
    void incrementarLectoresAtomic(unsigned int bucket);
    void decrementarLectoresAtomic(unsigned int bucket);
    std::mutex table_mutex; //Mutex para toda la tabla, usada para que haya contencion entre calculo de promedio e insercion de elementos

};

#endif  /* HMC_HPP */
