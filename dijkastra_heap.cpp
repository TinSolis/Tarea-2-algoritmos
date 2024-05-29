#include <iostream>
#include <vector>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <unordered_set>
#include <chrono>
#include <numeric>
#include <cmath>

using namespace std;

const int INF = numeric_limits<int>::max();

struct Arista {
    int destino;
    int peso;
};

typedef vector<vector<Arista>> Grafo;

class HeapBinario {
public:
    HeapBinario(int n) {
        data.reserve(n);
    }

    bool estaVacio() const {
        return data.empty();
    }

    void insertar(int distancia, int nodo) {
        data.push_back({distancia, nodo});
        disminuirClave(data.size() - 1);
    }

    pair<int, int> extraerMinimo() {
        if (data.empty()) {
            return {INF, -1};
        }
        pair<int, int> minElement = data[0];
        data[0] = data.back();
        data.pop_back();
        heapificar(0);
        return minElement;
    }

    void disminuirClave(int i) {
        while (i > 0 && data[padre(i)].first > data[i].first) {
            swap(data[i], data[padre(i)]);
            i = padre(i);
        }
    }

private:
    vector<pair<int, int>> data;

    int padre(int i) const {
        return (i - 1) / 2;
    }

    int izquierdo(int i) const {
        return 2 * i + 1;
    }

    int derecho(int i) const {
        return 2 * i + 2;
    }

    void heapificar(int i) {
        int l = izquierdo(i);
        int r = derecho(i);
        int menor = i;
        if (l < data.size() && data[l].first < data[menor].first) {
            menor = l;
        }
        if (r < data.size() && data[r].first < data[menor].first) {
            menor = r;
        }
        if (menor != i) {
            swap(data[i], data[menor]);
            heapificar(menor);
        }
    }
};

vector<int> dijkstraHeap(const Grafo& grafo, int origen) {
    int n = grafo.size();
    vector<int> dist(n, INF);
    dist[origen] = 0;
    HeapBinario pq(n);
    pq.insertar(0, origen);

    while (!pq.estaVacio()) {
        auto [distU, u] = pq.extraerMinimo();
        for (const auto& arista : grafo[u]) {
            int v = arista.destino;
            int peso = arista.peso;
            if (distU + peso < dist[v]) {
                dist[v] = distU + peso;
                pq.insertar(dist[v], v);
            }
        }
    }

    return dist;
}

Grafo generarMultigrafo(int numVertices, int numAristas) {
    if (numVertices <= 0 || numAristas <= 0) {
        cerr << "El número de vértices y aristas debe ser mayor que 0." << endl;
        exit(1);
    }

    Grafo grafo(numVertices);
    srand(time(0));

    for (int i = 1; i < numVertices; ++i) {
        int j = rand() % i;
        int peso = rand() % 100 + 1;
        grafo[i].push_back({j, peso});
        grafo[j].push_back({i, peso});
    }

    int aristasAgregadas = numVertices - 1;
    while (aristasAgregadas < numAristas) {
        int u = rand() % numVertices;
        int v = rand() % numVertices;
        if (u != v) {
            int peso = rand() % 100 + 1;
            grafo[u].push_back({v, peso});
            grafo[v].push_back({u, peso});
            aristasAgregadas++;
        }
    }
    return grafo;
}

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
            vector<double> tiemposConsulta;

            for (int k = 0; k < 50; ++k) {
                Grafo grafo = generarMultigrafo(numVertices, numAristas);
                int origen = rand() % numVertices;

                auto start = std::chrono::high_resolution_clock::now();
                dijkstraHeap(grafo, origen);
                auto end = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double> tiempoConsulta = end - start;
                tiemposConsulta.push_back(tiempoConsulta.count());
            }

            double tiempoTotalConsultas = std::accumulate(tiemposConsulta.begin(), tiemposConsulta.end(), 0.0);
            double tiempoPromedio = tiempoTotalConsultas / tiemposConsulta.size();
            
            cout << "i: " << i << ", j: " << j << ", Tiempo Total: " << tiempoTotalConsultas << " segundos" << endl;
            cout << "i: " << i << ", j: " << j << ", Tiempo Promedio: " << tiempoPromedio << " segundos" << endl;
        }
    }
}

int main() {
    realizarExperimento();
    return 0;
}
