#ifndef HMC_HPP
#define HMC_HPP

#include <mutex>
#include <array>
#include <atomic>
#include <string>
#include <vector>

#include "ListaAtomica.hpp"
#include "Lightswitch.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
 public:
    static constexpr int cantLetras = 26;

    HashMapConcurrente();
    
    void incrementar(std::string clave);
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);
   
    mutex barrera[cantLetras]; // turnstile
    mutex _buckets_lock[cantLetras]; // roomEmpty
    Lightswitch lightswitch;

    float promedio();
    float promedioParalelo(unsigned int cantThreads);

 private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];

    static unsigned int hashIndex(std::string clave);
    pair<unsigned int, unsigned int> sumaYCantidadPorFila(unsigned int fila);
    void promedioParaleloDesde(unsigned int &start, mutex &start_mutex, pair<unsigned int, unsigned int> &resultado, mutex &resultado_mutex);
};

#endif  /* HMC_HPP */
