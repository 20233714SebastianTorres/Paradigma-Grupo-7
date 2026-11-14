#include "integrante2.hpp"
#include <iostream>
#include <string>

using namespace std;

// Funcion de apoyo simple con un bucle for tradicional
bool existePunto(Datos& datos, string codigo) {
    for (size_t i = 0; i < datos.puntos.size(); i++) {
        if (datos.puntos[i].codigo == codigo) {
            return true;
        }
    }
    return false;
}

// 4.1.3 - Funciones de Registro
bool registrarPunto(Datos& datos) {
    Punto p;
    cout << "Codigo: ";
    cin >> p.codigo;

    if (existePunto(datos, p.codigo)) {
        cout << "Error: El codigo ya existe." << endl;
        return false;
    }

    cout << "Nombre (sin espacios): ";
    cin >> p.nombre;
    cout << "Tipo (sin espacios): ";
    cin >> p.tipo;
    cout << "Zona (sin espacios): ";
    cin >> p.zona;

    datos.puntos.push_back(p);
    cout << "Punto registrado correctamente." << endl;
    return true;
}

bool registrarConexion(Datos& datos) {
    Conexion c;
    cout << "Origen (codigo): ";
    cin >> c.origen;
    cout << "Destino (codigo): ";
    cin >> c.destino;

    if (!existePunto(datos, c.origen) || !existePunto(datos, c.destino)) {
        cout << "Error: Los puntos de origen o destino no existen." << endl;
        return false;
    }

    // Verificar que la conexion no exista ya
    for (size_t i = 0; i < datos.conexiones.size(); i++) {
        if (datos.conexiones[i].origen == c.origen && datos.conexiones[i].destino == c.destino) {
            cout << "Error: La conexion ya existe." << endl;
            return false;
        }
    }

    cout << "Distancia en km: ";
    cin >> c.distanciaKm;
    cout << "Tiempo en minutos: ";
    cin >> c.tiempoMin;
    cout << "Estado (habilitada/cerrada): ";
    cin >> c.estado;

    datos.conexiones.push_back(c);
    cout << "Conexion registrada correctamente." << endl;
    return true;
}

bool registrarCamion(Datos& datos) {
    Camion cam;
    cout << "Codigo del camion: ";
    cin >> cam.codigo;

    for (size_t i = 0; i < datos.camiones.size(); i++) {
        if (datos.camiones[i].codigo == cam.codigo) {
            cout << "Error: El codigo del camion ya existe." << endl;
            return false;
        }
    }

    cout << "Tipo (sin espacios): ";
    cin >> cam.tipo;
    cout << "Capacidad en m3: ";
    cin >> cam.capacidadM3;
    cout << "Costo por km: ";
    cin >> cam.costoKm;
    cout << "Estado (disponible/ocupada): ";
    cin >> cam.estado;

    datos.camiones.push_back(cam);
    cout << "Camion registrado correctamente." << endl;
    return true;
}

bool registrarMudanza(Datos& datos) {
    Mudanza m;
    cout << "Codigo de mudanza: ";
    cin >> m.codigo;

    for (size_t i = 0; i < datos.mudanzas.size(); i++) {
        if (datos.mudanzas[i].codigo == m.codigo) {
            cout << "Error: El codigo de mudanza ya existe." << endl;
            return false;
        }
    }

    cout << "Origen (codigo): ";
    cin >> m.origen;
    cout << "Destino (codigo): ";
    cin >> m.destino;

    if (!existePunto(datos, m.origen) || !existePunto(datos, m.destino)) {
        cout << "Error: Los puntos de origen o destino no existen." << endl;
        return false;
    }

    cout << "Carga en m3: ";
    cin >> m.cargaM3;
    cout << "Prioridad (sin espacios): ";
    cin >> m.prioridad;
    
    m.estado = "pendiente";
    m.camionAsignado = "";
    m.fecha = "";
    m.franja = "";

    datos.mudanzas.push_back(m);
    cout << "Mudanza registrada correctamente." << endl;
    return true;
}

// 4.1.4 - Gestion de Estados
bool gestionarConexion(Datos& datos) {
    string origen, destino;
    cout << "Origen (codigo): ";
    cin >> origen;
    cout << "Destino (codigo): ";
    cin >> destino;

    for (size_t i = 0; i < datos.conexiones.size(); i++) {
        if (datos.conexiones[i].origen == origen && datos.conexiones[i].destino == destino) {
            if (datos.conexiones[i].estado == "habilitada") {
                datos.conexiones[i].estado = "cerrada";
            } else {
                datos.conexiones[i].estado = "habilitada";
            }
            cout << "Estado actualizado a: " << datos.conexiones[i].estado << endl;
            return true; // Retorna true para que se guarde el archivo
        }
    }
    
    cout << "Error: Conexion no encontrada." << endl;
    return false;
}

