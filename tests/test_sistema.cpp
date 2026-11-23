#include <iostream>
#include <cassert>
#include <sstream>
#include <string>
#include <cmath>
#include "../include/models.h"
#include "../include/directorio.h"
#include "../include/ranking_bst.h"

// ========================================================
// SUITE DE PRUEBAS AUTOMATIZADAS - SISTEMA CLUB 'GV'
// ========================================================

void testEstadoInicialVacio() {
    std::cout << "[RUN ] Test 1: Estado inicial vacio de SistemaClub...\n";
    SistemaClub club(101);

    assert(club.getTotalJugadores() == 0);
    assert(club.buscarJugador("12345678") == nullptr);
    assert(club.buscarJugador("") == nullptr);
    assert(club.buscarJugador("99999999") == nullptr);

    std::cout << "[PASS] Test 1: Estado inicial vacio verificado con exito.\n";
}

void testRegistroYColisionesHash() {
    std::cout << "[RUN ] Test 2: Registro de multiples jugadores y manejo de colisiones...\n";
    SistemaClub club(101);

    // Registramos varios jugadores
    Jugador j1("10001", "Lionel Messi", 19870624, "Delantero", 10, "Sub-16");
    Jugador j2("10002", "Angel Di Maria", 19880214, "Mediocampista", 11, "Sub-16");
    Jugador j3("10003", "Emiliano Martinez", 19920902, "Portero", 23, "Sub-16");
    Jugador j4("10004", "Rodrigo De Paul", 19940524, "Mediocampista", 7, "Sub-16");

    assert(club.registrarJugador(j1) == true);
    assert(club.registrarJugador(j2) == true);
    assert(club.registrarJugador(j3) == true);
    assert(club.registrarJugador(j4) == true);

    assert(club.getTotalJugadores() == 4);

    // Verificamos que todos sean recuperables individualmente con datos íntegros
    Jugador* r1 = club.buscarJugador("10001");
    Jugador* r2 = club.buscarJugador("10002");
    Jugador* r3 = club.buscarJugador("10003");
    Jugador* r4 = club.buscarJugador("10004");

    assert(r1 != nullptr && r1->getNombre() == "Lionel Messi");
    assert(r2 != nullptr && r2->getNombre() == "Angel Di Maria");
    assert(r3 != nullptr && r3->getNombre() == "Emiliano Martinez");
    assert(r4 != nullptr && r4->getNombre() == "Rodrigo De Paul");

    // Forzamos colisión: probamos con una tabla pequeña de 3 cubetas
    // Por el principio del palomar, al registrar 5 elementos habrá colisiones en cubetas
    SistemaClub clubPequeno(3);
    Jugador p1("101", "Jugador A", 20100101, "Defensa", 2, "Sub-14");
    Jugador p2("102", "Jugador B", 20100102, "Defensa", 3, "Sub-14");
    Jugador p3("103", "Jugador C", 20100103, "Mediocampista", 5, "Sub-14");
    Jugador p4("104", "Jugador D", 20100104, "Delantero", 9, "Sub-14");
    Jugador p5("105", "Jugador E", 20100105, "Portero", 1, "Sub-14");

    assert(clubPequeno.registrarJugador(p1) == true);
    assert(clubPequeno.registrarJugador(p2) == true);
    assert(clubPequeno.registrarJugador(p3) == true);
    assert(clubPequeno.registrarJugador(p4) == true);
    assert(clubPequeno.registrarJugador(p5) == true);
    assert(clubPequeno.getTotalJugadores() == 5);

    // Todos los jugadores en cubetas colisionadas deben seguir siendo accesibles
    assert(clubPequeno.buscarJugador("101") != nullptr);
    assert(clubPequeno.buscarJugador("102") != nullptr);
    assert(clubPequeno.buscarJugador("103") != nullptr);
    assert(clubPequeno.buscarJugador("104") != nullptr);
    assert(clubPequeno.buscarJugador("105") != nullptr);

    std::cout << "[PASS] Test 2: Insercion multiple y colisiones de Tabla Hash verificadas.\n";
}

