#pragma once
#include <string>
#include <vector>

// Modelo compartido: mantener los mismos campos en los modulos del grupo.
struct Punto {
    std::string codigo, nombre, tipo, zona;
};
struct Conexion {
    std::string origen, destino;
    double distanciaKm{};
    int tiempoMin{};
    std::string estado; // habilitada | cerrada
};
struct Camion {
    std::string codigo, tipo;
    double capacidadM3{}, costoKm{};
    std::string estado; // disponible | ocupada
};
struct Mudanza {
    std::string codigo, origen, destino;
    double cargaM3{};
    std::string prioridad, estado, camionAsignado;
    // El integrante 2 agrego estos campos en la propuesta.
    std::string fecha, franja;
};
struct Datos {
    std::vector<Punto> puntos;
    std::vector<Conexion> conexiones;
    std::vector<Camion> camiones;
    std::vector<Mudanza> mudanzas;
};
