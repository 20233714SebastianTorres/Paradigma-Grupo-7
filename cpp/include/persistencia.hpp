#pragma once
#include "modelos.hpp"
#include <string>

// 4.1.2: Carga inicial de datos desde los 4 CSV.
// Si un archivo no existe, su coleccion se carga vacia.
// Ante un CSV invalido, se conserva sin cambios el objeto Datos recibido.
bool cargarDatos(const std::string& carpeta, Datos& datos, std::string& error);

// 4.1.5: Escribe los 4 archivos CSV con los cambios.
// Todos los pedidos se escriben con fecha y franja (9 campos).
bool guardarDatos(const std::string& carpeta, const Datos& datos, std::string& error);
