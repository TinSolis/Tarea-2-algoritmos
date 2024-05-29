#include <iostream>
#include <vector>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <unordered_set>
#include <tuple>
#include <unordered_map>
#include <cmath>
#include <list>

using namespace std;

const int INF = numeric_limits<int>::max();

// Estructura para representar una arista del grafo
struct Arista {
    int destino;  // Nodo destino de la arista
    int peso;     // Peso de la arista
};

typedef vector<vector<Arista>> Grafo;  // Definición del tipo de dato para el grafo

// Nodo de la cola de Fibonacci
struct NodoFibonacci {
    int nodo;
    int dist;
    int grado;
    NodoFibonacci* padre;
    NodoFibonacci* hijo;
    NodoFibonacci* izquierda;
    NodoFibonacci* derecha;
    bool marcado;

    NodoFibonacci(int nodo, int dist) : nodo(nodo), dist(dist), grado(0), padre(nullptr), hijo(nullptr), izquierda(this), derecha(this), marcado(false) {}
};

// Implementación de la Cola de Fibonacci
class ColaFibonacci {
public:
    ColaFibonacci() : minimo(nullptr), numNodos(0) {}

    void insertar(int nodo, int dist) {
        NodoFibonacci* nuevoNodo = new NodoFibonacci(nodo, dist);
        if (minimo == nullptr) {
            minimo = nuevoNodo;
        } else {
            agregarAlRaiz(nuevoNodo);
            if (nuevoNodo->dist < minimo->dist) {
                minimo = nuevoNodo;
            }
        }
        nodos[nodo] = nuevoNodo;
        numNodos++;
    }

    tuple<int, int> extraerMinimo() {
        NodoFibonacci* z = minimo;
        if (z != nullptr) {
            if (z->hijo != nullptr) {
                list<NodoFibonacci*> hijos;
                NodoFibonacci* x = z->hijo;
                do {
                    hijos.push_back(x);
                    x = x->derecha;
                } while (x != z->hijo);
                for (NodoFibonacci* x : hijos) {
                    agregarAlRaiz(x);
                    x->padre = nullptr;
                }
            }
            quitarDelRaiz(z);
            if (z == z->derecha) {
                minimo = nullptr;
            } else {
                minimo = z->derecha;
                consolidar();
            }
            numNodos--;
            nodos.erase(z->nodo);
            return make_tuple(z->dist, z->nodo);
        }
        return make_tuple(INF, -1);
    }

    void decreaseKey(int nodo, int nuevaDist) {
        NodoFibonacci* x = nodos[nodo];
        if (x == nullptr || nuevaDist >= x->dist) {
            return;
        }
        x->dist = nuevaDist;
        NodoFibonacci* y = x->padre;
        if (y != nullptr && x->dist < y->dist) {
            cortar(x, y);
            cortarCascada(y);
        }
        if (x->dist < minimo->dist) {
            minimo = x;
        }
    }

    bool estaVacio() const {
        return minimo == nullptr;
    }

private:
    NodoFibonacci* minimo;
    int numNodos;
    unordered_map<int, NodoFibonacci*> nodos;

    void agregarAlRaiz(NodoFibonacci* nodo) {
        if (minimo == nullptr) {
            minimo = nodo;
        } else {
            nodo->izquierda = minimo;
            nodo->derecha = minimo->derecha;
            minimo->derecha->izquierda = nodo;
            minimo->derecha = nodo;
        }
    }

    void quitarDelRaiz(NodoFibonacci* nodo) {
        nodo->izquierda->derecha = nodo->derecha;
        nodo->derecha->izquierda = nodo->izquierda;
    }

    void consolidar() {
        int D = static_cast<int>(log(numNodos) / log(2)) + 1;
        vector<NodoFibonacci*> A(D, nullptr);
        list<NodoFibonacci*> raiz;
        NodoFibonacci* w = minimo;
        do {
            raiz.push_back(w);
            w = w->derecha;
        } while (w != minimo);
        for (NodoFibonacci* w : raiz) {
            NodoFibonacci* x = w;
            int d = x->grado;
            while (A[d] != nullptr) {
                NodoFibonacci* y = A[d];
                if (x->dist > y->dist) {
                    swap(x, y);
                }
                enlazar(y, x);
                A[d] = nullptr;
                d++;
            }
            A[d] = x;
        }
        minimo = nullptr;
        for (NodoFibonacci* x : A) {
            if (x != nullptr) {
                if (minimo == nullptr) {
                    minimo = x;
                } else {
                    agregarAlRaiz(x);
                    if (x->dist < minimo->dist) {
                        minimo = x;
                    }
                }
            }
        }
    }

