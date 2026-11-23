#ifndef DIRECTORIO_H
#define DIRECTORIO_H

#include "models.h"
#include "ranking_bst.h"
#include <iostream>

// Nodo para el encadenamiento separado en la Tabla Hash
struct NodoHash {
    Jugador dato;
    NodoHash* siguiente;
    NodoHash(const Jugador& j) : dato(j), siguiente(nullptr) {}
};

class SistemaClub {
private:
    NodoHash** tabla; 
    size_t capacidad; // Tamaño primo de cubetas
    size_t totalJugadores; // Contador de jugadores registrados en el club

    // Función Hash Polinomial (DJB2) para strings (cédulas)
    size_t funcionHash(const std::string& clave) const {
        size_t hash = 5381;
        for (char c : clave) {
            hash = ((hash << 5) + hash) + static_cast<unsigned char>(c); 
        }
        return hash % capacidad;
    }

public:
    // Constructor
    SistemaClub(size_t cap = 101) : capacidad(cap), totalJugadores(0) {
        tabla = new NodoHash*[capacidad];
        for (size_t i = 0; i < capacidad; ++i) {
            tabla[i] = nullptr;
        }
    }

    // DESTRUCTOR: Previene Memory Leaks liberando TODA la memoria dinámica
    ~SistemaClub() {
        for (size_t i = 0; i < capacidad; ++i) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                NodoHash* temp = actual;
                actual = actual->siguiente;
                delete temp; 
            }
        }
        delete[] tabla; 
    }

    // Contador de jugadores registrados
    size_t getTotalJugadores() const {
        return totalJugadores;
    }

    // CRUD: Inserción O(1) con bloqueo de cédula duplicada
    bool registrarJugador(const Jugador& j) {
        // Bloqueo de duplicados en tiempo O(1) promedio sin reservar memoria
        if (buscarJugador(j.getCedula()) != nullptr) {
            return false;
        }

        size_t indice = funcionHash(j.getCedula());
        NodoHash* nuevo = new NodoHash(j);
        nuevo->siguiente = tabla[indice];
        tabla[indice] = nuevo;
        totalJugadores++;
        return true;
    }

    // CRUD: Búsqueda O(1). Devuelve PUNTERO para poder modificarlo (Update)
    Jugador* buscarJugador(const std::string& cedula) {
        size_t indice = funcionHash(cedula);
        NodoHash* actual = tabla[indice];
        
        while (actual != nullptr) {
            if (actual->dato.getCedula() == cedula) {
                return &(actual->dato); // Devuelve la dirección de memoria exacta
            }
            actual = actual->siguiente;
        }
        return nullptr; // No encontrado
    }

    // Búsqueda en versión const
    const Jugador* buscarJugador(const std::string& cedula) const {
        size_t indice = funcionHash(cedula);
        NodoHash* actual = tabla[indice];
        
        while (actual != nullptr) {
            if (actual->dato.getCedula() == cedula) {
                return &(actual->dato);
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    // Recorre la Tabla Hash insertando cada Jugador en un RankingBST externo
    void llenarRanking(RankingBST& ranking) const {
        for (size_t i = 0; i < capacidad; ++i) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                ranking.insertar(actual->dato);
                actual = actual->siguiente;
            }
        }
    }

    // Listar todos los jugadores recorriendo las 101 cubetas de la Tabla Hash
    void listarJugadores() const {
        if (totalJugadores == 0) {
            std::cout << "\n[!] No hay jugadores registrados en el club.\n";
            return;
        }

        std::cout << "\n=========================================\n";
        std::cout << "  LISTA COMPLETA DE JUGADORES DEL CLUB   \n";
        std::cout << "  Total de registrados: " << totalJugadores << "\n";
        std::cout << "=========================================\n";

        size_t contador = 1;
        for (size_t i = 0; i < capacidad; ++i) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                std::cout << "\n>>> Jugador #" << contador++ << " <<<";
                actual->dato.imprimirFicha();
                actual = actual->siguiente;
            }
        }
    }
    // Exporta todos los jugadores de la Tabla Hash en formato CSV
    void exportarCSV(std::ostream& out) const {
        for (size_t i = 0; i < capacidad; ++i) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                const Jugador& j = actual->dato;
                out << j.getCedula() << ","
                    << j.getNombre() << ","
                    << j.getFechaNacimiento() << ","
                    << j.getPosicion() << ","
                    << j.getDorsal() << ","
                    << j.getCategoria() << ","
                    << j.getGoles() << ","
                    << j.getAsistencias() << ","
                    << j.getMinutos() << ","
                    << j.getAsisEntrenamientos() << ","
                    << j.getTotalEntrenos() << ","
                    << (j.isSolvente() ? 1 : 0) << ","
                    << (j.isElegibleComet() ? 1 : 0) << "\n";
                actual = actual->siguiente;
            }
        }
    }
};

#endif // DIRECTORIO_H