bool asignarMudanza(Datos& datos) {
    string codMudanza, codCamion;
    cout << "Codigo de mudanza a asignar: ";
    cin >> codMudanza;

    int indiceMudanza = -1;
    for (size_t i = 0; i < datos.mudanzas.size(); i++) {
        if (datos.mudanzas[i].codigo == codMudanza) {
            indiceMudanza = i;
            break;
        }
    }

    if (indiceMudanza == -1 || datos.mudanzas[indiceMudanza].estado != "pendiente") {
        cout << "Error: Mudanza no encontrada o no esta pendiente." << endl;
        return false;
    }

    cout << "Codigo de camion a utilizar: ";
    cin >> codCamion;

    int indiceCamion = -1;
    for (size_t i = 0; i < datos.camiones.size(); i++) {
        if (datos.camiones[i].codigo == codCamion) {
            indiceCamion = i;
            break;
        }
    }

    if (indiceCamion == -1 || datos.camiones[indiceCamion].estado != "disponible") {
        cout << "Error: Camion no encontrado o no esta disponible." << endl;
        return false;
    }

    if (datos.camiones[indiceCamion].capacidadM3 < datos.mudanzas[indiceMudanza].cargaM3) {
        cout << "Error: El camion no tiene capacidad suficiente." << endl;
        return false;
    }

    // Realizar la asignacion
    datos.mudanzas[indiceMudanza].estado = "asignado";
    datos.mudanzas[indiceMudanza].camionAsignado = codCamion;
    datos.camiones[indiceCamion].estado = "ocupada";

    cout << "Mudanza asignada al camion exitosamente." << endl;
    return true;
}

bool entregarMudanza(Datos& datos) {
    string codMudanza;
    cout << "Codigo de mudanza entregada: ";
    cin >> codMudanza;

    int indiceMudanza = -1;
    for (size_t i = 0; i < datos.mudanzas.size(); i++) {
        if (datos.mudanzas[i].codigo == codMudanza) {
            indiceMudanza = i;
            break;
        }
    }

    if (indiceMudanza == -1 || datos.mudanzas[indiceMudanza].estado != "asignado") {
        cout << "Error: Mudanza no encontrada o no esta asignada." << endl;
        return false;
    }

    string codCamion = datos.mudanzas[indiceMudanza].camionAsignado;

    // Liberar el camion
    for (size_t i = 0; i < datos.camiones.size(); i++) {
        if (datos.camiones[i].codigo == codCamion) {
            datos.camiones[i].estado = "disponible";
            break;
        }
    }

    datos.mudanzas[indiceMudanza].estado = "entregado";
    datos.mudanzas[indiceMudanza].camionAsignado = "";

    cout << "Mudanza marcada como entregada. Camion liberado." << endl;
    return true;
}

// Funcionalidad Adicional 
bool agendarMudanza(Datos& datos) {
    string codMudanza;
    cout << "Codigo de mudanza a agendar: ";
    cin >> codMudanza;

    for (size_t i = 0; i < datos.mudanzas.size(); i++) {
        if (datos.mudanzas[i].codigo == codMudanza) {
            cout << "Nueva Fecha (AAAA-MM-DD): ";
            cin >> datos.mudanzas[i].fecha;
            
            cout << "Nueva Franja horaria (sin espacios, ej: 10:00-12:00): ";
            cin >> datos.mudanzas[i].franja;
            
            cout << "Mudanza agendada correctamente." << endl;
            return true;
        }
    }
    
    cout << "Error: Mudanza no encontrada." << endl;
    return false;
}


FuncionesGrupo crearFuncionesIntegrante2() {
    FuncionesGrupo f;
    f.registrarPunto = registrarPunto;
    f.registrarConexion = registrarConexion;
    f.registrarCamion = registrarCamion;
    f.registrarMudanza = registrarMudanza;
    f.gestionarConexion = gestionarConexion;
    f.asignarMudanza = asignarMudanza;
    f.entregarMudanza = entregarMudanza;
    f.agendarMudanza = agendarMudanza; 
    return f;
}