    void enlazar(NodoFibonacci* y, NodoFibonacci* x) {
        quitarDelRaiz(y);
        if (x->hijo == nullptr) {
            x->hijo = y;
            y->derecha = y;
            y->izquierda = y;
        } else {
            y->izquierda = x->hijo;
            y->derecha = x->hijo->derecha;
            x->hijo->derecha->izquierda = y;
            x->hijo->derecha = y;
        }
        y->padre = x;
        x->grado++;
        y->marcado = false;
    }

    void cortar(NodoFibonacci* x, NodoFibonacci* y) {
        if (x->derecha == x) {
            y->hijo = nullptr;
        } else {
            x->izquierda->derecha = x->derecha;
            x->derecha->izquierda = x->izquierda;
            if (y->hijo == x) {
                y->hijo = x->derecha;
            }
        }
        y->grado--;
        agregarAlRaiz(x);
        x->padre = nullptr;
        x->marcado = false;
    }

    void cortarCascada(NodoFibonacci* y) {
        NodoFibonacci* z = y->padre;
        if (z != nullptr) {
            if (!y->marcado) {
                y->marcado = true;
            } else {
                cortar(y, z);
                cortarCascada(z);
            }
        }
    }
};

// Implementación del algoritmo de Dijkstra utilizando una Cola de Fibonacci
vector<int> dijkstraColaFibonacci(const Grafo& grafo, int origen) {
    int n = grafo.size();
    vector<int> dist(n, INF);
    vector<int> prev(n, -1);
    dist[origen] = 0;
    ColaFibonacci pq;
    pq.insertar(origen, 0);

    while (!pq.estaVacio()) {
        auto [distU, u] = pq.extraerMinimo();
        for (const auto& arista : grafo[u]) {
            int v = arista.destino;
            int peso = arista.peso;
            if (distU + peso < dist[v]) {
                dist[v] = distU + peso;
                prev[v] = u;
                pq.decreaseKey(v, dist[v]);
            }
        }
    }

    return dist;
}

// Generación de un grafo aleatorio
Grafo generarGrafo(int numVertices, int numAristas) {
    if (numVertices <= 0 || numAristas <= 0) {
        cerr << "El número de vértices y aristas debe ser mayor que 0." << endl;
        exit(1);
    }

    Grafo grafo(numVertices);
    srand(time(0));

    // Generar árbol cobertor para asegurar conectividad
    for (int i = 1; i < numVertices; ++i) {
        int j = rand() % i;
        int peso = rand() % 100 + 1;
        grafo[i].push_back({j, peso});
        grafo[j].push_back({i, peso});
    }

    unordered_set<string> aristas;
    for (int i = 1; i < numVertices; ++i) {
        aristas.insert(to_string(min(i, i-1)) + "-" + to_string(max(i, i-1)));
    }

    // Añadir aristas adicionales hasta llegar a numAristas aristas
    int aristasAgregadas = numVertices - 1;
    while (aristasAgregadas < numAristas) {
        int u = rand() % numVertices;
        int v = rand() % numVertices;
        if (u != v) {
            string aristaStr = to_string(min(u, v)) + "-" + to_string(max(u, v));
            if (aristas.find(aristaStr) == aristas.end()) {
                int peso = rand() % 100 + 1;
                grafo[u].push_back({v, peso});
                grafo[v].push_back({u, peso});
                aristas.insert(aristaStr);
                aristasAgregadas++;
            }
        }
    }
    return grafo;
}

// Realización de experimentos
void realizarExperimento() {
    vector<int> is = {10, 12, 14};
    for (int i : is) {
        int numVertices = 1 << i;
        vector<int> js;
        if (i == 10) {
            js = {16, 17, 18};  // Para i = 10, probar solo j = 16, 17, 18
        } else {
            js = {16, 17, 18, 19, 20, 21, 22};  // Para i = 12 y i = 14, probar j = 16, 17, ... 22
        }
        for (int j : js) {
            int numAristas = 1 << j;
            double tiempoTotal = 0;
            for (int k = 0; k < 50; ++k) {
                Grafo grafo = generarGrafo(numVertices, numAristas);
                int origen = rand() % numVertices;
                clock_t inicio = clock();
                dijkstraColaFibonacci(grafo, origen);
                clock_t fin = clock();
                tiempoTotal += double(fin - inicio) / CLOCKS_PER_SEC;
            }
            cout << "i: " << i << ", j: " << j << ", Tiempo Promedio: " << tiempoTotal / 50 << " segundos" << endl;
        }
    }
}

int main() {
    realizarExperimento();
    return 0;
}
