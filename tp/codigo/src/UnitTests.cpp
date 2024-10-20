#include <vector>
#include <thread>
#include "lib/littletest.hpp"

#include "../src/ListaAtomica.hpp"
#include "../src/HashMapConcurrente.hpp"
#include "../src/CargarArchivos.hpp"

using namespace std;

// Tests Ejercicio 1

LT_BEGIN_SUITE(TestsEjercicio1)

ListaAtomica<int> l;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio1)

LT_BEGIN_TEST(TestsEjercicio1, ListaComienzaVacia)
    LT_CHECK_EQ(l.longitud(), 0);
LT_END_TEST(ListaComienzaVacia)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaElemento)
    l.insertar(42);
    LT_CHECK_EQ(l.longitud(), 1);
LT_END_TEST(InsertarAgregaElemento)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaElementoCorrecto)
    l.insertar(42);
    LT_CHECK_EQ(l[0], 42);
LT_END_TEST(InsertarAgregaElementoCorrecto)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaEnOrden)
    l.insertar(4);
    l.insertar(3);
    l.insertar(2);
    l.insertar(1);
    LT_CHECK_EQ(l.longitud(), 4);
    LT_CHECK_EQ(l[0], 1);
    LT_CHECK_EQ(l[1], 2);
    LT_CHECK_EQ(l[2], 3);
    LT_CHECK_EQ(l[3], 4);
LT_END_TEST(InsertarAgregaEnOrden)

// Tests Ejercicio 2

LT_BEGIN_SUITE(TestsEjercicio2)

HashMapConcurrente hM;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio2)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoEnHashMapVacio)
    LT_CHECK_EQ(hM.valor("Heladera"), 0);
LT_END_TEST(ValorEsCorrectoEnHashMapVacio)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoEnHashMapVacio)
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> expected = {};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoEnHashMapVacio)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasUnaInsercion)
    hM.incrementar("Heladera");
    LT_CHECK_EQ(hM.valor("Heladera"), 1);
LT_END_TEST(ValorEsCorrectoTrasUnaInsercion)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasUnaInsercion)
    hM.incrementar("Heladera");
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> expected = {"Heladera"};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoTrasUnaInsercion)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasDosInsercionesMismaPalabra)
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    LT_CHECK_EQ(hM.valor("Heladera"), 2);
LT_END_TEST(ValorEsCorrectoTrasDosInsercionesMismaPalabra)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasDosInsercionesMismaPalabra)
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> expected = {"Heladera"};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoTrasDosInsercionesMismaPalabra)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasVariasInsercionesMismoBucket)
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Lavarropa");
    LT_CHECK_EQ(hM.valor("Heladera"), 2);
    LT_CHECK_EQ(hM.valor("Lavarropa"), 1);
LT_END_TEST(ValorEsCorrectoTrasVariasInsercionesMismoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasVariasInsercionesMismoBucket)
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Lavarropa");
    std::vector<std::string> actual = hM.claves();
    LT_CHECK_EQ(actual.size(), 2);
    LT_CHECK(std::find(actual.begin(), actual.end(), "Heladera") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "Lavarropa") != actual.end());
LT_END_TEST(ClavesEsCorrectoTrasVariasInsercionesMismoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasVariasInsercionesDistintoBucket)
    hM.incrementar("Heladera");
    hM.incrementar("Microondas");
    hM.incrementar("Heladera");
    LT_CHECK_EQ(hM.valor("Heladera"), 2);
    LT_CHECK_EQ(hM.valor("Microondas"), 1);
LT_END_TEST(ValorEsCorrectoTrasVariasInsercionesDistintoBucket)


LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasVariasInsercionesDistintoBucket)
    hM.incrementar("Heladera");
    hM.incrementar("Microondas");
    hM.incrementar("Heladera");
    std::vector<std::string> actual = hM.claves();
    LT_CHECK_EQ(actual.size(), 2);
    LT_CHECK(std::find(actual.begin(), actual.end(), "Heladera") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "Microondas") != actual.end());
LT_END_TEST(ClavesEsCorrectoTrasVariasInsercionesDistintoBucket)

// Tests Ejercicio 3

LT_BEGIN_SUITE(TestsEjercicio3)

HashMapConcurrente hM;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio3)

LT_BEGIN_TEST(TestsEjercicio3, PromedioEsCorrecto)
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Microondas");
    hM.incrementar("Microondas");

    float actual = hM.promedio();
    LT_CHECK_EQ(actual, 3);
LT_END_TEST(PromedioEsCorrecto)

// Completar TEST para caso concurrente

LT_BEGIN_SUITE(TestsEjercicio4)

HashMapConcurrente hM;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio4)

LT_BEGIN_TEST(TestsEjercicio4, CargarArchivoFunciona)
    cargarArchivo(hM, "data/test-1");
    LT_CHECK_EQ(hM.valor("Heladera"), 1);
    LT_CHECK_EQ(hM.claves().size(), 5);
LT_END_TEST(CargarArchivoFunciona)

LT_BEGIN_TEST(TestsEjercicio4, CargarMultiplesArchivosFuncionaUnThread)
    cargarMultiplesArchivos(hM, 1, {"data/test-1", "data/test-2", "data/test-3"});
    LT_CHECK_EQ(hM.valor("Heladera"), 2);
    LT_CHECK_EQ(hM.valor("NotebookOMEN"), 3);
    LT_CHECK_EQ(hM.valor("Microondas"), 4);
    LT_CHECK_EQ(hM.claves().size(), 12);
LT_END_TEST(CargarMultiplesArchivosFuncionaUnThread)

