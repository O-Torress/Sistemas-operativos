#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <chrono>
#include <queue>
#include <algorithm>
#include <direct.h>
#include <cstring>

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

// Algoritmo LIFO
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

// Round Robin 
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

// Comparativa de metodo mas eficiente 
void compararResultados(const vector<ResultadoMetodo>& resultados, ofstream& archivoOut) {
    cout << "\n=========================================================\n";
    cout << "               COMPARATIVA DE ALGORITMOS                \n";
    cout << "=========================================================\n";
    cout << left << setw(15) << "Metodo" 
         << setw(12) << "Prom T" 
         << setw(12) << "Prom E" 
         << setw(12) << "Prom I" 
         << setw(15) << "Tiempo (us)" << "\n";

    archivoOut << "=========================================================\n";
    archivoOut << "               COMPARATIVA DE ALGORITMOS                \n";
    archivoOut << "=========================================================\n";
    archivoOut << left << setw(15) << "Metodo" 
               << setw(12) << "Prom T" 
               << setw(12) << "Prom E" 
               << setw(12) << "Prom I" 
               << setw(15) << "Tiempo (us)" << "\n";

    int mejor_idx = 0;
    for (size_t i = 0; i < resultados.size(); ++i) {
        cout << left << setw(15) << resultados[i].nombre
             << setw(12) << resultados[i].prom_T
             << setw(12) << resultados[i].prom_E
             << setw(12) << resultados[i].prom_I
             << setw(15) << resultados[i].duracion_us << "\n";

        archivoOut << left << setw(15) << resultados[i].nombre
                   << setw(12) << resultados[i].prom_T
                   << setw(12) << resultados[i].prom_E
                   << setw(12) << resultados[i].prom_I
                   << setw(15) << resultados[i].duracion_us << "\n";

        // Criterio de selección: Menor tiempo de espera promedio (Prom E)
        if (resultados[i].prom_E < resultados[mejor_idx].prom_E) {
            mejor_idx = i;
        }
    }

    string conclusion = "\nEL MEJOR METODO ES: " + resultados[mejor_idx].nombre +
                        " (Basado en el menor tiempo de espera promedio Prom E = " + 
                        to_string(resultados[mejor_idx].prom_E) + ")\n";

    cout << conclusion;
    archivoOut << conclusion;
}

int main(int argc, char* argv[]) {
    string archivoEntrada;
    double Q = 4; // Quantum por defecto = 4

    auto fileExists = [&](const string &path) {
        ifstream f(path);
        return f.is_open();
    };

    auto joinPath = [&](const string &dir, const string &file) {
        if (dir.empty()) return file;
        string d = dir;
        char sep = '\\';
        if (d.back() == '\\' || d.back() == '/') return d + file;
        return d + sep + file;
    };

    auto parentPath = [&](string path) {
        // Normalize separators
        for (char &c : path) if (c == '/') c = '\\';
        while (!path.empty() && (path.back() == '\\' || path.back() == '/')) path.pop_back();
        size_t pos = path.find_last_of('\\');
        if (pos == string::npos) return string("");
        return path.substr(0, pos);
    };

    auto getCurrentDir = [&]() {
        char buf[1024];
        if (_getcwd(buf, sizeof(buf))) return string(buf);
        return string("");
    };

    // Si se pasa argumento, usarlo directamente
    if (argc > 1) {
        archivoEntrada = argv[1];
    } else {
        string target = "Data.txt";
        string cur = getCurrentDir();
        string p = joinPath(cur, target);
        if (fileExists(p)) {
            archivoEntrada = p;
        } else {
            // Intentar en el directorio del ejecutable
            string exe0(argv[0]);
            string exeDir;
            if (exe0.find('\\') != string::npos || exe0.find('/') != string::npos) exeDir = parentPath(exe0); else exeDir = cur;
            p = joinPath(exeDir, target);
            if (fileExists(p)) {
                archivoEntrada = p;
            } else {
                // Buscar en padres del directorio actual (hasta 10 niveles)
                string dir = cur;
                bool encontrado = false;
                for (int i = 0; i < 10 && !dir.empty(); ++i) {
                    p = joinPath(dir, target);
                    if (fileExists(p)) { archivoEntrada = p; encontrado = true; break; }
                    string parent = parentPath(dir);
                    if (parent.empty() || parent == dir) break;
                    dir = parent;
                }

                if (!encontrado) {
                    cout << "No se encontro 'Data.txt'. Ingrese el nombre o ruta del archivo: ";
                    cin >> archivoEntrada;
                }
            }
        }
    }

    vector<Actividad> actividades = cargarActividades(archivoEntrada);

    if (actividades.empty()) {
        cout << "No se pudieron cargar actividades. Ingrese el nombre del archivo manual: ";
        cin >> archivoEntrada;
        actividades = cargarActividades(archivoEntrada);
        if (actividades.empty()) return 1;
    }

    cout << "Ingrese el valor del Quantum (Q) [Default 4]: ";
    if (!(cin >> Q)) Q = 4;

    ofstream archivoOut("resultados.txt");
    if (!archivoOut.is_open()) {
        cerr << "Error al crear el archivo resultados.txt\n";
        return 1;
    }

    vector<ResultadoMetodo> resultados;
    resultados.push_back(ejecutarFIFO(actividades, archivoOut));
    resultados.push_back(ejecutarLIFO(actividades, archivoOut));
    resultados.push_back(ejecutarRoundRobin(actividades, Q, archivoOut));

    compararResultados(resultados, archivoOut);

    archivoOut.close();
    cout << "\nLos resultados se han guardado exitosamente en 'resultados.txt'.\n";

    return 0;
}