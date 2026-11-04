#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>

using namespace std;

struct Maquinaria {
    int id;
    string nombre;
    int prioridad;
    Maquinaria* siguiente;
};

struct Pedido {
    int id;
    string modelo;
    int prioridad;
    Pedido* siguiente;
};

struct Componente {
    int id;
    string pieza;
    Componente* siguiente;
};

Maquinaria* listaMaquinaria = NULL;
Pedido* colaFrente = NULL;
Componente* pilaAlmacen = NULL;

int opcionGlobal;

void menuGestorMaquinaria();
void registrarNuevaMaquinaria();
void eliminarMaquinaria();
void buscarMaquinaria();
void modificarPrioridadEnsamblaje();

void menuPlanificadorProduccion();
void encolarPedidoSegunPrioridad();
void desencolarYEjecutarProduccion();
void visualizarColaProduccion();

void menuGestorAlmacen();
void asignarComponenteMemoria();
void liberarComponenteMemoria();
void verificarEstadoAlmacen();

void guardarEstadoSistema();
void cargarEstadoSistema();

int main() {
    cargarEstadoSistema();
    do {
        system("cls");
        cout << "===================================================\n";
        cout << "      SISTEMA 'LA CASA DEL AGRICULTOR' S.A.       \n";
        cout << "===================================================\n";
        cout << "1. Gestor de Maquinaria Agricola\n";
        cout << "2. Planificador de Produccion\n";
        cout << "3. Gestor de Almacen y Componentes\n";
        cout << "4. Guardar Estado del Sistema\n";
        cout << "5. Salir del Sistema\n";
        cout << "Ingrese una opcion: ";
        cin >> opcionGlobal;

        switch (opcionGlobal) {
            case 1: menuGestorMaquinaria(); break;
            case 2: menuPlanificadorProduccion(); break;
            case 3: menuGestorAlmacen(); break;
            case 4: guardarEstadoSistema(); system("pause"); break;
            case 5: guardarEstadoSistema(); cout << "Saliendo...\n"; break;
            default: cout << "Opcion no valida.\n"; system("pause");
        }
    } while (opcionGlobal != 5);
    return 0;
}

void menuGestorMaquinaria() {
    do {
        system("cls");
        cout << "--- GESTOR DE MAQUINARIA AGRICOLA ---\n";
        cout << "1. Registrar nueva maquinaria\n";
        cout << "2. Eliminar maquinaria\n";
        cout << "3. Buscar maquinaria\n";
        cout << "4. Modificar prioridad\n";
        cout << "5. Volver\n";
        cout << "Seleccione: ";
        cin >> opcionGlobal;
        switch (opcionGlobal) {
            case 1: registrarNuevaMaquinaria(); break;
            case 2: eliminarMaquinaria(); break;
            case 3: buscarMaquinaria(); break;
            case 4: modificarPrioridadEnsamblaje(); break;
        }
        if (opcionGlobal != 5) system("pause");
    } while (opcionGlobal != 5);
}

void registrarNuevaMaquinaria() {
    Maquinaria* nuevo = new Maquinaria();
    cout << "ID: "; cin >> nuevo->id;
    cout << "Nombre: "; cin >> nuevo->nombre;
    cout << "Prioridad: "; cin >> nuevo->prioridad;
    nuevo->siguiente = listaMaquinaria;
    listaMaquinaria = nuevo;
    cout << "Maquinaria registrada con exito.\n";
}

void eliminarMaquinaria() {
    int idEliminar;
    cout << "Ingrese ID a eliminar: "; cin >> idEliminar;
    if (listaMaquinaria == NULL) { cout << "Lista vacia.\n"; return; }
    
    Maquinaria* aux = listaMaquinaria;
    Maquinaria* ant = NULL;
    
    while (aux != NULL && aux->id != idEliminar) {
        ant = aux;
        aux = aux->siguiente;
    }
    
    if (aux == NULL) { cout << "No encontrado.\n"; return; }
    
    if (ant == NULL) listaMaquinaria = listaMaquinaria->siguiente;
    else ant->siguiente = aux->siguiente;
    
    delete aux;
    cout << "Maquinaria eliminada.\n";
}

void buscarMaquinaria() {
    int idBuscar;
    cout << "Ingrese ID a buscar: "; cin >> idBuscar;
    Maquinaria* aux = listaMaquinaria;
    while (aux != NULL) {
        if (aux->id == idBuscar) {
            cout << "Encontrado: " << aux->nombre << " (Prioridad " << aux->prioridad << ")\n";
            return;
        }
        aux = aux->siguiente;
    }
    cout << "No encontrado.\n";
}

void modificarPrioridadEnsamblaje() {
    int idModificar;
    cout << "Ingrese ID a modificar: "; cin >> idModificar;
    Maquinaria* aux = listaMaquinaria;
    while (aux != NULL) {
        if (aux->id == idModificar) {
            cout << "Nueva prioridad: "; cin >> aux->prioridad;
            cout << "Prioridad modificada.\n";
            return;
        }
        aux = aux->siguiente;
    }
    cout << "No encontrado.\n";
}

