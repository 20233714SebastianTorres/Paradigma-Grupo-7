#include "integrante3.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

namespace {

void limpiarSaltoDeLinea() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Busca un punto por codigo
const Punto* buscarPunto(const Datos& datos, const string& codigo) {
    for (const auto& p : datos.puntos)
        if (p.codigo == codigo) return &p;
    return nullptr;
}

// Busca un camion por codigo
const Camion* buscarCamion(const Datos& datos, const string& codigo) {
    for (const auto& c : datos.camiones)
        if (c.codigo == codigo) return &c;
    return nullptr;
}

// Estructura para almacenar un camino encontrado
struct Camino {
    vector<string> nodos;
    double distanciaTotal;
};

// Algoritmo DFS para buscar todos los caminos simples entre origen y destino
void buscarCaminosDFS(const Datos& datos,
                      const string& actual,
                      const string& destino,
                      vector<string>& visitados,
                      double distanciaAcumulada,
                      vector<Camino>& caminosEncontrados) {
    visitados.push_back(actual);

    if (actual == destino) {
        caminosEncontrados.push_back({visitados, distanciaAcumulada});
    } else {
        for (const auto& c : datos.conexiones) {
            if (c.estado != "habilitada" || c.distanciaKm <= 0) continue;
            string vecino = "";
            if (c.origen == actual) vecino = c.destino;
            else if (c.destino == actual) vecino = c.origen;

            if (!vecino.empty()) {
                bool yaVisitado = false;
                for (const auto& v : visitados) {
                    if (v == vecino) {
                        yaVisitado = true;
                        break;
                    }
                }
                if (!yaVisitado) {
                    buscarCaminosDFS(datos, vecino, destino, visitados, distanciaAcumulada + c.distanciaKm, caminosEncontrados);
                }
            }
        }
    }

    visitados.pop_back();
}

// Encuentra la ruta habilitada mas corta entre origen y destino
bool rutaOptima(const Datos& datos, const string& origen, const string& destino, Camino& mejorCamino) {
    vector<Camino> caminos;
    vector<string> visitados;
    buscarCaminosDFS(datos, origen, destino, visitados, 0.0, caminos);

    if (caminos.empty()) return false;

    mejorCamino = caminos[0];
    for (size_t i = 1; i < caminos.size(); ++i) {
        if (caminos[i].distanciaTotal < mejorCamino.distanciaTotal) {
            mejorCamino = caminos[i];
        }
    }
    return true;
}

}

// 4.1.6 — consultarPuntos
bool consultarPuntos(Datos& datos) {
    if (datos.puntos.empty()) {
        cout << "No hay puntos registrados." << endl;
        return false;
    }
    cout << "\n=== PUNTOS (" << datos.puntos.size() << " registros) ===" << endl;
    cout << left
         << setw(6)  << "Cod"
         << setw(32) << "Nombre"
         << setw(14) << "Tipo"
         << "Zona" << endl;
    cout << string(62, '-') << endl;
    for (const auto& p : datos.puntos) {
        cout << setw(6)  << p.codigo
             << setw(32) << p.nombre
             << setw(14) << p.tipo
             << p.zona << endl;
    }
    return false;
}

// 4.1.6 — consultarConexiones
bool consultarConexiones(Datos& datos) {
    string codigo;
    cout << "Codigo del punto: " << flush;
    cin >> codigo;
    limpiarSaltoDeLinea();

    if (!buscarPunto(datos, codigo)) {
        cout << "Error: el punto '" << codigo << "' no existe." << endl;
        return false;
    }

    cout << "\n=== CONEXIONES del punto " << codigo << " ===" << endl;
    bool hayAlguna = false;
    for (const auto& c : datos.conexiones) {
        if (c.origen != codigo && c.destino != codigo) continue;
        hayAlguna = true;
        string vecino = (c.origen == codigo) ? c.destino : c.origen;
        string dir    = (c.origen == codigo) ? "->" : "<-";
        cout << "  " << dir << " " << vecino
             << "  |  " << fixed << setprecision(1) << c.distanciaKm << " km"
             << "  |  " << c.tiempoMin << " min"
             << "  |  " << c.estado << endl;
    }
    if (!hayAlguna)
        cout << "  (sin conexiones registradas para este punto)" << endl;
    return false;
}