void testRechazoCedulasDuplicadas() {
    std::cout << "[RUN ] Test 3: Bloqueo estricto de cedulas duplicadas en O(1)...\n";
    SistemaClub club(101);

    Jugador jOriginal("30123456", "Carlos Perez", 20090101, "Delantero", 9, "Sub-15");
    assert(club.registrarJugador(jOriginal) == true);
    assert(club.getTotalJugadores() == 1);

    // Intentar registrar el mismo jugador nuevamente
    assert(club.registrarJugador(jOriginal) == false);
    assert(club.getTotalJugadores() == 1);

    // Intentar registrar otro jugador con datos distintos pero MISMA cédula
    Jugador jClon("30123456", "Nombre Falso", 20100505, "Portero", 1, "Sub-13");
    assert(club.registrarJugador(jClon) == false);
    assert(club.getTotalJugadores() == 1);

    // Aseguramos que los datos del registro original no fueron corrompidos
    Jugador* recuperado = club.buscarJugador("30123456");
    assert(recuperado != nullptr);
    assert(recuperado->getNombre() == "Carlos Perez");

    std::cout << "[PASS] Test 3: Rechazo de duplicados O(1) e integridad de datos comprobados.\n";
}

void testBusquedaO1YModificacion() {
    std::cout << "[RUN ] Test 4: Busqueda O(1) y mutacion mediante puntero...\n";
    SistemaClub club(101);

    Jugador j("28555444", "Andres Iniesta", 19840511, "Mediocampista", 8, "Sub-16");
    club.registrarJugador(j);

    Jugador* jPtr = club.buscarJugador("28555444");
    assert(jPtr != nullptr);
    assert(jPtr->getCedula() == "28555444");
    assert(jPtr->getNombre() == "Andres Iniesta");

    // Verificamos que la búsqueda inexistente retorne nullptr
    assert(club.buscarJugador("99999999") == nullptr);
    assert(club.buscarJugador("00000000") == nullptr);

    // Mutación en memoria: suspender y reactivar pagos
    assert(jPtr->esConvocable() == true);
    jPtr->suspenderPorPago();
    assert(jPtr->esConvocable() == false);

    // Verificamos que buscarlo de nuevo refleja el cambio
    Jugador* jPtr2 = club.buscarJugador("28555444");
    assert(jPtr2->esConvocable() == false);
    jPtr2->registrarPago();
    assert(jPtr2->esConvocable() == true);

    std::cout << "[PASS] Test 4: Busqueda O(1) y mutacion via puntero completadas con exito.\n";
}

