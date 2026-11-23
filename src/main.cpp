#include <iostream>
#include <string>
#include <cctype>
#include <cstdlib>
#include "../include/models.h"
#include "../include/directorio.h"
#include "../include/ranking_bst.h"
#include "../include/persistencia.h"

// Limpieza de pantalla multiplataforma
#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

void limpiarPantalla() {
    std::system(CLEAR);
}

void pausar() {
    std::cout << "\nPresione ENTER para continuar...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

// Función auxiliar para eliminar espacios en blanco al inicio y al final
std::string trim(const std::string& str) {
    size_t inicio = str.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) return "";
    size_t fin = str.find_last_not_of(" \t\r\n");
    return str.substr(inicio, fin - inicio + 1);
}

// Validador estricto de cédula: solo dígitos numéricos (3 a 12 caracteres, sin letras, puntos ni espacios)
bool esCedulaValida(const std::string& str) {
    if (str.length() < 3 || str.length() > 12) {
        return false;
    }
    for (char c : str) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

std::string leerCedula(const std::string& mensaje) {
    std::string entrada;
    while (true) {
        std::cout << mensaje;
        if (!std::getline(std::cin, entrada)) {
            std::cin.clear();
            continue;
        }
        if (esCedulaValida(entrada)) {
            return entrada;
        }
        std::cout << "[!] Error: La cedula debe contener solo digitos numericos (entre 3 y 12 caracteres, sin letras, puntos ni espacios).\n";
    }
}

// Validador de nombre completo: solo letras (incluyendo acentos) y espacios, mínimo 2 letras, sin números ni símbolos
bool esNombreValido(const std::string& str) {
    std::string limpia = trim(str);
    if (limpia.empty()) return false;

    int totalLetras = 0;
    for (size_t i = 0; i < limpia.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(limpia[i]);
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            totalLetras++;
        } else if (c == ' ') {
            // Espacio intermedio permitido
        } else if (c >= 128) {
            // Caracteres extendidos (letras acentuadas y ñ en UTF-8 o codificaciones ANSI)
            totalLetras++;
        } else {
            // Dígitos o símbolos ASCII no permitidos
            return false;
        }
    }
    return totalLetras >= 2;
}

std::string leerNombre(const std::string& mensaje) {
    std::string entrada;
    while (true) {
        std::cout << mensaje;
        if (!std::getline(std::cin, entrada)) {
            std::cin.clear();
            continue;
        }
        if (esNombreValido(entrada)) {
            return trim(entrada);
        }
        std::cout << "[!] Error: El nombre debe contener solo letras y espacios, con un minimo de 2 letras y sin numeros ni simbolos.\n";
    }
}