// 4.1.6 — consultarMudanzasEstado
bool consultarMudanzasEstado(Datos& datos) {
    string estado;
    cout << "Estado a consultar (pendiente/asignado/entregado): " << flush;
    cin >> estado;
    limpiarSaltoDeLinea();

    cout << "\n=== MUDANZAS con estado '" << estado << "' ===" << endl;
    bool hayAlguna = false;
    for (const auto& m : datos.mudanzas) {
        if (m.estado != estado) continue;
        hayAlguna = true;
        cout << "  [" << m.codigo << "]"
             << "  " << m.origen << " -> " << m.destino
             << "  |  " << fixed << setprecision(1) << m.cargaM3 << " m3"
             << "  |  prioridad: " << m.prioridad;
        if (!m.camionAsignado.empty())
            cout << "  |  camion: " << m.camionAsignado;
        if (!m.fecha.empty())
            cout << "  |  " << m.fecha << " " << m.franja;
        cout << endl;
    }
    if (!hayAlguna)
        cout << "  (ninguna mudanza con ese estado)" << endl;
    return false;
}

// 4.1.6 — consultarMudanzasCamion
bool consultarMudanzasCamion(Datos& datos) {
    string codCamion;
    cout << "Codigo del camion: " << flush;
    cin >> codCamion;
    limpiarSaltoDeLinea();

    if (!buscarCamion(datos, codCamion)) {
        cout << "Error: el camion '" << codCamion << "' no existe." << endl;
        return false;
    }

    cout << "\n=== MUDANZAS asignadas al camion " << codCamion << " ===" << endl;
    bool hayAlguna = false;
    for (const auto& m : datos.mudanzas) {
        if (m.camionAsignado != codCamion) continue;
        hayAlguna = true;
        cout << "  [" << m.codigo << "]"
             << "  " << m.origen << " -> " << m.destino
             << "  |  " << fixed << setprecision(1) << m.cargaM3 << " m3"
             << "  |  estado: " << m.estado;
        if (!m.fecha.empty())
            cout << "  |  " << m.fecha << " " << m.franja;
        cout << endl;
    }
    if (!hayAlguna)
        cout << "  (ninguna mudanza asignada a este camion)" << endl;
    return false;
}

// 4.1.7 — reporteGeneral
bool reporteGeneral(Datos& datos) {
    cout << "\n========================================" << endl;
    cout << "         REPORTE GENERAL DEL SISTEMA" << endl;
    cout << "========================================" << endl;

    // ---- Puntos ----
    cout << "\n--- PUNTOS (" << datos.puntos.size() << " total) ---" << endl;
    int depositos = 0, domicilios = 0;
    for (const auto& p : datos.puntos)
        if (p.tipo == "deposito") ++depositos; else ++domicilios;
    cout << "  Depositos : " << depositos << endl;
    cout << "  Domicilios: " << domicilios << endl;

    // ---- Conexiones ----
    int habilitadas = 0, cerradas = 0;
    for (const auto& c : datos.conexiones)
        if (c.estado == "habilitada") ++habilitadas; else ++cerradas;
    cout << "\n--- CONEXIONES (" << datos.conexiones.size() << " total) ---" << endl;
    cout << "  Habilitadas: " << habilitadas << endl;
    cout << "  Cerradas   : " << cerradas   << endl;

    // ---- Camiones ----
    int disponibles = 0, ocupados = 0;
    for (const auto& c : datos.camiones)
        if (c.estado == "disponible") ++disponibles; else ++ocupados;
    cout << "\n--- CAMIONES (" << datos.camiones.size() << " total) ---" << endl;
    cout << "  Disponibles: " << disponibles << endl;
    cout << "  Ocupados   : " << ocupados   << endl;

    // ---- Mudanzas ----
    int pendientes = 0, asignados = 0, entregados = 0;
    double cargaEnTransito = 0.0;
    vector<string> pendientesAlta;
    for (const auto& m : datos.mudanzas) {
        if (m.estado == "pendiente") {
            ++pendientes;
            if (m.prioridad == "alta") pendientesAlta.push_back(m.codigo);
        } else if (m.estado == "asignado") {
            ++asignados;
            cargaEnTransito += m.cargaM3;
        } else if (m.estado == "entregado") {
            ++entregados;
        }
    }
    cout << "\n--- MUDANZAS (" << datos.mudanzas.size() << " total) ---" << endl;
    cout << "  Pendientes : " << pendientes << endl;
    cout << "  Asignadas  : " << asignados  << endl;
    cout << "  Entregadas : " << entregados << endl;
    cout << fixed << setprecision(1);
    cout << "  Carga en transito: " << cargaEnTransito << " m3" << endl;

    if (!pendientesAlta.empty()) {
        cout << "\n--- PENDIENTES DE PRIORIDAD ALTA ---" << endl;
        for (const auto& cod : pendientesAlta)
            cout << "  " << cod << endl;
    }

    cout << "========================================" << endl;
    return false;
}

