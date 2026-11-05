#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <limits>

using namespace std;

// Estructuras dinamicas autorreferenciadas
struct Proceso {
    int pid;
    string nombre;
    int prioridad;
    string estado;
    Proceso* siguiente;
};

struct NodoColaCPU {
    int pid;
    string nombre;
    int prioridad;
    int tiempoRafaga;
    NodoColaCPU* siguiente;
};

struct BloqueMemoria {
    int idBloque;
    int pidPropietario;
    string nombreProceso;
    int tamanoKB;
    BloqueMemoria* siguiente;
};

// Punteros globales (cabeza de lista, frente de cola y tope de pila)
Proceso* listaProcesos = NULL;
NodoColaCPU* colaCPUFrente = NULL;
BloqueMemoria* pilaMemoriaTope = NULL;

// Arreglo estatico para mapear los niveles de prioridad
const string NIVELES_PRIORIDAD[4] = {"N/A", "Alta", "Media", "Baja"};

// Prototipos de funciones
int leerEnteroSeguro(const string& mensaje);
int leerPrioridadValida(const string& mensaje);
string leerTextoSinEspacios(const string& mensaje);
Proceso* obtenerProcesoPorPID(int pid);
void liberarTodaLaMemoria();

void menuGestorProcesos();
void insertarProceso();
void eliminarProceso();
void buscarProcesoPorPIDoNombre();
void modificarPrioridadProceso();
void mostrarListaProcesos();

void menuPlanificadorCPU();
void encolarProcesoCPU();
void insertarOrdenadoEnColaCPU(int pid, string nombre, int prioridad, int rafaga);
void desencolarYEjecutarCPU();
void buscarProcesoEnColaCPU();
void visualizarColaCPU();

void menuGestorMemoria();
void asignarMemoriaPush();
void liberarMemoriaPop();
void buscarBloqueMemoria();
void verificarEstadoMemoria();

void guardarEstadoSistema();
void cargarEstadoSistema();

int main() {
    int opcionPrincipal;
    cargarEstadoSistema();

    do {
        system("cls");
        cout << "==============================================================\n";
        cout << "     SIMULADOR DE SISTEMA OPERATIVO - GESTION DE PROCESOS     \n";
        cout << "==============================================================\n";
        cout << "1. Gestor de Procesos (Lista Enlazada Simple)\n";
        cout << "2. Planificador de CPU (Cola de Prioridad)\n";
        cout << "3. Gestor de Memoria RAM (Pila LIFO)\n";
        cout << "4. Guardar Estado Completo del Sistema\n";
        cout << "5. Salir del Sistema\n";
        cout << "--------------------------------------------------------------\n";
        opcionPrincipal = leerEnteroSeguro("Seleccione una opcion: ");

        switch (opcionPrincipal) {
            case 1: menuGestorProcesos(); break;
            case 2: menuPlanificadorCPU(); break;
            case 3: menuGestorMemoria(); break;
            case 4: guardarEstadoSistema(); system("pause"); break;
            case 5:
                guardarEstadoSistema();
                liberarTodaLaMemoria();
                cout << "Memoria dinamica liberada. Saliendo del sistema...\n";
                break;
            default:
                cout << "Opcion no valida. Intente nuevamente.\n";
                system("pause");
        }
    } while (opcionPrincipal != 5);

    return 0;
}