void testSemaforoYListaMedicaLIFO() {
    std::cout << "[RUN ] Test 5: Lógica del Semáforo y orden LIFO en Historial Medico...\n";
    Jugador atleta("25111222", "Radamel Falcao", 19860210, "Delantero", 9, "Sub-16");

    // 1. Estado inicial: apto y convocable
    assert(atleta.esConvocable() == true);
    assert(atleta.historial_medico.is_empty() == true);

    // 2. Registrar primera lesión (sin alta)
    EventoMedico lesion1("20261101", "Lesion", "Rotura de ligamento cruzado", false);
    atleta.historial_medico.insert_first(lesion1);

    assert(atleta.historial_medico.is_empty() == false);
    assert(atleta.historial_medico.get_first().descripcion == "Rotura de ligamento cruzado");
    assert(atleta.esConvocable() == false); // Semáforo en ROJO

    // 3. Registrar parte posterior con Alta Médica (LIFO: queda en la cabeza)
    EventoMedico alta("20261120", "Control", "Alta medica y reintegro", true);
    atleta.historial_medico.insert_first(alta);

    assert(atleta.historial_medico.get_first().descripcion == "Alta medica y reintegro");
    assert(atleta.historial_medico.get_first().alta_medica == true);
    assert(atleta.esConvocable() == true); // Semáforo en VERDE

    // 4. Registrar recaída posterior (LIFO: sobreescribe el estado de la cabeza)
    EventoMedico recaida("20261125", "Lesion", "Sobrecarga muscular", false);
    atleta.historial_medico.insert_first(recaida);

    assert(atleta.historial_medico.get_first().descripcion == "Sobrecarga muscular");
    assert(atleta.esConvocable() == false); // Semáforo en ROJO nuevamente

    // 5. Verificamos copia profunda (Regla de Tres en ListaMedica)
    Jugador clon = atleta;
    assert(!clon.historial_medico.is_empty());
    assert(clon.historial_medico.get_first().descripcion == "Sobrecarga muscular");
    
    // Limpiamos la lista del original y verificamos que el clon no sufra dangling pointers
    atleta.historial_medico.limpiar();
    assert(atleta.historial_medico.is_empty() == true);
    assert(clon.historial_medico.is_empty() == false);
    assert(clon.historial_medico.get_first().descripcion == "Sobrecarga muscular");

    std::cout << "[PASS] Test 5: Semaforo medico, orden LIFO y copia profunda superados.\n";
}

void testAsistenciaAEntrenamientos() {
    std::cout << "[RUN ] Test 6: Registro de asistencia y calculo de rendimiento...\n";
    Jugador jugador("27333444", "Federico Valverde", 19980722, "Mediocampista", 15, "Sub-16");

    // 0 entrenamientos registrados -> 100.0% por defecto
    assert(std::fabs(jugador.getPorcentajeAsistencia() - 100.0f) < 0.001f);

    // Asistencia 1 de 1 -> 100.0%
    jugador.registrarAsistencia(true);
    assert(std::fabs(jugador.getPorcentajeAsistencia() - 100.0f) < 0.001f);

    // Inasistencia: 1 de 2 -> 50.0%
    jugador.registrarAsistencia(false);
    assert(std::fabs(jugador.getPorcentajeAsistencia() - 50.0f) < 0.001f);

    // Asistencias consecutivas: 2 de 3 (66.666%) y 3 de 4 (75.0%)
    jugador.registrarAsistencia(true);
    assert(std::fabs(jugador.getPorcentajeAsistencia() - 66.6667f) < 0.01f);

    jugador.registrarAsistencia(true);
    assert(std::fabs(jugador.getPorcentajeAsistencia() - 75.0f) < 0.001f);

    std::cout << "[PASS] Test 6: Calculo de porcentaje de asistencia verificado.\n";
}

void testListarJugadoresRedireccionOutput() {
    std::cout << "[RUN ] Test 7: Redireccion de std::cout en listarJugadores()...\n";

    // 1. Probar club vacío
    SistemaClub clubVacio(101);
    std::streambuf* bufferOriginal = std::cout.rdbuf();
    std::ostringstream salidaVacio;

    std::cout.rdbuf(salidaVacio.rdbuf());
    clubVacio.listarJugadores();
    std::cout.rdbuf(bufferOriginal);

    std::string strVacio = salidaVacio.str();
    assert(strVacio.find("No hay jugadores registrados") != std::string::npos);

    // 2. Probar club con jugadores
    SistemaClub club(101);
    club.registrarJugador(Jugador("11111111", "Cristiano Ronaldo", 19850205, "Delantero", 7, "Sub-16"));
    club.registrarJugador(Jugador("22222222", "Luka Modric", 19850909, "Mediocampista", 10, "Sub-16"));

    std::ostringstream salidaLleno;
    std::cout.rdbuf(salidaLleno.rdbuf());
    club.listarJugadores();
    std::cout.rdbuf(bufferOriginal);

    std::string strLleno = salidaLleno.str();
    assert(strLleno.find("LISTA COMPLETA DE JUGADORES DEL CLUB") != std::string::npos);
    assert(strLleno.find("Total de registrados: 2") != std::string::npos);
    assert(strLleno.find("Cristiano Ronaldo") != std::string::npos);
    assert(strLleno.find("Luka Modric") != std::string::npos);
    assert(strLleno.find("Jugador #1") != std::string::npos);
    assert(strLleno.find("Jugador #2") != std::string::npos);

    std::cout << "[PASS] Test 7: Redireccion de flujo y formato de listado validados.\n";
}

