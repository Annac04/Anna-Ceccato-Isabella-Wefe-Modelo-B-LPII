#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <limits>
using namespace std;

const int NIVELES = 5;
struct Pos { int x, y; };
vector<string> ganadores;
string nombreJugador; 

#define ROJO     "\x1b[31m"
#define BLANCO   "\x1b[37m"
#define VERDE    "\x1b[32m"
#define AMARILLO "\x1b[33m"
#define NARANJA  "\x1b[38;5;208m"
#define RESET    "\x1b[0m"


void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
void dormir(int ms) { this_thread::sleep_for(chrono::milliseconds(ms)); }
void imprimirCentrado(const string& texto, int ancho = 60) {
    int espacios = (ancho - (int)texto.size()) / 2;
    if (espacios < 0) espacios = 0;
    cout << string(espacios, ' ') << texto << "\n";
}
bool colision(const Pos& a, const Pos& b) { return (a.x == b.x && a.y == b.y); }

void mostrarMenu() {
    limpiarPantalla();
    cout << "=====================================\n";
    cout << "   JUEGO DE LABERINTO TIPO PACMAN   \n";
    cout << "=====================================\n\n";
    cout << "Instrucciones:\n";
    cout << " - Usa las teclas W (arriba), A (izquierda), S (abajo), D (derecha) y enter despues de tocar cada tecla\n";
    cout << " - Tu personaje es '@'\n";
    cout << " - El fantasma es 'F' y te persigue siempre, asi que ten cuidado de que te atrape\n";
    cout << " - La meta es 'X'\n";
    cout << " - Si el fantasma te atrapa o chocas contra un muro, pierdes\n";
    cout << " - Cada nivel es más grande y el fantasma se vuelve más rápido\n\n";
    cout << "=====================================\n";
    cout << "Ingrese su nombre: ";
    getline(cin, nombreJugador);
    if (nombreJugador.empty()) nombreJugador = "Jugador";
}

void mostrarLose() {
    limpiarPantalla();
    imprimirCentrado("YOU LOSE!");
    dormir(2000);
}
void mostrarWin() {
    limpiarPantalla();
    imprimirCentrado("YOU WIN!");
    dormir(2000);
}
void mostrarNivel(int n) {
    limpiarPantalla();
    imprimirCentrado("Nivel " + to_string(n) + "#");
    dormir(1500);
}

string colorFantasmaAleatorio() {
    vector<string> colores = {ROJO, BLANCO, VERDE, AMARILLO, NARANJA};
    int idx = rand() % colores.size();
    return colores[idx];
}

void mostrarLaberinto(int tam, Pos jugador, Pos fantasma, string colorFantasma, vector<vector<char>>& lab) {
    for (int y=0; y<tam; y++) {
        for (int x=0; x<tam; x++) {
            if (x==jugador.x && y==jugador.y) cout << "@";
            else if (x==fantasma.x && y==fantasma.y) cout << colorFantasma << "F" << RESET;
            else if (x==tam-2 && y==tam-2) cout << "X"; // meta
            else cout << lab[y][x];
        }
        cout << "\n";
    }
}

vector<vector<char>> generarLaberinto(int tam, int nivel) {
    vector<vector<char>> lab(tam, vector<char>(tam,' '));
    for (int y=0; y<tam; y++) {
        for (int x=0; x<tam; x++) {
            if (y==0 || x==0 || y==tam-1 || x==tam-1) lab[y][x] = '#';
        }
    }
    for (int y=2; y<tam-2; y+=nivel+1) {
        for (int x=2; x<tam-2; x+=nivel+1) {
            if (rand()%2==0 && !(x==tam-2 && y==tam-2)) {
                lab[y][x] = '#';
            }
        }
    }
    return lab;
}

bool jugarNivel(int nivel) {
    int tam = 7 + 2*nivel; 
    Pos jugador{1,1};
    Pos fantasma{tam-2,1};

    mostrarNivel(nivel);

    int turno = 0;
    int velocidadFantasma = max(1, 4 - nivel); 
    int pausaJugador = 300 - nivel*40; 

    string colorFantasma = colorFantasmaAleatorio();
    vector<vector<char>> lab = generarLaberinto(tam, nivel);

    while (true) {
        limpiarPantalla();
        cout << "Jugador: " << nombreJugador << " | Nivel " << nivel << "\n";
        mostrarLaberinto(tam, jugador, fantasma, colorFantasma, lab);

        char mov;
        cin >> mov;
        if (mov=='w' && lab[jugador.y-1][jugador.x]!='#') jugador.y--;
        if (mov=='s' && lab[jugador.y+1][jugador.x]!='#') jugador.y++;
        if (mov=='a' && lab[jugador.y][jugador.x-1]!='#') jugador.x--;
        if (mov=='d' && lab[jugador.y][jugador.x+1]!='#') jugador.x++;

        turno++;
        if (turno % velocidadFantasma == 0) {
            if (fantasma.x < jugador.x && lab[fantasma.y][fantasma.x+1]!='#') fantasma.x++;
            else if (fantasma.x > jugador.x && lab[fantasma.y][fantasma.x-1]!='#') fantasma.x--;
            if (fantasma.y < jugador.y && lab[fantasma.y+1][fantasma.x]!='#') fantasma.y++;
            else if (fantasma.y > jugador.y && lab[fantasma.y-1][fantasma.x]!='#') fantasma.y--;
        }

        if (jugador.x==tam-2 && jugador.y==tam-2) return true;
        if (colision(jugador,fantasma)) return false;

        dormir(pausaJugador);
    }
}

void ordenarGanadores(vector<string>& v) {
  
    for (int i=0; i<v.size(); i++)
        for (int j=i+1; j<v.size(); j++)
            if (v[j] < v[i]) swap(v[i], v[j]);
    
    for (int i=0; i<v.size(); i++) {
        int max=i;
        for (int j=i+1; j<v.size(); j++)
            if (v[j].size() > v[max].size()) max=j;
        swap(v[i], v[max]);
    }
}


int main() {
    srand(time(0));

    mostrarMenu();

    for (int n=1; n<=NIVELES; n++) {
        if (!jugarNivel(n)) {
            mostrarLose();
            cout << "Perdiste en el laberinto numero " << n << "\n";
            return 0;
        }
    }
    mostrarWin();
    cout << "You win!" << nombreJugador << endl;

    ganadores.push_back(nombreJugador);
    ordenarGanadores(ganadores);
    cout << "\nGanadores:\n";
    for (auto& g: ganadores) cout << g << "\n";

    return 0;
}

//no nos corre en dev c++ pero si en otro compilador online.