// FA — cotizarMudanza
bool cotizarMudanza(Datos& datos) {
    string codMudanza;
    cout << "Codigo de mudanza a cotizar: " << flush;
    cin >> codMudanza;
    limpiarSaltoDeLinea();

    const Mudanza* mud = nullptr;
    for (const auto& m : datos.mudanzas) {
        if (m.codigo == codMudanza) {
            mud = &m;
            break;
        }
    }

    if (!mud) {
        cout << "Error: la mudanza '" << codMudanza << "' no existe." << endl;
        return false;
    }
    if (mud->estado != "pendiente") {
        cout << "Error: la mudanza debe estar en estado 'pendiente' (estado actual: "
             << mud->estado << ")." << endl;
        return false;
    }

    // Calcula de forma automatica la ruta e informacion desde el origen y destino del pedido
    Camino ruta;
    if (!rutaOptima(datos, mud->origen, mud->destino, ruta)) {
        cout << "Error: No existe una ruta habilitada entre el origen ("
             << mud->origen << ") y el destino (" << mud->destino << ")." << endl;
        return false;
    }

    cout << "\nInformacion del pedido " << mud->codigo << ":" << endl;
    cout << "  Origen: " << mud->origen << " | Destino: " << mud->destino
         << " | Carga: " << fixed << setprecision(1) << mud->cargaM3 << " m3" << endl;
    cout << "  Ruta planificada: ";
    for (size_t i = 0; i < ruta.nodos.size(); ++i) {
        cout << ruta.nodos[i] << (i + 1 < ruta.nodos.size() ? " -> " : "");
    }
    cout << endl;
    cout << "  Distancia de la ruta: " << fixed << setprecision(2) << ruta.distanciaTotal << " km" << endl;

    // Evaluar camiones disponibles que cubran la carga
    struct Cotizacion {
        string codigo;
        string tipo;
        double costoTotal;
        double porcentajeCapacidad;
        double precioPorM3;
    };

    vector<Cotizacion> cotizaciones;
    for (const auto& cam : datos.camiones) {
        if (cam.estado != "disponible") continue;
        if (cam.capacidadM3 < mud->cargaM3) continue;

        double costo   = ruta.distanciaTotal * cam.costoKm;
        double porcCap = (mud->cargaM3 / cam.capacidadM3) * 100.0;
        double precM3  = costo / mud->cargaM3;
        cotizaciones.push_back({cam.codigo, cam.tipo, costo, porcCap, precM3});
    }

    if (cotizaciones.empty()) {
        cout << "No hay camiones disponibles con capacidad suficiente para este servicio." << endl;
        return false;
    }

    // Ordenar por costo total ascendente (el primero es el mas economico)
    sort(cotizaciones.begin(), cotizaciones.end(),
         [](const Cotizacion& a, const Cotizacion& b) {
             return a.costoTotal < b.costoTotal;
         });

    cout << "\n=== COTIZACION COMPARATIVA ===" << endl;
    cout << left
         << setw(8)  << "Camion"
         << setw(18) << "Tipo"
         << setw(14) << "Costo total"
         << setw(16) << "Cap. utilizada"
         << "Precio/m3" << endl;
    cout << string(68, '-') << endl;
    for (const auto& cot : cotizaciones) {
        cout << setw(8)  << cot.codigo
             << setw(18) << cot.tipo
             << "S/ " << setw(10) << cot.costoTotal
             << setw(13) << (to_string((int)round(cot.porcentajeCapacidad)) + "%")
             << "S/ " << cot.precioPorM3 << "/m3" << endl;
    }

    cout << "\n>> Se recomienda utilizar el camion " << cotizaciones.front().codigo
         << " (S/ " << cotizaciones.front().costoTotal << " en total)." << endl;

    return false;
}

// Punto de entrada del modulo
void conectarFuncionesIntegrante3(FuncionesGrupo& f) {
    f.consultarPuntos         = consultarPuntos;
    f.consultarConexiones     = consultarConexiones;
    f.consultarMudanzasEstado = consultarMudanzasEstado;
    f.consultarMudanzasCamion = consultarMudanzasCamion;
    f.reporteGeneral          = reporteGeneral;
    f.cotizarMudanza          = cotizarMudanza;
}