LT_BEGIN_TEST(TestsEjercicio4, CargarMultiplesArchivosFuncionaDosThreads)
    cargarMultiplesArchivos(hM, 2, {"data/test-1", "data/test-2", "data/test-3"});
    LT_CHECK_EQ(hM.valor("Heladera"), 2);
    LT_CHECK_EQ(hM.valor("NotebookOMEN"), 3);
    LT_CHECK_EQ(hM.valor("Microondas"), 4);
    LT_CHECK_EQ(hM.claves().size(), 12);
LT_END_TEST(CargarMultiplesArchivosFuncionaDosThreads)

LT_BEGIN_TEST(TestsEjercicio4, CargarMultiplesArchivosFuncionaVariosThreads)
    int UPPER_LIMIT = 100;
    int LOWER_LIMIT = 1;
    int v1 = 0;
    int v2 = 0;
    int v3 = 0;
    std::vector<std::string> filePaths = {};
    for(int i = 0; i < LOWER_LIMIT; i++){
        filePaths.push_back("data/test-1");
        filePaths.push_back("data/test-2");
        filePaths.push_back("data/test-3");
    }
    for(int i = LOWER_LIMIT; i <= UPPER_LIMIT; i++){
        v1 += 1*(i+1);
        v2 += 3*(i+1);
        v3 += 2*(i+1);
        filePaths.push_back("data/test-1");
        filePaths.push_back("data/test-2");
        filePaths.push_back("data/test-3");
        cargarMultiplesArchivos(hM, i,filePaths);
        LT_CHECK_EQ(hM.valor("BicicletaFixie"), v1);
        LT_CHECK_EQ(hM.valor("Iphone15"), v2);
        LT_CHECK_EQ(hM.valor("TV"), v3);
        LT_CHECK_EQ(hM.claves().size(), 12);
    }
LT_END_TEST(CargarMultiplesArchivosFuncionaVariosThreads)



LT_BEGIN_SUITE(TestsConcurrencia)

HashMapConcurrente hM;
ListaAtomica<int> l;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsConcurrencia)

LT_BEGIN_TEST(TestsConcurrencia, IncrementarConcurrenteClave)

   HashMapConcurrente hM;
    std::vector<std::thread> threads;
    uint numThreads = 100;

    for (uint i = 0; i < numThreads; i++) {
        threads.emplace_back([] (HashMapConcurrente &hM) {
            hM.incrementar("test");
        }, std::ref(hM));
    }
    for (uint i = 0; i < numThreads; i++) {
        threads[i].join();
    }
    LT_CHECK_EQ(hM.valor("test"), numThreads);
    LT_CHECK_EQ(hM.claves().size(), 1);

LT_END_TEST(IncrementarConcurrenteClave) 


LT_BEGIN_TEST(TestsConcurrencia, Ejercicio3IncrementarYPromedioConcurrentemente)

   HashMapConcurrente hM;
    std::vector<std::thread> threads;
    uint numThreadsA = 20;
    uint numThreadsB = 20;
    uint numThreadsC = 20;

    float promedioInicial = hM.promedio();

    for (uint i = 0; i < numThreadsA; i++) {
        threads.emplace_back([] (HashMapConcurrente &hM) {
            hM.incrementar("a");
        }, std::ref(hM));
    }

    for (uint i = 0; i < numThreadsB; i++) {
        threads.emplace_back([] (HashMapConcurrente &hM) {
            hM.incrementar("b");
        }, std::ref(hM));
    }

    for (uint i = 0; i < numThreadsC; i++) {
        threads.emplace_back([] (HashMapConcurrente &hM) {
            hM.incrementar("c");
        }, std::ref(hM));
    }


    for (uint i = 0; i < numThreadsA + numThreadsB + numThreadsC; i++) {
        threads[i].join();
    }

    float promedioFinal = hM.promedio();

    LT_CHECK_EQ(promedioInicial, 0);
    LT_CHECK_EQ(promedioFinal, 20);
    LT_CHECK_EQ(hM.claves().size(), 3);

LT_END_TEST(Ejercicio3IncrementarYPromedioConcurrentemente) 



LT_BEGIN_TEST(TestsConcurrencia, Ejercicio3IncrementarYPromedioParalelo)

    HashMapConcurrente hM;
    std::vector<std::thread> threads;
    uint numThreadsA = 20;
    uint numThreadsB = 20;
    uint numThreadsC = 20;

    uint threadsPromedio = 50;

    float promedioInicial = hM.promedioParalelo(threadsPromedio);

    for (uint i = 0; i < numThreadsA; i++) {
        threads.emplace_back([] (HashMapConcurrente &hM) {
            hM.incrementar("a");
        }, std::ref(hM));
    }

    for (uint i = 0; i < numThreadsB; i++) {
        threads.emplace_back([] (HashMapConcurrente &hM) {
            hM.incrementar("b");
        }, std::ref(hM));
    }

    for (uint i = 0; i < numThreadsC; i++) {
        threads.emplace_back([] (HashMapConcurrente &hM) {
            hM.incrementar("c");
        }, std::ref(hM));
    }

    for (uint i = 0; i < numThreadsA + numThreadsB + numThreadsC; i++) {
        threads[i].join();
    }

    float promedioFinal = hM.promedioParalelo(threadsPromedio);

    LT_CHECK_EQ(promedioInicial, 0);
    LT_CHECK_EQ(promedioFinal, 20);
    LT_CHECK_EQ(hM.claves().size(), 3);

LT_END_TEST(Ejercicio3IncrementarYPromedioParalelo) 

// Ejecutar tests
LT_BEGIN_AUTO_TEST_ENV()
    AUTORUN_TESTS()
LT_END_AUTO_TEST_ENV()