int main() {
    std::cout << "====================================================\n";
    std::cout << " INICIANDO SUITE DE PRUEBAS AUTOMATIZADAS (C++17) \n";
    std::cout << "====================================================\n\n";

    testEstadoInicialVacio();
    testRegistroYColisionesHash();
    testRechazoCedulasDuplicadas();
    testBusquedaO1YModificacion();
    testSemaforoYListaMedicaLIFO();
    testAsistenciaAEntrenamientos();
    testListarJugadoresRedireccionOutput();

    std::cout << "\n====================================================\n";
    std::cout << " RESULTADO: 7/7 PRUEBAS SUPERADAS EXITOSAMENTE [OK] \n";
    std::cout << "====================================================\n";

    // ============================================================
    // TESTS 8-12: RANKING BST
    // ============================================================

    // Auxiliar: crea un Jugador con stats ya fijadas
    auto crearJugadorConStats = [](const std::string& ced, const std::string& nom,
                                   int goles, int asis) -> Jugador {
        Jugador j(ced, nom, 20100101, "Delantero", 9, "Sub-16");
        for (int i = 0; i < goles; ++i) j.registrarGol();
        for (int i = 0; i < asis;  ++i) j.registrarAsistenciaDeportiva();
        return j;
    };

    // ---- Test 8: BST vacío ----
    std::cout << "\n[RUN ] Test 8: RankingBST vacio y estado inicial...\n";
    {
        RankingBST ranking;
        assert(ranking.estaVacio() == true);
        assert(ranking.altura() == 0);
        std::ostringstream os;
        ranking.imprimirRankingDescendente(os);
        assert(os.str().find("vacio") != std::string::npos);
    }
    std::cout << "[PASS] Test 8: RankingBST vacio verificado.\n";

    // ---- Test 9: Inserción y orden descendente ----
    std::cout << "[RUN ] Test 9: Insercion y orden descendente en RankingBST...\n";
    {
        RankingBST ranking;
        // A=10pts, B=6pts, C=4pts → orden esperado en salida: A, B, C
        Jugador jA = crearJugadorConStats("30001", "Jugador A", 5, 0);
        Jugador jB = crearJugadorConStats("30002", "Jugador B", 2, 2);
        Jugador jC = crearJugadorConStats("30003", "Jugador C", 1, 2);
        ranking.insertar(jA);
        ranking.insertar(jB);
        ranking.insertar(jC);

        assert(!ranking.estaVacio());
        std::ostringstream os;
        ranking.imprimirRankingDescendente(os);
        std::string salida = os.str();

        assert(salida.find("RANKING DEPORTIVO") != std::string::npos);
        size_t posA = salida.find("Jugador A");
        size_t posB = salida.find("Jugador B");
        size_t posC = salida.find("Jugador C");
        assert(posA != std::string::npos);
        assert(posB != std::string::npos);
        assert(posC != std::string::npos);
        assert(posA < posB);
        assert(posB < posC);
    }
    std::cout << "[PASS] Test 9: Orden descendente por puntaje verificado.\n";

    // ---- Test 10: Desempate sin pérdida de nodos ----
    std::cout << "[RUN ] Test 10: Desempate deterministico sin perdida de nodos...\n";
    {
        RankingBST ranking;
        // Mismo puntaje (pts=4), diferente cédula → desempate por cédula
        Jugador jX = crearJugadorConStats("50001", "Zidane",    2, 0);
        Jugador jY = crearJugadorConStats("50002", "Maldini",   2, 0);
        Jugador jZ = crearJugadorConStats("50003", "Cannavaro", 2, 0);
        ranking.insertar(jX);
        ranking.insertar(jY);
        ranking.insertar(jZ);

        std::ostringstream os;
        ranking.imprimirRankingDescendente(os);
        std::string salida = os.str();

        assert(salida.find("Zidane")     != std::string::npos);
        assert(salida.find("Maldini")    != std::string::npos);
        assert(salida.find("Cannavaro")  != std::string::npos);
        // Cedula mayor ("50003") va a la derecha → aparece primero en In-Order inverso
        assert(salida.find("Cannavaro") < salida.find("Maldini"));
        assert(salida.find("Maldini")   < salida.find("Zidane"));
    }
    std::cout << "[PASS] Test 10: Desempate deterministico y sin perdida de nodos.\n";

    // ---- Test 11: vaciar() y destructor ----
    std::cout << "[RUN ] Test 11: Liberacion de memoria: vaciar() y destructor...\n";
    {
        RankingBST ranking;
        for (int i = 0; i < 10; ++i) {
            std::string ced = "7000" + std::to_string(i);
            std::string nom = "Atleta " + std::to_string(i);
            Jugador j = crearJugadorConStats(ced, nom, i, i);
            ranking.insertar(j);
        }
        assert(!ranking.estaVacio());
        ranking.vaciar();
        assert(ranking.estaVacio());
        assert(ranking.altura() == 0);
        // Re-insertar después de vaciar
        Jugador j2 = crearJugadorConStats("70099", "Post-Vaciado", 3, 1);
        ranking.insertar(j2);
        assert(!ranking.estaVacio());
    } // destructor al salir del scope
    std::cout << "[PASS] Test 11: vaciar() y destructor liberan memoria sin doble-free.\n";

    // ---- Test 12: llenarRanking desde SistemaClub ----
    std::cout << "[RUN ] Test 12: Construccion de RankingBST desde SistemaClub...\n";
    {
        SistemaClub club(101);
        Jugador j1 = crearJugadorConStats("80001", "Ronaldo", 5, 1); // pts=11, goles=5
        Jugador j2 = crearJugadorConStats("80002", "Messi",   4, 3); // pts=11, goles=4
        Jugador j3 = crearJugadorConStats("80003", "Neymar",  3, 2); // pts=8
        Jugador j4 = crearJugadorConStats("80004", "Mbappe",  1, 0); // pts=2
        club.registrarJugador(j1);
        club.registrarJugador(j2);
        club.registrarJugador(j3);
        club.registrarJugador(j4);
        assert(club.getTotalJugadores() == 4);

        RankingBST ranking;
        club.llenarRanking(ranking);
        assert(!ranking.estaVacio());

        std::ostringstream os;
        ranking.imprimirRankingDescendente(os);
        std::string salida = os.str();

        assert(salida.find("Ronaldo") != std::string::npos);
        assert(salida.find("Messi")   != std::string::npos);
        assert(salida.find("Neymar")  != std::string::npos);
        assert(salida.find("Mbappe")  != std::string::npos);
        // Neymar (pts=8) aparece antes que Mbappe (pts=2)
        assert(salida.find("Neymar") < salida.find("Mbappe"));
        // Ronaldo (pts=11, goles=5) desempata por goles contra Messi (pts=11, goles=4)
        assert(salida.find("Ronaldo") < salida.find("Messi"));
    }
    std::cout << "[PASS] Test 12: llenarRanking y orden complejo superados.\n";

    std::cout << "\n====================================================\n";
    std::cout << " RESULTADO FINAL: 12/12 PRUEBAS SUPERADAS [OK]      \n";
    std::cout << "====================================================\n";

    return 0;
}