// Evita bucles infinitos si el usuario ingresa caracteres en lugar de numeros
int leerEnteroSeguro(const string& mensaje) {
    int valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        } else {
            cout << "Error: Entrada invalida. Ingrese un numero entero.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

int leerPrioridadValida(const string& mensaje) {
    int prio;
    do {
        prio = leerEnteroSeguro(mensaje);
        if (prio < 1 || prio > 3) {
            cout << "Prioridad fuera de rango. Use 1 (Alta), 2 (Media) o 3 (Baja).\n";
        }
    } while (prio < 1 || prio > 3);
    return prio;
}

// Reemplaza espacios por guiones bajos para no corromper la lectura del archivo txt
string leerTextoSinEspacios(const string& mensaje) {
    string texto;
    cout << mensaje;
    getline(cin, texto);
    while (texto.empty()) {
        cout << "El campo no puede estar vacio. " << mensaje;
        getline(cin, texto);
    }
    for (size_t i = 0; i < texto.length(); i++) {
        if (texto[i] == ' ') texto[i] = '_';
    }
    return texto;
}

Proceso* obtenerProcesoPorPID(int pid) {
    Proceso* aux = listaProcesos;
    while (aux != NULL) {
        if (aux->pid == pid) return aux;
        aux = aux->siguiente;
    }
    return NULL;
}

void menuGestorProcesos() {
    int opcion;
    do {
        system("cls");
        cout << "--- GESTOR DE PROCESOS (LISTA ENLAZADA) ---\n";
        cout << "1. Insertar nuevo proceso\n";
        cout << "2. Eliminar proceso por PID\n";
        cout << "3. Buscar proceso por PID o Nombre\n";
        cout << "4. Modificar prioridad de un proceso\n";
        cout << "5. Mostrar todos los procesos registrados\n";
        cout << "6. Volver\n";
        opcion = leerEnteroSeguro("Seleccione: ");

        switch (opcion) {
            case 1: insertarProceso(); break;
            case 2: eliminarProceso(); break;
            case 3: buscarProcesoPorPIDoNombre(); break;
            case 4: modificarPrioridadProceso(); break;
            case 5: mostrarListaProcesos(); break;
            case 6: break;
            default: cout << "Opcion no valida.\n"; break;
        }
        if (opcion != 6) system("pause");
    } while (opcion != 6);
}

// Insercion al inicio de la lista enlazada en O(1)
void insertarProceso() {
    int pid = leerEnteroSeguro("Ingrese PID del nuevo proceso: ");
    if (obtenerProcesoPorPID(pid) != NULL) {
        cout << "Error: Ya existe un proceso con el PID " << pid << ".\n";
        return;
    }

    Proceso* nuevo = new Proceso();
    nuevo->pid = pid;
    nuevo->nombre = leerTextoSinEspacios("Nombre del proceso: ");
    nuevo->prioridad = leerPrioridadValida("Prioridad (1=Alta, 2=Media, 3=Baja): ");
    nuevo->estado = "Nuevo";

    nuevo->siguiente = listaProcesos;
    listaProcesos = nuevo;

    cout << "Proceso registrado con exito.\n";
}

// Eliminacion reconectando el puntero anterior y liberando memoria con delete
void eliminarProceso() {
    if (listaProcesos == NULL) {
        cout << "La lista de procesos esta vacia.\n";
        return;
    }

    int pidEliminar = leerEnteroSeguro("Ingrese PID a eliminar: ");
    Proceso* aux = listaProcesos;
    Proceso* ant = NULL;

    while (aux != NULL && aux->pid != pidEliminar) {
        ant = aux;
        aux = aux->siguiente;
    }

    if (aux == NULL) {
        cout << "Proceso no encontrado.\n";
        return;
    }

    if (ant == NULL) listaProcesos = listaProcesos->siguiente;
    else ant->siguiente = aux->siguiente;

    delete aux;
    cout << "Proceso eliminado.\n";
}

void buscarProcesoPorPIDoNombre() {
    if (listaProcesos == NULL) {
        cout << "La lista de procesos esta vacia.\n";
        return;
    }

    cout << "1. Buscar por PID\n2. Buscar por Nombre\n";
    int criterio = leerEnteroSeguro("Seleccione criterio: ");

    Proceso* aux = listaProcesos;
    bool encontrado = false;

    if (criterio == 1) {
        int pidBuscar = leerEnteroSeguro("Ingrese PID a buscar: ");
        while (aux != NULL) {
            if (aux->pid == pidBuscar) {
                cout << "Encontrado -> PID: " << aux->pid << " | Nombre: " << aux->nombre
                     << " | Prioridad: " << aux->prioridad << " (" << NIVELES_PRIORIDAD[aux->prioridad]
                     << ") | Estado: " << aux->estado << "\n";
                encontrado = true;
                break;
            }
            aux = aux->siguiente;
        }
    } else if (criterio == 2) {
        string nomBuscar = leerTextoSinEspacios("Ingrese Nombre a buscar: ");
        while (aux != NULL) {
            if (aux->nombre == nomBuscar) {
                cout << "Encontrado -> PID: " << aux->pid << " | Nombre: " << aux->nombre
                     << " | Prioridad: " << aux->prioridad << " (" << NIVELES_PRIORIDAD[aux->prioridad]
                     << ") | Estado: " << aux->estado << "\n";
                encontrado = true;
            }
            aux = aux->siguiente;
        }
    } else {
        cout << "Criterio no valido.\n";
        return;
    }

    if (!encontrado) cout << "Proceso no encontrado.\n";
}

// Modifica la prioridad en la lista y reubica el nodo si ya estaba en la cola de CPU
void modificarPrioridadProceso() {
    if (listaProcesos == NULL) {
        cout << "La lista de procesos esta vacia.\n";
        return;
    }

    int pidMod = leerEnteroSeguro("Ingrese PID a modificar: ");
    Proceso* proc = obtenerProcesoPorPID(pidMod);

    if (proc == NULL) {
        cout << "Proceso no encontrado.\n";
        return;
    }

    int nuevaPrio = leerPrioridadValida("Nueva prioridad (1=Alta, 2=Media, 3=Baja): ");
    proc->prioridad = nuevaPrio;
    cout << "Prioridad actualizada en la lista de procesos.\n";

    NodoColaCPU* aux = colaCPUFrente;
    NodoColaCPU* ant = NULL;
    while (aux != NULL && aux->pid != pidMod) {
        ant = aux;
        aux = aux->siguiente;
    }
    if (aux != NULL) {
        int rafagaGuardada = aux->tiempoRafaga;
        if (ant == NULL) colaCPUFrente = colaCPUFrente->siguiente;
        else ant->siguiente = aux->siguiente;
        delete aux;

        insertarOrdenadoEnColaCPU(proc->pid, proc->nombre, nuevaPrio, rafagaGuardada);
        cout << "Posicion actualizada en la cola de prioridad de CPU.\n";
    }
}

void mostrarListaProcesos() {
    if (listaProcesos == NULL) {
        cout << "Lista de procesos vacia.\n";
        return;
    }
    cout << "Procesos registrados:\n";
    Proceso* aux = listaProcesos;
    while (aux != NULL) {
        cout << "PID: " << aux->pid << " | Nombre: " << aux->nombre
             << " | Prio: " << aux->prioridad << " (" << NIVELES_PRIORIDAD[aux->prioridad]
             << ") | Estado: " << aux->estado << "\n";
        aux = aux->siguiente;
    }
}

void menuPlanificadorCPU() {
    int opcion;
    do {
        system("cls");
        cout << "--- PLANIFICADOR DE CPU (COLA DE PRIORIDAD) ---\n";
        cout << "1. Encolar proceso segun prioridad\n";
        cout << "2. Desencolar y ejecutar proceso\n";
        cout << "3. Buscar proceso en la cola\n";
        cout << "4. Visualizar cola actual\n";
        cout << "5. Volver\n";
        opcion = leerEnteroSeguro("Seleccione: ");

        switch (opcion) {
            case 1: encolarProcesoCPU(); break;
            case 2: desencolarYEjecutarCPU(); break;
            case 3: buscarProcesoEnColaCPU(); break;
            case 4: visualizarColaCPU(); break;
            case 5: break;
            default: cout << "Opcion no valida.\n"; break;
        }
        if (opcion != 5) system("pause");
    } while (opcion != 5);
}

// Insercion ordenada por prioridad (menor valor = mas urgente, empates en orden FIFO)
void insertarOrdenadoEnColaCPU(int pid, string nombre, int prioridad, int rafaga) {
    NodoColaCPU* nuevo = new NodoColaCPU();
    nuevo->pid = pid;
    nuevo->nombre = nombre;
    nuevo->prioridad = prioridad;
    nuevo->tiempoRafaga = rafaga;
    nuevo->siguiente = NULL;

    if (colaCPUFrente == NULL || colaCPUFrente->prioridad > nuevo->prioridad) {
        nuevo->siguiente = colaCPUFrente;
        colaCPUFrente = nuevo;
    } else {
        NodoColaCPU* aux = colaCPUFrente;
        while (aux->siguiente != NULL && aux->siguiente->prioridad <= nuevo->prioridad) {
            aux = aux->siguiente;
        }
        nuevo->siguiente = aux->siguiente;
        aux->siguiente = nuevo;
    }
}

void encolarProcesoCPU() {
    if (listaProcesos == NULL) {
        cout << "Primero debe registrar procesos en el Gestor de Procesos.\n";
        return;
    }

    int pid = leerEnteroSeguro("Ingrese PID del proceso a encolar: ");
    Proceso* proc = obtenerProcesoPorPID(pid);

    if (proc == NULL) {
        cout << "No existe un proceso registrado con ese PID.\n";
        return;
    }

    int rafaga = leerEnteroSeguro("Tiempo de rafaga de CPU requerido (ms): ");
    insertarOrdenadoEnColaCPU(proc->pid, proc->nombre, proc->prioridad, rafaga);
    proc->estado = "Listo";

    cout << "Proceso encolado por prioridad.\n";
}

// Extraccion del frente de la cola en O(1)
void desencolarYEjecutarCPU() {
    if (colaCPUFrente == NULL) {
        cout << "No hay procesos en la cola de CPU.\n";
        return;
    }

    NodoColaCPU* aux = colaCPUFrente;
    colaCPUFrente = colaCPUFrente->siguiente;

    cout << "Ejecutando proceso -> PID: " << aux->pid << " | Nombre: " << aux->nombre
         << " | Prioridad: " << aux->prioridad << " | Rafaga: " << aux->tiempoRafaga << " ms\n";

    Proceso* proc = obtenerProcesoPorPID(aux->pid);
    if (proc != NULL) proc->estado = "Terminado";

    delete aux;
}

void buscarProcesoEnColaCPU() {
    if (colaCPUFrente == NULL) {
        cout << "Cola de CPU vacia.\n";
        return;
    }

    int pidBuscar = leerEnteroSeguro("Ingrese PID a buscar en la cola: ");
    NodoColaCPU* aux = colaCPUFrente;
    int posicion = 1;
    bool encontrado = false;

    while (aux != NULL) {
        if (aux->pid == pidBuscar) {
            cout << "Encontrado en posicion #" << posicion << " -> PID: " << aux->pid
                 << " | Nombre: " << aux->nombre << " | Prio: " << aux->prioridad
                 << " | Rafaga: " << aux->tiempoRafaga << " ms\n";
            encontrado = true;
        }
        aux = aux->siguiente;
        posicion++;
    }

    if (!encontrado) cout << "Proceso no encontrado en la cola.\n";
}

void visualizarColaCPU() {
    if (colaCPUFrente == NULL) {
        cout << "Cola de CPU vacia.\n";
        return;
    }

    cout << "Cola de planificacion de CPU (Frente a Final):\n";
    NodoColaCPU* aux = colaCPUFrente;
    while (aux != NULL) {
        cout << "PID: " << aux->pid << " | Nombre: " << aux->nombre
             << " | Prio: " << aux->prioridad << " (" << NIVELES_PRIORIDAD[aux->prioridad]
             << ") | Rafaga: " << aux->tiempoRafaga << " ms\n";
        aux = aux->siguiente;
    }
}

void menuGestorMemoria() {
    int opcion;
    do {
        system("cls");
        cout << "--- GESTOR DE MEMORIA RAM (PILA LIFO) ---\n";
        cout << "1. Asignar bloque de memoria (Push)\n";
        cout << "2. Liberar bloque de memoria (Pop)\n";
        cout << "3. Buscar bloques asignados por PID\n";
        cout << "4. Verificar estado de la memoria\n";
        cout << "5. Volver\n";
        opcion = leerEnteroSeguro("Seleccione: ");

        switch (opcion) {
            case 1: asignarMemoriaPush(); break;
            case 2: liberarMemoriaPop(); break;
            case 3: buscarBloqueMemoria(); break;
            case 4: verificarEstadoMemoria(); break;
            case 5: break;
            default: cout << "Opcion no valida.\n"; break;
        }
        if (opcion != 5) system("pause");
    } while (opcion != 5);
}

// Operacion Push sobre el tope de la pila en O(1)
void asignarMemoriaPush() {
    if (listaProcesos == NULL) {
        cout << "Primero debe registrar procesos en el Gestor de Procesos.\n";
        return;
    }

    int pid = leerEnteroSeguro("Ingrese PID del proceso que solicita memoria: ");
    Proceso* proc = obtenerProcesoPorPID(pid);
    if (proc == NULL) {
        cout << "No existe un proceso registrado con ese PID.\n";
        return;
    }

    BloqueMemoria* nuevo = new BloqueMemoria();
    nuevo->idBloque = leerEnteroSeguro("ID del bloque de memoria: ");
    nuevo->pidPropietario = proc->pid;
    nuevo->nombreProceso = proc->nombre;
    nuevo->tamanoKB = leerEnteroSeguro("Tamano del bloque (KB): ");

    nuevo->siguiente = pilaMemoriaTope;
    pilaMemoriaTope = nuevo;

    cout << "Bloque de memoria apilado correctamente.\n";
}

// Operacion Pop retirando el tope de la pila en O(1)
void liberarMemoriaPop() {
    if (pilaMemoriaTope == NULL) {
        cout << "Memoria vacia. No hay bloques para liberar.\n";
        return;
    }

    BloqueMemoria* aux = pilaMemoriaTope;
    pilaMemoriaTope = pilaMemoriaTope->siguiente;

    cout << "Bloque liberado -> ID: " << aux->idBloque << " | Proceso: " << aux->nombreProceso
         << " | Tamano: " << aux->tamanoKB << " KB\n";
    delete aux;
}

void buscarBloqueMemoria() {
    if (pilaMemoriaTope == NULL) {
        cout << "Memoria vacia.\n";
        return;
    }

    int pidBuscar = leerEnteroSeguro("Ingrese PID del proceso propietario: ");
    BloqueMemoria* aux = pilaMemoriaTope;
    bool encontrado = false;

    while (aux != NULL) {
        if (aux->pidPropietario == pidBuscar) {
            cout << "Bloque ID: " << aux->idBloque << " | Proceso: " << aux->nombreProceso
                 << " | Tamano: " << aux->tamanoKB << " KB\n";
            encontrado = true;
        }
        aux = aux->siguiente;
    }

    if (!encontrado) cout << "El proceso no tiene bloques asignados en memoria.\n";
}

void verificarEstadoMemoria() {
    if (pilaMemoriaTope == NULL) {
        cout << "Memoria vacia.\n";
        return;
    }

    cout << "Estado actual de la memoria (Tope a Base):\n";
    BloqueMemoria* aux = pilaMemoriaTope;
    while (aux != NULL) {
        cout << "Bloque ID: " << aux->idBloque << " | PID: " << aux->pidPropietario
             << " (" << aux->nombreProceso << ") | Tamano: " << aux->tamanoKB << " KB\n";
        aux = aux->siguiente;
    }
}

// Guarda las tres estructuras en archivo de texto usando prefijos P, C y M
void guardarEstadoSistema() {
    ofstream archivo("datos_sistema.txt");
    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo para guardar.\n";
        return;
    }

    Proceso* auxP = listaProcesos;
    while (auxP != NULL) {
        archivo << "P " << auxP->pid << " " << auxP->nombre << " "
                << auxP->prioridad << " " << auxP->estado << "\n";
        auxP = auxP->siguiente;
    }

    NodoColaCPU* auxC = colaCPUFrente;
    while (auxC != NULL) {
        archivo << "C " << auxC->pid << " " << auxC->nombre << " "
                << auxC->prioridad << " " << auxC->tiempoRafaga << "\n";
        auxC = auxC->siguiente;
    }

    BloqueMemoria* auxM = pilaMemoriaTope;
    while (auxM != NULL) {
        archivo << "M " << auxM->idBloque << " " << auxM->pidPropietario << " "
                << auxM->nombreProceso << " " << auxM->tamanoKB << "\n";
        auxM = auxM->siguiente;
    }

    archivo.close();
    cout << "Datos guardados correctamente en datos_sistema.txt.\n";
}

