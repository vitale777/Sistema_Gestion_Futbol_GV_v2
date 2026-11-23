#ifndef RANKING_BST_H
#define RANKING_BST_H

#include "models.h"
#include <iostream>
#include <string>

// ============================================================
// ÁRBOL BINARIO DE BÚSQUEDA (BST) PARA RANKING DEPORTIVO
// ============================================================
// Criterio de orden (mayor a menor al recorrer In-Order inverso):
//   Primario:   puntaje = goles*2 + asistencias  (mayor = mejor)
//   Desempate 1: goles                            (mayor = mejor)
//   Desempate 2: cedula_id                        (lexicográfico ASC,
//                                                  único → no hay pérdida)
//
// Al desempatar por cédula se garantiza un árbol BST estricto
// (sin claves iguales), lo que evita pérdida de nodos con igual puntaje.
// ============================================================

struct NodoBST {
    // Datos propios del nodo (copia ligera de los campos clave)
    std::string cedula;
    std::string nombre;
    std::string posicion;
    std::string categoria;
    int         dorsal;
    int         goles;
    int         asistencias;
    int         puntaje;     // goles*2 + asistencias — precalculado al insertar

    NodoBST* izq;
    NodoBST* der;

    NodoBST(const Jugador& j)
        : cedula(j.getCedula()),
          nombre(j.getNombre()),
          posicion(j.getPosicion()),
          categoria(j.getCategoria()),
          dorsal(j.getDorsal()),
          goles(j.getGoles()),
          asistencias(j.getAsistencias()),
          puntaje(j.getPuntajeRanking()),
          izq(nullptr), der(nullptr) {}
};

class RankingBST {
private:
    NodoBST* raiz;

    // ---- Comparación determinista entre dos nodos ----
    // Retorna  < 0 si a "va antes" (es peor) que b en el ranking
    // Retorna  > 0 si a "va después" (es mejor) que b
    // Retorna  = 0 si son idénticos (no debería ocurrir con cédulas únicas)
    static int comparar(const NodoBST* a, const NodoBST* b) {
        if (a->puntaje != b->puntaje)
            return a->puntaje - b->puntaje;          // mayor puntaje → Der
        if (a->goles != b->goles)
            return a->goles - b->goles;              // mayor goles → Der
        // Desempate final: cédula ASC → menor cédula va a la izquierda
        if (a->cedula < b->cedula) return -1;
        if (a->cedula > b->cedula) return  1;
        return 0;
    }

    // ---- Inserción recursiva ----
    NodoBST* insertarRec(NodoBST* nodo, NodoBST* nuevo) {
        if (nodo == nullptr) return nuevo;

        int cmp = comparar(nuevo, nodo);
        if (cmp < 0)
            nodo->izq = insertarRec(nodo->izq, nuevo);
        else if (cmp > 0)
            nodo->der = insertarRec(nodo->der, nuevo);
        // cmp == 0 → cédula duplicada (no debería ocurrir desde SistemaClub)
        // Si ocurre, se descarta silenciosamente el duplicado (delete nuevo)
        else
            delete nuevo;

        return nodo;
    }

    // ---- Recorrido In-Order INVERSO (der → raiz → izq) ----
    // Visita primero los nodos mayores → imprime de mayor a menor puntaje
    void recorridoDescRec(NodoBST* nodo, std::ostream& os, int& posicion) const {
        if (nodo == nullptr) return;

        recorridoDescRec(nodo->der, os, posicion);

        os << "\n  #" << posicion++ << "  "
           << nodo->nombre
           << " [" << nodo->cedula << "]"
           << " | Pts: "   << nodo->puntaje
           << " (G:" << nodo->goles << " A:" << nodo->asistencias << ")"
           << " | " << nodo->posicion
           << " | " << nodo->categoria
           << " | Dorsal #" << nodo->dorsal
           << "\n";

        recorridoDescRec(nodo->izq, os, posicion);
    }

    // ---- Liberación recursiva de toda la memoria ----
    void vaciarRec(NodoBST* nodo) {
        if (nodo == nullptr) return;
        vaciarRec(nodo->izq);
        vaciarRec(nodo->der);
        delete nodo;
    }

public:
    RankingBST() : raiz(nullptr) {}

    // Destructor: libera todos los nodos dinámicos sin fugas
    ~RankingBST() { vaciarRec(raiz); raiz = nullptr; }

    // Vaciar manualmente (útil para reconstruir el ranking en cada llamada)
    void vaciar() { vaciarRec(raiz); raiz = nullptr; }

    // Insertar un jugador en el BST por puntaje; O(log n) promedio
    void insertar(const Jugador& j) {
        NodoBST* nuevo = new NodoBST(j);
        raiz = insertarRec(raiz, nuevo);
    }

    // Recorrido In-Order inverso → imprime ranking de mayor a menor
    void imprimirRankingDescendente(std::ostream& os = std::cout) const {
        if (raiz == nullptr) {
            os << "\n[!] El ranking esta vacio. Registre jugadores primero.\n";
            return;
        }
        os << "\n=========================================\n";
        os << "       RANKING DEPORTIVO DEL CLUB        \n";
        os << "  (ordenado de mayor a menor puntaje)    \n";
        os << "  Criterio: Goles x2 + Asistencias       \n";
        os << "=========================================\n";
        int posicion = 1;
        recorridoDescRec(raiz, os, posicion);
        os << "=========================================\n";
    }

    // Consulta si el árbol está vacío
    bool estaVacio() const { return raiz == nullptr; }

    // Altura del árbol (útil para pruebas de estructura)
    int altura(NodoBST* nodo) const {
        if (nodo == nullptr) return 0;
        int h_izq = altura(nodo->izq);
        int h_der = altura(nodo->der);
        return 1 + (h_izq > h_der ? h_izq : h_der);
    }
    int altura() const { return altura(raiz); }
};

#endif // RANKING_BST_H
