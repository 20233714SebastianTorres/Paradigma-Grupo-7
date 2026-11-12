// FA - Integrante 1: persistencia de datos (funcionalidades 4.1.2 y 4.1.5).
#include "persistencia.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
namespace fs = std::filesystem;

std::string sinCR(std::string s) {
    if (!s.empty() && s.back() == '\r') s.pop_back();
    return s;
}

bool parsearCSV(const std::string& linea, std::vector<std::string>& campos) {
    campos.clear();
    std::string actual;
    bool entreComillas = false;
    for (std::size_t i = 0; i < linea.size(); ++i) {
        const char c = linea[i];
        if (c == '"') {
            if (entreComillas && i + 1 < linea.size() && linea[i + 1] == '"') {
                actual += '"'; ++i;
            } else {
                entreComillas = !entreComillas;
            }
        } else if (c == ',' && !entreComillas) {
            campos.push_back(actual); actual.clear();
        } else {
            actual += c;
        }
    }
    if (entreComillas) return false;
    campos.push_back(actual);
    return true;
}

std::string csvCampo(const std::string& valor) {
    if (valor.find_first_of(",\"\r\n") == std::string::npos) return valor;
    std::string s = "\"";
    for (char c : valor) { if (c == '"') s += '"'; s += c; }
    return s + "\"";
}

std::string numero(double n) {
    std::ostringstream out;
    out << std::setprecision(15) << n;
    return out.str();
}

bool convertirDouble(const std::string& s, double& result) {
    try {
        std::size_t consumidos = 0;
        result = std::stod(s, &consumidos);
        return consumidos == s.size() && std::isfinite(result);
    } catch (const std::exception&) { return false; }
}

bool convertirInt(const std::string& s, int& result) {
    try {
        std::size_t consumidos = 0;
        result = std::stoi(s, &consumidos);
        return consumidos == s.size();
    } catch (const std::exception&) { return false; }
}

bool leerFilas(const fs::path& archivo,
               std::vector<std::vector<std::string>>& filas,
               std::string& error) {
    if (!fs::exists(archivo)) return true; // Primera ejecucion.
    std::ifstream in(archivo);
    if (!in) { error = "No se pudo abrir " + archivo.string(); return false; }
    std::string linea;
    int numeroLinea = 0;
    while (std::getline(in, linea)) {
        ++numeroLinea;
        linea = sinCR(linea);
        if (linea.empty()) continue;
        std::vector<std::string> c;
        if (!parsearCSV(linea, c)) {
            error = archivo.string() + ": comillas invalidas en linea " + std::to_string(numeroLinea);
            return false;
        }
        // Permitir CSV con o sin encabezado.
        if (filas.empty() && (c[0] == "codigo" || c[0] == "origen")) continue;
        filas.push_back(std::move(c));
    }
    if (!in.eof() && in.fail()) { error = "Error de lectura en " + archivo.string(); return false; }
    return true;
}

bool errorFila(std::string& error, const std::string& archivo, std::size_t fila) {
    error = "Formato invalido en " + archivo + ", registro " + std::to_string(fila + 1);
    return false;
}

bool escribir(const fs::path& path, const std::vector<std::vector<std::string>>& filas,
              std::string& error) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) { error = "No se pudo escribir " + path.string(); return false; }
    for (const auto& fila : filas) {
        for (std::size_t i = 0; i < fila.size(); ++i) {
            if (i) out << ',';
            out << csvCampo(fila[i]);
        }
        out << '\n';
    }
    out.flush();
    if (!out.good()) { error = "Fallo al guardar " + path.string(); return false; }
    return true;
}
} // namespace

bool cargarDatos(const std::string& carpeta, Datos& datos, std::string& error) {
    error.clear();
    Datos nuevos;
    std::vector<std::vector<std::string>> filas;
    const fs::path base(carpeta);

    if (!leerFilas(base / "puntos.csv", filas, error)) return false;
    for (std::size_t i = 0; i < filas.size(); ++i) {
        const auto& c = filas[i];
        if (c.size() != 4) return errorFila(error, "puntos.csv", i);
        nuevos.puntos.push_back({c[0], c[1], c[2], c[3]});
    }
    filas.clear();

    if (!leerFilas(base / "conexiones.csv", filas, error)) return false;
    for (std::size_t i = 0; i < filas.size(); ++i) {
        const auto& c = filas[i];
        Conexion x;
        if (c.size() != 5 || !convertirDouble(c[2], x.distanciaKm) ||
            !convertirInt(c[3], x.tiempoMin)) return errorFila(error, "conexiones.csv", i);
        x.origen = c[0]; x.destino = c[1]; x.estado = c[4];
        nuevos.conexiones.push_back(x);
    }
    filas.clear();

    if (!leerFilas(base / "unidades.csv", filas, error)) return false;
    for (std::size_t i = 0; i < filas.size(); ++i) {
        const auto& c = filas[i];
        Camion x;
        if (c.size() != 5 || !convertirDouble(c[2], x.capacidadM3) ||
            !convertirDouble(c[3], x.costoKm)) return errorFila(error, "unidades.csv", i);
        x.codigo = c[0]; x.tipo = c[1]; x.estado = c[4];
        nuevos.camiones.push_back(x);
    }
    filas.clear();

    if (!leerFilas(base / "pedidos.csv", filas, error)) return false;
    for (std::size_t i = 0; i < filas.size(); ++i) {
        const auto& c = filas[i];
        Mudanza x;
        if ((c.size() != 7 && c.size() != 9) || !convertirDouble(c[3], x.cargaM3))
            return errorFila(error, "pedidos.csv", i);
        x.codigo = c[0]; x.origen = c[1]; x.destino = c[2];
        x.prioridad = c[4]; x.estado = c[5]; x.camionAsignado = c[6];
        if (c.size() == 9) { x.fecha = c[7]; x.franja = c[8]; }
        nuevos.mudanzas.push_back(x);
    }
    datos = std::move(nuevos);
    return true;
}

bool guardarDatos(const std::string& carpeta, const Datos& datos, std::string& error) {
    error.clear();
    const fs::path base(carpeta);
    std::error_code ec;
    fs::create_directories(base, ec);
    if (ec) { error = "No se pudo crear la carpeta " + carpeta + ": " + ec.message(); return false; }
    std::vector<std::vector<std::string>> filas;

    for (const auto& p : datos.puntos) filas.push_back({p.codigo, p.nombre, p.tipo, p.zona});
    if (!escribir(base / "puntos.csv", filas, error)) return false;
    filas.clear();

    for (const auto& c : datos.conexiones)
        filas.push_back({c.origen, c.destino, numero(c.distanciaKm),
                         std::to_string(c.tiempoMin), c.estado});
    if (!escribir(base / "conexiones.csv", filas, error)) return false;
    filas.clear();

    for (const auto& u : datos.camiones)
        filas.push_back({u.codigo, u.tipo, numero(u.capacidadM3), numero(u.costoKm), u.estado});
    if (!escribir(base / "unidades.csv", filas, error)) return false;
    filas.clear();

    for (const auto& m : datos.mudanzas)
        filas.push_back({m.codigo, m.origen, m.destino, numero(m.cargaM3),
                         m.prioridad, m.estado, m.camionAsignado, m.fecha, m.franja});
    return escribir(base / "pedidos.csv", filas, error);
}
