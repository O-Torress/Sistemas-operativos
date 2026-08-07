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