// Validador de número entero estricto en rango cerrado [min, max]
bool esEnteroValido(const std::string& str, int min, int max, int& salida) {
    std::string limpia = trim(str);
    if (limpia.empty()) return false;

    for (char c : limpia) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }

    try {
        long long val = std::stoll(limpia);
        if (val >= min && val <= max) {
            salida = static_cast<int>(val);
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

int leerEnteroRango(const std::string& mensaje, int min, int max) {
    std::string entrada;
    int valor = 0;
    while (true) {
        std::cout << mensaje;
        if (!std::getline(std::cin, entrada)) {
            std::cin.clear();
            continue;
        }
        if (esEnteroValido(entrada, min, max, valor)) {
            return valor;
        }
        std::cout << "[!] Error: Ingrese un numero entero valido entre " << min << " y " << max << ".\n";
    }
}

// Submenú numérico cerrado para Posición
std::string leerPosicion() {
    std::cout << "\nSeleccione la Posicion:\n";
    std::cout << "  1. Portero\n";
    std::cout << "  2. Defensa\n";
    std::cout << "  3. Mediocampista\n";
    std::cout << "  4. Delantero\n";
    int opc = leerEnteroRango("Opcion (1-4): ", 1, 4);
    switch (opc) {
        case 1: return "Portero";
        case 2: return "Defensa";
        case 3: return "Mediocampista";
        case 4: return "Delantero";
        default: return "Portero";
    }
}

// Submenú numérico cerrado para Categoría
std::string leerCategoria() {
    std::cout << "\nSeleccione la Categoria:\n";
    std::cout << "  1. Sub-13\n";
    std::cout << "  2. Sub-14\n";
    std::cout << "  3. Sub-15\n";
    std::cout << "  4. Sub-16\n";
    int opc = leerEnteroRango("Opcion (1-4): ", 1, 4);
    switch (opc) {
        case 1: return "Sub-13";
        case 2: return "Sub-14";
        case 3: return "Sub-15";
        case 4: return "Sub-16";
        default: return "Sub-13";
    }
}

// Validador de texto no vacío (rechaza cadenas vacías o compuestas únicamente de espacios)
std::string leerTextoNoVacio(const std::string& mensaje) {
    std::string entrada;
    while (true) {
        std::cout << mensaje;
        if (!std::getline(std::cin, entrada)) {
            std::cin.clear();
            continue;
        }
        std::string limpia = trim(entrada);
        if (!limpia.empty()) {
            return limpia;
        }
        std::cout << "[!] Error: El campo no puede estar vacio ni contener solo espacios.\n";
    }
}

int main() {
    // Instanciamos la Tabla Hash en RAM (101 cubetas) y cargamos persistencia CSV si existe
    SistemaClub gestorGV(101);
    int cargadosInicio = PersistenciaCSV::cargarCSV(gestorGV);
    if (cargadosInicio > 0) {
        limpiarPantalla();
        std::cout << "[+] Se cargaron " << cargadosInicio
                  << " jugador(es) desde " << PersistenciaCSV::RUTA_CSV << ".\n";
        pausar();
    }

    int opcion = 0;

    while (opcion != 7) {
        limpiarPantalla();
        std::cout << "=========================================\n";
        std::cout << "    SISTEMA DE GESTION DEPORTIVA 'GV'    \n";
        std::cout << "=========================================\n";
        std::cout << "1. Registrar Nuevo Jugador\n";
        std::cout << "2. Consultar Ficha de Jugador (Busqueda O(1))\n";
        std::cout << "3. Listar Todos los Jugadores\n";
        std::cout << "4. Registrar Evento Medico (Lesion)\n";
        std::cout << "5. Registrar Asistencia a Entrenamiento\n";
        std::cout << "6. Ver Ranking Deportivo (BST)\n";
        std::cout << "7. Salir\n";
        std::cout << "=========================================\n";
        opcion = leerEnteroRango("Seleccione una opcion (1-7): ", 1, 7);

        if (opcion == 1) {
            limpiarPantalla();
            std::cout << "=========================================\n";
            std::cout << "         REGISTRAR NUEVO JUGADOR         \n";
            std::cout << "=========================================\n";
            std::string ced = leerCedula("Cedula/ID (3-12 digitos numericos): ");
            
            // Verificación inmediata de duplicado O(1) antes de solicitar el resto de campos
            if (gestorGV.buscarJugador(ced) != nullptr) {
                std::cout << "\n[-] Error: Ya existe un jugador registrado con la cedula " << ced << ".\n";
                std::cout << "    El registro ha sido cancelado para evitar duplicados.\n";
                pausar();
                continue;
            }

            std::string nom = leerNombre("Nombre Completo: ");
            int dor = leerEnteroRango("Dorsal (1-99): ", 1, 99);
            std::string pos = leerPosicion();
            std::string cat = leerCategoria();

            // Insertamos al jugador y validamos la operación O(1)
            bool registrado = gestorGV.registrarJugador(Jugador(ced, nom, 20100101, pos, dor, cat));
            if (registrado) {
                std::cout << "\n[+] Jugador registrado exitosamente en el directorio.\n";
            } else {
                std::cout << "\n[-] Error: No se pudo registrar el jugador (cedula duplicada).\n";
            }
            pausar();
        }
        else if (opcion == 2) {
            limpiarPantalla();
            std::cout << "=========================================\n";
            std::cout << "       CONSULTAR FICHA DE JUGADOR        \n";
            std::cout << "=========================================\n";
            std::string ced = leerCedula("Ingrese Cedula a buscar: ");
            
            Jugador* j = gestorGV.buscarJugador(ced);
            if (j) {
                j->imprimirFicha();
            } else {
                std::cout << "\n[-] Error: Jugador no encontrado en la base de datos.\n";
            }
            pausar();
        }
        else if (opcion == 3) {
            limpiarPantalla();
            gestorGV.listarJugadores();
            pausar();
        }
        else if (opcion == 4) {
            limpiarPantalla();
            std::cout << "=========================================\n";
            std::cout << "     REGISTRAR EVENTO MEDICO (LESION)    \n";
            std::cout << "=========================================\n";
            std::string ced = leerCedula("Cedula del jugador lesionado: ");
            
            Jugador* j = gestorGV.buscarJugador(ced);
            if (j) {
                std::cout << "\nJugador seleccionado: " << j->getNombre() << "\n";
                std::string desc = leerTextoNoVacio("Descripcion de la lesion: ");
                
                EventoMedico nuevaLesion("20261112", "Lesion", desc, false);
                j->historial_medico.insert_first(nuevaLesion);
                
                std::cout << "\n[!] Lesion registrada exitosamente.\n";
                std::cout << "    El semaforo bloqueador se ha activado ([ROJO] - NO CONVOCABLE).\n";
            } else {
                std::cout << "\n[-] Error: Jugador no encontrado en la base de datos.\n";
            }
            pausar();
        }
        else if (opcion == 5) {
            limpiarPantalla();
            std::cout << "=========================================\n";
            std::cout << "  REGISTRAR ASISTENCIA A ENTRENAMIENTO   \n";
            std::cout << "=========================================\n";
            std::string ced = leerCedula("Cedula del jugador: ");
            
            Jugador* j = gestorGV.buscarJugador(ced);
            if (j) {
                std::cout << "\nJugador seleccionado: " << j->getNombre() << "\n";
                int asis = leerEnteroRango("Asistio al entrenamiento de hoy? (1 = Si, 0 = No): ", 0, 1);
                j->registrarAsistencia(asis == 1);
                std::cout << "\n[+] Asistencia procesada. Rendimiento actual: " 
                          << j->getPorcentajeAsistencia() << "%\n";
            } else {
                std::cout << "\n[-] Error: Jugador no encontrado en la base de datos.\n";
            }
            pausar();
        }
        else if (opcion == 6) {
            limpiarPantalla();
            std::cout << "=========================================\n";
            std::cout << "         RANKING DEPORTIVO (BST)         \n";
            std::cout << "=========================================\n";

            if (gestorGV.getTotalJugadores() == 0) {
                std::cout << "\n[!] No hay jugadores registrados. Registre atletas primero.\n";
                pausar();
                continue;
            }

            // Construye el BST desde los datos actuales de la Tabla Hash
            RankingBST ranking;
            gestorGV.llenarRanking(ranking);
            ranking.imprimirRankingDescendente(std::cout);
            pausar();
        }
        else if (opcion == 7) {
            limpiarPantalla();
            std::cout << "=========================================\n";
            std::cout << "  Saliendo del Sistema de Gestion 'GV'   \n";
            std::cout << "=========================================\n";
            if (PersistenciaCSV::guardarCSV(gestorGV)) {
                std::cout << "Datos guardados en " << PersistenciaCSV::RUTA_CSV << ".\n";
            } else {
                std::cout << "[!] Aviso: No se pudo guardar en " << PersistenciaCSV::RUTA_CSV << ".\n";
            }
            std::cout << "Liberando recursos de la Tabla Hash...\n";
            std::cout << "Sesion finalizada con exito.\n";
        }
    }

    return 0;
}