void menuPlanificadorProduccion() {
    do {
        system("cls");
        cout << "--- PLANIFICADOR DE PRODUCCION ---\n";
        cout << "1. Encolar pedido\n";
        cout << "2. Procesar pedido\n";
        cout << "3. Visualizar cola\n";
        cout << "4. Volver\n";
        cout << "Seleccione: ";
        cin >> opcionGlobal;
        switch (opcionGlobal) {
            case 1: encolarPedidoSegunPrioridad(); break;
            case 2: desencolarYEjecutarProduccion(); break;
            case 3: visualizarColaProduccion(); break;
        }
        if (opcionGlobal != 4) system("pause");
    } while (opcionGlobal != 4);
}

void encolarPedidoSegunPrioridad() {
    Pedido* nuevo = new Pedido();
    cout << "ID Pedido: "; cin >> nuevo->id;
    cout << "Modelo: "; cin >> nuevo->modelo;
    cout << "Prioridad (menor numero = mas urgente): "; cin >> nuevo->prioridad;
    nuevo->siguiente = NULL;

    if (colaFrente == NULL || colaFrente->prioridad > nuevo->prioridad) {
        nuevo->siguiente = colaFrente;
        colaFrente = nuevo;
    } else {
        Pedido* aux = colaFrente;
        while (aux->siguiente != NULL && aux->siguiente->prioridad <= nuevo->prioridad) {
            aux = aux->siguiente;
        }
        nuevo->siguiente = aux->siguiente;
        aux->siguiente = nuevo;
    }
    cout << "Pedido encolado por prioridad.\n";
}

void desencolarYEjecutarProduccion() {
    if (colaFrente == NULL) { cout << "No hay pedidos en cola.\n"; return; }
    Pedido* aux = colaFrente;
    colaFrente = colaFrente->siguiente;
    cout << "Procesando pedido ID: " << aux->id << " Modelo: " << aux->modelo << "\n";
    delete aux;
}

void visualizarColaProduccion() {
    if (colaFrente == NULL) { cout << "Cola vacia.\n"; return; }
    Pedido* aux = colaFrente;
    cout << "Cola de produccion actual:\n";
    while (aux != NULL) {
        cout << "ID: " << aux->id << " | Modelo: " << aux->modelo << " | Prio: " << aux->prioridad << "\n";
        aux = aux->siguiente;
    }
}

void menuGestorAlmacen() {
    do {
        system("cls");
        cout << "--- GESTOR DE ALMACEN ---\n";
        cout << "1. Asignar componente (Push)\n";
        cout << "2. Liberar componente (Pop)\n";
        cout << "3. Ver estado\n";
        cout << "4. Volver\n";
        cout << "Seleccione: ";
        cin >> opcionGlobal;
        switch (opcionGlobal) {
            case 1: asignarComponenteMemoria(); break;
            case 2: liberarComponenteMemoria(); break;
            case 3: verificarEstadoAlmacen(); break;
        }
        if (opcionGlobal != 4) system("pause");
    } while (opcionGlobal != 4);
}

void asignarComponenteMemoria() {
    Componente* nuevo = new Componente();
    cout << "ID Componente: "; cin >> nuevo->id;
    cout << "Nombre Pieza: "; cin >> nuevo->pieza;
    nuevo->siguiente = pilaAlmacen;
    pilaAlmacen = nuevo;
    cout << "Componente apilado en almacen.\n";
}

void liberarComponenteMemoria() {
    if (pilaAlmacen == NULL) { cout << "Almacen vacio.\n"; return; }
    Componente* aux = pilaAlmacen;
    pilaAlmacen = pilaAlmacen->siguiente;
    cout << "Componente liberado: " << aux->pieza << "\n";
    delete aux;
}

void verificarEstadoAlmacen() {
    if (pilaAlmacen == NULL) { cout << "Almacen vacio.\n"; return; }
    Componente* aux = pilaAlmacen;
    cout << "Estado actual del almacen (Tope a Base):\n";
    while (aux != NULL) {
        cout << "ID: " << aux->id << " | Pieza: " << aux->pieza << "\n";
        aux = aux->siguiente;
    }
}

void guardarEstadoSistema() {
    ofstream archivo("datos_sistema.txt");
    if (archivo.is_open()) {
        Maquinaria* aux = listaMaquinaria;
        while(aux != NULL){
            archivo << "M " << aux->id << " " << aux->nombre << " " << aux->prioridad << "\n";
            aux = aux->siguiente;
        }
        archivo.close();
        cout << "Datos guardados correctamente.\n";
    }
}

void cargarEstadoSistema() {
    ifstream archivo("datos_sistema.txt");
    if (archivo.is_open()) {
        char tipo; int id, prio; string nom;
        while(archivo >> tipo >> id >> nom >> prio) {
            if (tipo == 'M') {
                Maquinaria* nuevo = new Maquinaria();
                nuevo->id = id; nuevo->nombre = nom; nuevo->prioridad = prio;
                nuevo->siguiente = listaMaquinaria;
                listaMaquinaria = nuevo;
            }
        }
        archivo.close();
    }
}