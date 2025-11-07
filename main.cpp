#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <cctype>
#include <map>
#include <cmath>
#include <utility>
using namespace std;

// -------------------- ESTRUCTURA DE NODO --------------------
struct Nodo {
    string valor;
    Nodo* izq;
    Nodo* der;
    explicit Nodo(string v);
};

/**
 *
 * @param v
 */
Nodo::Nodo(std::string v) : valor(std::move(v)), izq(nullptr), der(nullptr) {}

// -------------------- FUNCIONES AUXILIARES --------------------

// Prioridad de operadores
int prioridad(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

// Convierte una expresión infija a postfija
string infijaAPostfija(const string& expresion) {
    stack<char> pila;
    string salida;

    for (char c : expresion) {
        if (isspace(c)) continue;

        if (isalnum(c)) {
            salida += c;
            salida += ' ';
        } else if (c == '(') {
            pila.push(c);
        } else if (c == ')') {
            while (!pila.empty() && pila.top() != '(') {
                salida += pila.top();
                salida += ' ';
                pila.pop();
            }
            pila.pop(); // Quita '('
        } else { // operador
            while (!pila.empty() && prioridad(pila.top()) >= prioridad(c)) {
                salida += pila.top();
                salida += ' ';
                pila.pop();
            }
            pila.push(c);
        }
    }

    while (!pila.empty()) {
        salida += pila.top();
        salida += ' ';
        pila.pop();
    }

    return salida;
}

// Construye el árbol de expresión a partir de una expresión postfija
Nodo* construirArbol(const string& postfija) {
    stack<Nodo*> pila;

    for (char c : postfija) {
        if (isspace(c)) continue;

        if (isalnum(c)) {
            pila.push(new Nodo(string(1, c)));
        } else { // operador
            if (pila.size() < 2) {
                cerr << "Error: expresión postfija inválida." << endl;
                return nullptr;
            }
            Nodo* nodo = new Nodo(string(1, c));
            nodo->der = pila.top(); pila.pop();
            nodo->izq = pila.top(); pila.pop();
            pila.push(nodo);
        }
    }

    return pila.empty() ? nullptr : pila.top();
}

// Solicita valores de las variables del árbol
map<string, double> obtenerValores(const Nodo* nodo) {
    map<string, double> valores;
    stack<const Nodo*> pila;
    pila.push(nodo);

    while (!pila.empty()) {
        const Nodo* actual = pila.top();
        pila.pop();

        if (actual) {
            if (actual->izq) pila.push(actual->izq);
            if (actual->der) pila.push(actual->der);

            if (isalpha(actual->valor[0]) && !valores.contains(actual->valor)) {
                cout << "Ingrese el valor de " << actual->valor << ": ";
                double v;
                cin >> v;
                valores[actual->valor] = v;
            }
        }
    }
    return valores;
}

// Evalúa el árbol de expresión
double evaluar(const Nodo* nodo, const map<string, double>& valores) {
    if (!nodo) return 0;

    if (isalnum(nodo->valor[0]) && !ispunct(nodo->valor[0])) {
        if (isalpha(nodo->valor[0]))
            return valores.at(nodo->valor);
        else
            return stod(nodo->valor);
    }

    double izq = evaluar(nodo->izq, valores);
    double der = evaluar(nodo->der, valores);

    if (nodo->valor == "+") return izq + der;
    if (nodo->valor == "-") return izq - der;
    if (nodo->valor == "*") return izq * der;
    if (nodo->valor == "/") return izq / der;
    if (nodo->valor == "^") return pow(izq, der);

    return 0;
}

// Reemplaza las variables por sus valores numéricos
Nodo* reemplazavalores(const Nodo* nodo, const map<string, double>& valores) {
    if (!nodo) return nullptr;
    Nodo* nuevo = new Nodo(nodo->valor);

    if (isalpha(nodo->valor[0])) {
        nuevo->valor = to_string(valores.at(nodo->valor));
    }

    nuevo->izq = reemplazavalores(nodo->izq, valores);
    nuevo->der = reemplazavalores(nodo->der, valores);
    return nuevo;
}
void liberarArbol(const Nodo* nodo) {
    if (!nodo) return;
    liberarArbol(nodo->izq);
    liberarArbol(nodo->der);
    delete nodo;
}

// Genera el archivo DOT recursivamente
void generarDotRec(const Nodo* nodo, ofstream& out) {
    if (!nodo) return;

    out << "  \"" << nodo << "\" [label=\"" << nodo->valor << "\"];\n";

    if (nodo->izq) {
        out << "  \"" << nodo << "\" -> \"" << nodo->izq << "\";\n";
        generarDotRec(nodo->izq, out);
    }
    if (nodo->der) {
        out << "  \"" << nodo << "\" -> \"" << nodo->der << "\";\n";
        generarDotRec(nodo->der, out);
    }
}

// Crea el archivo DOT
void crearArchivoDot(const Nodo* raiz, const string& nombreArchivo) {
    ofstream out(nombreArchivo);
    if (!out.is_open()) {
        cerr << "Error al crear el archivo " << nombreArchivo << endl;
        return;
    }

    out << "digraph ArbolExpresion {\n";
    out << "  node [shape=circle, style=filled, fillcolor=white];\n";
    out << "  rankdir=TB;\n\n";

    generarDotRec(raiz, out);

    out << "}\n";
    out.close();

    cout << "\nArchivo '" << nombreArchivo << "' generado correctamente.\n";
    cout << "Para generar la imagen ejecute:\n";
    cout << "dot -Tpng " << nombreArchivo << " -o "
         << nombreArchivo.substr(0, nombreArchivo.size() - 4) << ".png\n\n";
}

// -------------------- FUNCIÓN PRINCIPAL --------------------
int main() {
    string expresion;
    cout << "Ingrese una expresion aritmetica: ";
    getline(cin, expresion);

    string postfija = infijaAPostfija(expresion);
    cout << "Expresion en postfijo: " << postfija << endl;

    Nodo* raiz = construirArbol(postfija);
    if (!raiz) return 1;

    crearArchivoDot(raiz, "arbol.dot");

    map<string, double> valores = obtenerValores(raiz);

    double resultado = evaluar(raiz, valores);
    cout << "\nResultado de la expresion: " << resultado << endl;

    Nodo* arbolConValores = reemplazavalores(raiz, valores);
    crearArchivoDot(arbolConValores, "arbol_valores.dot");
    liberarArbol(raiz);
    liberarArbol(arbolConValores);
    return 0;
}
