#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

// ============================================================
// MÓDULO DE PERSISTENCIA CSV  —  Sistema GV  (C++17)
// ============================================================
// Formato del archivo:  data/jugadores.csv
//
// Cabecera (línea 1, siempre presente al escribir):
//   cedula,nombre,fecha_nac,posicion,dorsal,categoria,
//   goles,asistencias,minutos,asis_entrenos,total_entrenos,
//   solvente,comet
//
// Una fila por jugador. El historial médico NO se persiste en
// esta versión (es volátil entre sesiones); sólo se guarda el
// estado booleano resultante (solvente_pago, elegible_comet).
//
// Contrato de robustez:
//   - Líneas vacías y líneas con < 13 campos son ignoradas
//     silenciosamente (sin crashear el programa).
//   - Cédulas ya presentes en el SistemaClub al momento de
//     cargar son descartadas (evita duplicados).
//   - El directorio "data/" debe existir antes de guardar;
//     si no existe, guardarCSV() devuelve false.
// ============================================================

#include "directorio.h"   // SistemaClub + Jugador
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace PersistenciaCSV {

    // ---- Nombre del archivo de datos ----
    static const char* RUTA_CSV = "data/jugadores.csv";

    // ---- Cabecera del CSV ----
    static const char* CABECERA =
        "cedula,nombre,fecha_nac,posicion,dorsal,categoria,"
        "goles,asistencias,minutos,asis_entrenos,total_entrenos,"
        "solvente,comet";

    // ----------------------------------------------------------------
    // guardarCSV(club, ruta)
    //   Serializa todos los jugadores de SistemaClub en el CSV.
    //   Retorna true si el archivo fue escrito correctamente.
    // ----------------------------------------------------------------
    inline bool guardarCSV(const SistemaClub& club,
                           const std::string& ruta = RUTA_CSV)
    {
        std::ofstream out(ruta);
        if (!out.is_open()) return false;

        out << CABECERA << "\n";

        // Recorre las cubetas de la tabla hash y vuelca cada jugador
        // SistemaClub expone iteración mediante llenarRanking; aquí
        // necesitamos acceso raw → usamos el método exportarCSV()
        // que añadimos a SistemaClub.
        club.exportarCSV(out);
        out.flush();
        return out.good();
    }

    // ----------------------------------------------------------------
    // cargarCSV(club, ruta)
    //   Lee el CSV y registra en club los jugadores nuevos.
    //   Devuelve el número de jugadores cargados (0 si no existe).
    //   Líneas con formato inválido son silenciosamente ignoradas.
    // ----------------------------------------------------------------
    inline int cargarCSV(SistemaClub& club,
                         const std::string& ruta = RUTA_CSV)
    {
        std::ifstream in(ruta);
        if (!in.is_open()) return 0;   // archivo inexistente → no es error

        std::string linea;
        // Saltar la cabecera
        if (!std::getline(in, linea)) return 0;

        int cargados = 0;
        while (std::getline(in, linea)) {
            // Eliminar \r residual (Windows)
            if (!linea.empty() && linea.back() == '\r')
                linea.pop_back();
            if (linea.empty()) continue;

            std::istringstream ss(linea);
            std::string campos[13];
            int n = 0;
            while (n < 13 && std::getline(ss, campos[n], ','))
                ++n;

            // Fila malformada: ignorar
            if (n < 13) continue;

            const std::string& cedula    = campos[0];
            const std::string& nombre    = campos[1];
            const std::string& posicion  = campos[3];
            const std::string& categoria = campos[5];

            // Parseo defensivo de enteros
            int fecha_nac = 0, dorsal = 0;
            int goles = 0, asistencias = 0, minutos = 0;
            int asis_e = 0, total_e = 0;
            int solvente_i = 1, comet_i = 1;

            try {
                fecha_nac    = std::stoi(campos[2]);
                dorsal       = std::stoi(campos[4]);
                goles        = std::stoi(campos[6]);
                asistencias  = std::stoi(campos[7]);
                minutos      = std::stoi(campos[8]);
                asis_e       = std::stoi(campos[9]);
                total_e      = std::stoi(campos[10]);
                solvente_i   = std::stoi(campos[11]);
                comet_i      = std::stoi(campos[12]);
            } catch (...) {
                // Campo no numérico → fila corrupta, ignorar
                continue;
            }

            // Crear Jugador y restaurar stats
            Jugador j(cedula, nombre, fecha_nac, posicion, dorsal, categoria);
            for (int i = 0; i < goles;        ++i) j.registrarGol();
            for (int i = 0; i < asistencias;  ++i) j.registrarAsistenciaDeportiva();
            j.registrarMinutos(minutos);
            for (int i = 0; i < total_e; ++i)
                j.registrarAsistencia(i < asis_e);
            if (!solvente_i) j.suspenderPorPago();
            // comet_i: reservado para futura integración con COMET
            (void)comet_i;

            if (club.registrarJugador(j)) ++cargados;
            // Si la cédula ya existe (duplicado en el CSV), se descarta
        }
        return cargados;
    }

} // namespace PersistenciaCSV

// ============================================================
// Método exportarCSV() que SistemaClub necesita exponer para
// que el namespace PersistenciaCSV pueda serializar la tabla.
// Se declara aquí como friend/inline dentro de directorio.h
// mediante un parche en la clase SistemaClub.
//
// NOTA: La implementación real de exportarCSV() se agrega
// directamente en include/directorio.h.
// ============================================================

#endif // PERSISTENCIA_H