// Reconstruye las tres estructuras conservando el orden original mediante punteros al ultimo nodo
void cargarEstadoSistema() {
    ifstream archivo("datos_sistema.txt");
    if (!archivo.is_open()) return;

    char tipo;
    Proceso* ultimoProceso = NULL;
    BloqueMemoria* ultimoBloque = NULL;

    while (archivo >> tipo) {
        if (tipo == 'P') {
            Proceso* nuevo = new Proceso();
            archivo >> nuevo->pid >> nuevo->nombre >> nuevo->prioridad >> nuevo->estado;
            nuevo->siguiente = NULL;
            if (listaProcesos == NULL) {
                listaProcesos = nuevo;
                ultimoProceso = nuevo;
            } else {
                ultimoProceso->siguiente = nuevo;
                ultimoProceso = nuevo;
            }
        } else if (tipo == 'C') {
            int pid, prio, rafaga;
            string nom;
            archivo >> pid >> nom >> prio >> rafaga;
            insertarOrdenadoEnColaCPU(pid, nom, prio, rafaga);
        } else if (tipo == 'M') {
            BloqueMemoria* nuevo = new BloqueMemoria();
            archivo >> nuevo->idBloque >> nuevo->pidPropietario >> nuevo->nombreProceso >> nuevo->tamanoKB;
            nuevo->siguiente = NULL;
            if (pilaMemoriaTope == NULL) {
                pilaMemoriaTope = nuevo;
                ultimoBloque = nuevo;
            } else {
                ultimoBloque->siguiente = nuevo;
                ultimoBloque = nuevo;
            }
        }
    }
    archivo.close();
}

// Libera todos los nodos remanentes en memoria antes de cerrar el programa
void liberarTodaLaMemoria() {
    while (listaProcesos != NULL) {
        Proceso* aux = listaProcesos;
        listaProcesos = listaProcesos->siguiente;
        delete aux;
    }
    while (colaCPUFrente != NULL) {
        NodoColaCPU* aux = colaCPUFrente;
        colaCPUFrente = colaCPUFrente->siguiente;
        delete aux;
    }
    while (pilaMemoriaTope != NULL) {
        BloqueMemoria* aux = pilaMemoriaTope;
        pilaMemoriaTope = pilaMemoriaTope->siguiente;
        delete aux;
    }
}