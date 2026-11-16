#include "integrante2.hpp"
#include <iostream>
#include <limits>
#include <string>

using namespace std;

// 4.1.3: reads a number that must be greater than zero. If the input is not
// numeric, the stream is cleared so the menu keeps working.
template <typename T>
bool leerPositivo(const string& mensaje, T& valor) {
    cout << mensaje;
    if (!(cin >> valor)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: debe ingresar un numero." << endl;
        return false;
    }
    if (valor <= 0) {
        cout << "Error: el valor debe ser mayor que cero." << endl;
        return false;
    }
    return true;
}

// Reads a word and checks it is one of the allowed values (c is optional).
bool leerOpcion(const string& mensaje, const string& a, const string& b,
                const string& c, string& valor) {
    cout << mensaje;
    cin >> valor;
    if (valor == a || valor == b || (!c.empty() && valor == c)) return true;
    cout << "Error: valor no valido (" << valor << ")." << endl;
    return false;
}

// Connections are two-way: a-b and b-a are the same road segment.
bool mismoTramo(const Conexion& c, const string& a, const string& b) {
    return (c.origen == a && c.destino == b) || (c.origen == b && c.destino == a);
}

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

    if (c.origen == c.destino) {
        cout << "Error: El origen y el destino deben ser distintos." << endl;
        return false;
    }

    // Verificar que el tramo no exista ya, en cualquier sentido
    for (size_t i = 0; i < datos.conexiones.size(); i++) {
        if (mismoTramo(datos.conexiones[i], c.origen, c.destino)) {
            cout << "Error: La conexion ya existe." << endl;
            return false;
        }
    }

    if (!leerPositivo("Distancia en km: ", c.distanciaKm)) return false;
    if (!leerPositivo("Tiempo en minutos: ", c.tiempoMin)) return false;
    if (!leerOpcion("Estado (habilitada/cerrada): ", "habilitada", "cerrada", "", c.estado))
        return false;

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
    if (!leerPositivo("Capacidad en m3: ", cam.capacidadM3)) return false;
    if (!leerPositivo("Costo por km: ", cam.costoKm)) return false;
    if (!leerOpcion("Estado (disponible/ocupada): ", "disponible", "ocupada", "", cam.estado))
        return false;

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

    if (!leerPositivo("Carga en m3: ", m.cargaM3)) return false;
    if (!leerOpcion("Prioridad (alta/media/baja): ", "alta", "media", "baja", m.prioridad))
        return false;
    
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
        if (mismoTramo(datos.conexiones[i], origen, destino)) {
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

    datos.mudanzas[indiceMudanza].estado = "entregado";
    datos.mudanzas[indiceMudanza].camionAsignado = "";

    // Liberar el camion solo si ya no tiene otras mudanzas asignadas
    bool tieneOtras = false;
    for (size_t i = 0; i < datos.mudanzas.size(); i++) {
        if (datos.mudanzas[i].estado == "asignado" && datos.mudanzas[i].camionAsignado == codCamion) {
            tieneOtras = true;
            break;
        }
    }
    if (tieneOtras) {
        cout << "Mudanza marcada como entregada. El camion " << codCamion
             << " sigue ocupado con otras mudanzas." << endl;
        return true;
    }
    for (size_t i = 0; i < datos.camiones.size(); i++) {
        if (datos.camiones[i].codigo == codCamion) {
            datos.camiones[i].estado = "disponible";
            break;
        }
    }

    cout << "Mudanza marcada como entregada. Camion liberado." << endl;
    return true;
}

// Reads the number formed by the digits s[desde .. desde+largo); -1 if any is not a digit.
int leerDigitos(const string& s, size_t desde, size_t largo) {
    int valor = 0;
    for (size_t i = desde; i < desde + largo; i++) {
        if (s[i] < '0' || s[i] > '9') return -1;
        valor = valor * 10 + (s[i] - '0');
    }
    return valor;
}

// Checks a real calendar date in AAAA-MM-DD format (leap years included).
bool fechaValida(const string& f) {
    if (f.size() != 10 || f[4] != '-' || f[7] != '-') return false;
    int anio = leerDigitos(f, 0, 4), mes = leerDigitos(f, 5, 2), dia = leerDigitos(f, 8, 2);
    if (anio < 1 || mes < 1 || mes > 12 || dia < 1) return false;
    int diasMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool bisiesto = (anio % 4 == 0 && anio % 100 != 0) || anio % 400 == 0;
    int maximo = (mes == 2 && bisiesto) ? 29 : diasMes[mes - 1];
    return dia <= maximo;
}

// Converts HH:MM to minutes since midnight; -1 if the time is not valid.
int minutosDelDia(const string& s, size_t desde) {
    if (s[desde + 2] != ':') return -1;
    int h = leerDigitos(s, desde, 2), m = leerDigitos(s, desde + 3, 2);
    if (h < 0 || h > 23 || m < 0 || m > 59) return -1;
    return h * 60 + m;
}

// Checks a time slot in HH:MM-HH:MM format where the start is before the end.
bool franjaValida(const string& f) {
    if (f.size() != 11 || f[5] != '-') return false;
    int inicio = minutosDelDia(f, 0), fin = minutosDelDia(f, 6);
    return inicio >= 0 && fin >= 0 && inicio < fin;
}

// Funcionalidad Adicional - Autor: Rado Delgado Jean Carlo de Jesus
// Agenda de servicios: asigna fecha y franja horaria a una mudanza no entregada.
bool agendarMudanza(Datos& datos) {
    string codMudanza;
    cout << "Codigo de mudanza a agendar: ";
    cin >> codMudanza;

    for (size_t i = 0; i < datos.mudanzas.size(); i++) {
        if (datos.mudanzas[i].codigo == codMudanza) {
            if (datos.mudanzas[i].estado == "entregado") {
                cout << "Error: La mudanza ya fue entregada." << endl;
                return false;
            }

            string fecha, franja;
            cout << "Nueva Fecha (AAAA-MM-DD): ";
            cin >> fecha;
            if (!fechaValida(fecha)) {
                cout << "Error: fecha no valida (" << fecha << "), use AAAA-MM-DD." << endl;
                return false;
            }

            cout << "Nueva Franja horaria (sin espacios, ej: 10:00-12:00): ";
            cin >> franja;
            if (!franjaValida(franja)) {
                cout << "Error: franja no valida (" << franja << "), use HH:MM-HH:MM." << endl;
                return false;
            }

            datos.mudanzas[i].fecha = fecha;
            datos.mudanzas[i].franja = franja;
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