#ifndef LISTA_ATOMICA_HPP
#define LISTA_ATOMICA_HPP

#include <atomic>
#include <cstddef>
#include <mutex>

using namespace std;

template<typename T>
class ListaAtomica {
 private:
    struct Nodo {
        Nodo(const T &val) : _valor(val), _siguiente(nullptr), _mtx() {}

        T _valor;
        Nodo *_siguiente;
        mutex _mtx;
    };

    mutex _mtx_insertar;
    atomic<Nodo *> _cabeza;
    

 public:
    ListaAtomica() : _cabeza(nullptr), _mtx_insertar() {}

    ~ListaAtomica() {
        Nodo *n, *t;
        n = _cabeza.load();
        while (n) {
            t = n;
            n = n->_siguiente;
            delete t;
        }
    }

    void insertar(const T &valor) {
        // Completar (Ejercicio 1)
        
        this->_mtx_insertar.lock();
        
        Nodo* nueva_cabeza =  new Nodo(valor);
        nueva_cabeza->_siguiente = _cabeza.load();
        _cabeza.store(nueva_cabeza);

        this->_mtx_insertar.unlock();
    }

    

    T& operator[](size_t i) const {
        Nodo *n = _cabeza.load();
        for (size_t j = 0; j < i; j++) {
            n = n->_siguiente;
        }
        return n->_valor;
    }

    unsigned int longitud() const {
        Nodo *n = _cabeza.load();
        unsigned int cant = 0;
        while (n != nullptr) {
            cant++;
            n = n->_siguiente;
        }
        return cant;
    }

    struct iterator {
    private:
        ListaAtomica *_lista;

        typename ListaAtomica::Nodo *_nodo_sig;

        iterator(ListaAtomica<T> *lista, typename ListaAtomica<T>::Nodo *sig)
            : _lista(lista), _nodo_sig(sig) {}

    public:
        iterator &operator=(const typename ListaAtomica::iterator &otro) {
            _lista = otro._lista;
            _nodo_sig = otro._nodo_sig;
            return *this;
        }

        T& operator*() {
            return _nodo_sig->_valor;
        }

        iterator& operator++() { 
            _nodo_sig = _nodo_sig->_siguiente;
            return *this;
        }

        iterator operator++(int) { 
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const typename ListaAtomica::iterator &otro) const {
            return _lista->_cabeza.load() == otro._lista->_cabeza.load()
                && _nodo_sig == otro._nodo_sig;
        }

        bool operator!=(const typename ListaAtomica::iterator &otro) const {
            return !(*this == otro);
        }

        friend iterator ListaAtomica<T>::begin();
        friend iterator ListaAtomica<T>::end();
    };

    iterator begin() { 
        return iterator(this, _cabeza);
    }

    iterator end() { 
        return iterator(this, nullptr);
    }
};

#endif /* LISTA_ATOMICA_HPP */
