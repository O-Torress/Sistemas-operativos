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

// ---------------------------------------------------------
// ALGORITMO LIFO (Basado estrictamente en tu diagrama LIFO)
// ---------------------------------------------------------
ResultadoMetodo ejecutarLIFO(vector<Actividad> lista, ofstream& archivoOut) {
    auto inicio = chrono::high_resolution_clock::now();
    int N = lista.size();
    double tiempo_actual = 0, suma_T = 0, suma_E = 0, suma_I = 0;
    int completados = 0;
    vector<bool> completado(N, false);

    cout << "\n========================================\n";
    cout << "          EJECUCIÓN METODO LIFO         \n";
    cout << "========================================\n";
    archivoOut << "========================================\n";
    archivoOut << "          EJECUCIÓN METODO LIFO         \n";
    archivoOut << "========================================\n";

    while (completados < N) {
        int k = -1;
        double max_ti = -1;

        // Seleccionar proceso 'k' no completado con el mayor ti <= tiempo_actual
        for (int j = 0; j < N; ++j) {
            if (!completado[j] && lista[j].ti <= tiempo_actual) {
                if (lista[j].ti > max_ti) {
                    max_ti = lista[j].ti;
                    k = j;
                }
            }
        }

        if (k == -1) { // No existen procesos pendientes con ti <= tiempo_actual
            cout << "[Tiempo " << tiempo_actual << "] NOLOOP / CPU en espera\n";
            archivoOut << "[Tiempo " << tiempo_actual << "] NOLOOP / CPU en espera\n";

            // Buscar ti del próximo proceso pendiente más cercano
            double min_next_ti = 1e9;
            for (int j = 0; j < N; ++j) {
                if (!completado[j] && lista[j].ti > tiempo_actual) {
                    min_next_ti = min(min_next_ti, lista[j].ti);
                }
            }
            tiempo_actual = min_next_ti;
        } else {
            lista[k].tf = tiempo_actual + lista[k].t;
            lista[k].T = lista[k].tf - lista[k].ti;
            lista[k].E = lista[k].T - lista[k].t;
            lista[k].I = lista[k].t / lista[k].T;

            cout << "Actividad: " << lista[k].id << " | tf: " << lista[k].tf 
                 << " | T: " << lista[k].T << " | E: " << lista[k].E << " | I: " << lista[k].I << "\n";
            archivoOut << "Actividad: " << lista[k].id << " | tf: " << lista[k].tf 
                       << " | T: " << lista[k].T << " | E: " << lista[k].E << " | I: " << lista[k].I << "\n";

            tiempo_actual = lista[k].tf;
            completado[k] = true;
            completados++;

            suma_T += lista[k].T;
            suma_E += lista[k].E;
            suma_I += lista[k].I;
        }
    }

    double prom_T = suma_T / N;
    double prom_E = suma_E / N;
    double prom_I = suma_I / N;

    auto fin = chrono::high_resolution_clock::now();
    double duracion = chrono::duration<double, micro>(fin - inicio).count();

    cout << "\n--- Promedios LIFO ---\n"
         << "Prom T: " << prom_T << " | Prom E: " << prom_E << " | Prom I: " << prom_I << "\n"
         << "Tiempo de ejecucion: " << duracion << " us\n";

    archivoOut << "\n--- Promedios LIFO ---\n"
               << "Prom T: " << prom_T << " | Prom E: " << prom_E << " | Prom I: " << prom_I << "\n"
               << "Tiempo de ejecucion: " << duracion << " us\n\n";

    return {"LIFO", prom_T, prom_E, prom_I, duracion};
}

// ---------------------------------------------------------------
// ALGORITMO ROUND ROBIN (Basado estrictamente en tu diagrama RR)
// ---------------------------------------------------------------
ResultadoMetodo ejecutarRoundRobin(vector<Actividad> lista, double Q, ofstream& archivoOut) {
    auto inicio = chrono::high_resolution_clock::now();
    int N = lista.size();
    double tiempo_actual = 0, suma_T = 0, suma_E = 0, suma_I = 0;

    vector<double> restante(N);
    vector<bool> en_cola(N, false);
    vector<bool> finalizado(N, false);
    queue<int> cola;
    int completados = 0;

    for (int j = 0; j < N; ++j) {
        restante[j] = lista[j].t;
    }

    cout << "\n========================================\n";
    cout << "       EJECUCIÓN METODO ROUND ROBIN (Q=" << Q << ")\n";
    cout << "========================================\n";
    archivoOut << "========================================\n";
    archivoOut << "       EJECUCIÓN METODO ROUND ROBIN (Q=" << Q << ")\n";
    archivoOut << "========================================\n";

    while (completados < N) {
        // Encolar los procesos que hayan llegado en ti <= tiempo_actual
        for (int j = 0; j < N; ++j) {
            if (!finalizado[j] && !en_cola[j] && lista[j].ti <= tiempo_actual) {
                cola.push(j);
                en_cola[j] = true;
            }
        }

        if (cola.empty()) {
            tiempo_actual++;
        } else {
            int i = cola.front();
            cola.pop();

            if (restante[i] > Q) {
                tiempo_actual += Q;
                restante[i] -= Q;

                // Verificar nuevos ingresos antes de re-encolar
                for (int j = 0; j < N; ++j) {
                    if (!finalizado[j] && !en_cola[j] && lista[j].ti <= tiempo_actual) {
                        cola.push(j);
                        en_cola[j] = true;
                    }
                }
                cola.push(i); // Volver al final de la cola
            } else {
                tiempo_actual += restante[i];
                restante[i] = 0;
                lista[i].tf = tiempo_actual;
                lista[i].T = lista[i].tf - lista[i].ti;
                lista[i].E = lista[i].T - lista[i].t;
                lista[i].I = lista[i].t / lista[i].T;

                finalizado[i] = true;
                completados++;

                cout << "Actividad: " << lista[i].id << " | tf: " << lista[i].tf 
                     << " | T: " << lista[i].T << " | E: " << lista[i].E << " | I: " << lista[i].I << "\n";
                archivoOut << "Actividad: " << lista[i].id << " | tf: " << lista[i].tf 
                           << " | T: " << lista[i].T << " | E: " << lista[i].E << " | I: " << lista[i].I << "\n";

                suma_T += lista[i].T;
                suma_E += lista[i].E;
                suma_I += lista[i].I;
            }
        }
    }

    double prom_T = suma_T / N;
    double prom_E = suma_E / N;
    double prom_I = suma_I / N;

    auto fin = chrono::high_resolution_clock::now();
    double duracion = chrono::duration<double, micro>(fin - inicio).count();

    cout << "\n--- Promedios Round Robin ---\n"
         << "Prom T: " << prom_T << " | Prom E: " << prom_E << " | Prom I: " << prom_I << "\n"
         << "Tiempo de ejecucion: " << duracion << " us\n";

    archivoOut << "\n--- Promedios Round Robin ---\n"
               << "Prom T: " << prom_T << " | Prom E: " << prom_E << " | Prom I: " << prom_I << "\n"
               << "Tiempo de ejecucion: " << duracion << " us\n\n";

    return {"Round Robin", prom_T, prom_E, prom_I, duracion};
}