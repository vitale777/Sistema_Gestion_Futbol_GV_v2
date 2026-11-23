# Documento Técnico (SDD): Sistema Integral de Gestión Deportiva y Táctica - Fútbol (GV)

**Autor:** Edmundo Gabriele Vitale Dugarte  
**Cátedra:** Programación 3 — Universidad de Los Andes (ULA)  
**Lenguaje y Estándar:** C++17 (compilación estricta con `-Wall -Wextra`, sin librerías externas)  

---

## 1. Resumen del Proyecto y Contexto
El presente proyecto consiste en el desarrollo de un software backend robusto en C++17 diseñado para la administración integral de un club de fútbol (**GV**). El sistema automatiza el control estadístico, administrativo y médico de los jugadores (desde categorías formativas como Sub-13 a Sub-16 hasta categorías superiores), empleando estructuras de datos dinámicas implementadas manualmente y una gestión estricta de memoria.

### Alcance Actual y Visión
* **Dentro del alcance actual:** Registro y directorio de atletas en $O(1)$, historial médico dinámico con validación de convocatoria en tiempo real, ranking deportivo ordenado dinámicamente, interfaz de consola robusta y suite de pruebas automatizadas.
* **Evolución planificada:** Persistencia en archivos planos (`data/jugadores.csv`) y posterior acoplamiento de una interfaz gráfica (GUI) con gestión visual de plantilla y pizarra táctica, reutilizando intacto el núcleo lógico actual.

---

## 2. Objetivos Claros
* **Objetivo General:** Desarrollar un sistema de gestión deportiva en C++17 que centralice la información de los atletas mediante estructuras de datos lineales y no lineales propias, asegurando tiempos de respuesta óptimos ($O(1)$ en búsquedas clave) y ausencia de fugas de memoria.
* **Objetivos Específicos:**
  * Diseñar el modelado de clases de la entidad `Jugador` mediante Programación Orientada a Objetos (POO).
  * Implementar un motor de búsqueda e inserción instantáneo basado en Tablas Hash para el directorio del club.
  * Desarrollar un "Semáforo Médico y Administrativo" autónomo que valide la elegibilidad del jugador en tiempo real.
  * Construir un sistema de ranking deportivo determinístico basado en Árboles Binarios de Búsqueda (BST).
  * Garantizar la calidad del código mediante pruebas unitarias automatizadas y compilación libre de advertencias.

---

## 3. Arquitectura General y Separación de Capas
El proyecto sigue una arquitectura modular diseñada para mantener la lógica de negocio y las estructuras de datos completamente desacopladas de la capa de presentación:

```text
Sistema_Gestion_Futbol/
├── docs/
│   └── DOCUMENTO_TECNICO.md   # Documentación de diseño (SDD)
├── include/
│   ├── models.h               # Entidades de dominio (Persona, Jugador, ListaMedica)
│   ├── directorio.h           # Motor Hash (SistemaClub) y operaciones CRUD
│   └── ranking_bst.h          # Árbol Binario de Búsqueda (RankingBST)
├── src/
│   └── main.cpp               # Capa de presentación (menú consola y validaciones)
└── tests/
    └── test_sistema.cpp       # Suite de pruebas unitarias automatizadas