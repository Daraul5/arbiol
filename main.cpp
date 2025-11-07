#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <cctype>
#include <utility>
#include <map>
using namespace std;

// Estructura para el nodo del árbol
struct Nodo {
    string valor;
    Nodo* izq;
    Nodo* der;
    explicit Nodo(string v) : valor(std::move(v)), izq(nullptr), der(nullptr) {}
};

// -------------------- FUNCIONES AUXILIARES --------------------

// Prioridad de operadores
int prioridad(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

// Convierte una expresión infija a postfija (notación polaca inversa)
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
    string token;

    for (char c : postfija) {
        if (isspace(c)) continue;

        if (isalnum(c)) {
            pila.push(new Nodo(string(1, c)));
        } else { // operador
            Nodo* nodo = new Nodo(string(1, c));
            nodo->der = pila.top(); pila.pop();
            nodo->izq = pila.top(); pila.pop();
            pila.push(nodo);
        }
    }

    return pila.top();
}
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
                cout << "Ingrese el valor de" << actual->valor << endl;
                double v;
                cin >> v;
                valores[actual->valor] = v;
            }
        }
    }
    return valores;
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

// Crea el archivo DOT completo
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

    crearArchivoDot(raiz, "arbol.dot");

    return 0;
}
