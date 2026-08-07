#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <chrono>
#include <queue>
#include <algorithm>

using namespace std;

struct Actividad {
    string id;
    double ti;
    double t;
    double tf = 0;
    double T = 0;
    double E = 0;
    double I = 0;
};

struct ResultadoMetodo {
    string nombre;
    double prom_T;
    double prom_E;
    double prom_I;
    double duracion_us; // microsegundos
};

// Función para cargar datos desde un archivo .txt (Formato por línea: ID ti t)
vector<Actividad> cargarActividades(const string& filename) {
    vector<Actividad> actividades;
    ifstream archivo(filename);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << filename << endl;
        return actividades;
    }
    string id;
    double ti, t;
    while (archivo >> id >> ti >> t) {
        actividades.push_back({id, ti, t});
    }
    archivo.close();
    return actividades;
}
// Algoritmo FIFO 
ResultadoMetodo ejecutarFIFO(vector<Actividad> lista, ofstream& archivoOut) {
    auto inicio = chrono::high_resolution_clock::now();
    int N = lista.size();
    double tiempo_actual = 0, suma_T = 0, suma_E = 0, suma_I = 0;
    int i = 0;

    cout << "\n========================================\n";
    cout << "          EJECUCIÓN METODO FIFO         \n";
    cout << "========================================\n";
    archivoOut << "========================================\n";
    archivoOut << "          EJECUCIÓN METODO FIFO         \n";
    archivoOut << "========================================\n";

    while (i < N) {
        if (tiempo_actual < lista[i].ti) {
            cout << "[Tiempo " << tiempo_actual << "] NOLOOP / CPU ociosa\n";
            archivoOut << "[Tiempo " << tiempo_actual << "] NOLOOP / CPU ociosa\n";
            tiempo_actual = lista[i].ti;
        }

        lista[i].tf = tiempo_actual + lista[i].t;
        lista[i].T = lista[i].tf - lista[i].ti;
        lista[i].E = lista[i].T - lista[i].t;
        lista[i].I = lista[i].t / lista[i].T;

        cout << "Actividad: " << lista[i].id << " | tf: " << lista[i].tf 
             << " | T: " << lista[i].T << " | E: " << lista[i].E << " | I: " << lista[i].I << "\n";
        archivoOut << "Actividad: " << lista[i].id << " | tf: " << lista[i].tf 
                   << " | T: " << lista[i].T << " | E: " << lista[i].E << " | I: " << lista[i].I << "\n";

        tiempo_actual = lista[i].tf;
        suma_T += lista[i].T;
        suma_E += lista[i].E;
        suma_I += lista[i].I;
        i = i + 1;
    }

    double prom_T = suma_T / N;
    double prom_E = suma_E / N;
    double prom_I = suma_I / N;

    auto fin = chrono::high_resolution_clock::now();
    double duracion = chrono::duration<double, micro>(fin - inicio).count();

    cout << "\n--- Promedios FIFO ---\n"
         << "Prom T: " << prom_T << " | Prom E: " << prom_E << " | Prom I: " << prom_I << "\n"
         << "Tiempo de ejecucion: " << duracion << " us\n";

    archivoOut << "\n--- Promedios FIFO ---\n"
               << "Prom T: " << prom_T << " | Prom E: " << prom_E << " | Prom I: " << prom_I << "\n"
               << "Tiempo de ejecucion: " << duracion << " us\n\n";

    return {"FIFO", prom_T, prom_E, prom_I, duracion};
}