#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ARRIBA 0
#define ABAJO 1
#define IZQUIERDA 2
#define DERECHA 3

typedef struct
{
    char nombre[50];
    int valor;
    int peso;
} Item;

//La estructura, en cada nodo representa un escenario.
typedef struct NodoGrafo NodoGrafo;

struct NodoGrafo
{
    int id;
    char nombre[50];
    char descripcion[300];
    Item* items;
    int numItems;
    NodoGrafo* conexiones[4];
    int id_conexiones[4];
    int esFinal;
    NodoGrafo* siguiente;
};

typedef struct
{
    NodoGrafo* inicio;
    NodoGrafo* fin;
    NodoGrafo* lista;
} Grafo;

typedef struct
{
    Item* items;
    int cantidad;
    int peso_total;
    int valor_total;
} Inventario;

//Compara dos enteros y retorna 1 si key 1 es menor que la key2 o 0 en el caso contrario
int lower_than_int(int key1, int key2){
    return key1 < key2;
}

/*Busca el nodo por el id especifico, recorriendo la lista del grafo,
devuleve el puntero al nodo si lo encuentra, si no lo encuentra, retorna NULL*/
NodoGrafo* buscarNodo(Grafo* grafo, int id)
{
    NodoGrafo* actual = grafo->lista;
    while (actual) {
        if (actual->id == id) return actual;
        actual = actual->siguiente;
    }
    return NULL;
}

//Inserta un nodo en la lista del grafo en orden ascendente
void insertarNodo(Grafo* grafo, NodoGrafo* nodo)
{
    if (!grafo->lista || nodo->id < grafo->lista->id)
    {
        nodo->siguiente = grafo->lista;
        grafo->lista = nodo;
        return;
    }
    NodoGrafo* actual = grafo->lista;
    while (actual->siguiente && actual->siguiente->id < nodo->id)
    {
        actual = actual->siguiente;
    }
    nodo->siguiente = actual->siguiente;
    actual->siguiente = nodo;
}

//elimina espacios en blanco al inicio o al final de la cadena
char* trim(char* str){
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

/*Divide una cadena en partes, usando un delimitador y devolviendo un arreglo 
de cadenas, igualmente elimina los espacios innecesarios .*/
char** split_string(const char* str, const char* delim, int* out_count)
{
    char* input = strdup(str);
    char* token;
    char** result = NULL;
    int count = 0;
    
    token = strtok(input, delim);
    while(token != NULL){
        result = realloc(result, sizeof(char*) * (count + 1));
        result[count] = strdup(trim(token));
        count++;
        token = strtok(NULL, delim);
    }
    
    *out_count = count;
    free(input);
    return result;
}

//Recorre y limpia cada cadena del arreglo, y luego el arreglo en si
void free_split(char** arr, int count){
    for (int i = 0; i < count; i++) free(arr[i]);
    free(arr);
}

//Devuelve la linea leida sin espacios al inicio y al final, usando trim
char* leer_linea(FILE* f){
    static char buffer[1024];
    if (!fgets(buffer, sizeof(buffer), f)) return NULL;
    return trim(buffer);
}

/*-----------------------------------------------------------------------------------------*/

//funcion que abre el archivo, con los escenarios necesarios para el juego
Grafo* leer_escenarios(const char* nombreArchivo)
{
    //Abre el archivo, si no logra abrirlo, muestra un mensaje
    FILE* f = fopen(nombreArchivo, "r");
    if (!f)
    {
        printf("No se pudo abrir archivo\n");
        return NULL;
    }

    //reserva memoria para el grafo y inicializa sus punteros en NULL
    Grafo* grafo = malloc(sizeof(Grafo));
    grafo->inicio = NULL;
    grafo->fin = NULL;
    grafo->lista = NULL;

    //Ayuda a verificar si el archivo esta vacio, si ocurre, cierra y libera memoria
    char* linea = leer_linea(f);
    if (!linea)
    {
        fclose(f);
        free(grafo);
        return NULL;
    }

    //recorre todas las lienas, desde la segunda linea primero
    while ((linea = leer_linea(f)) != NULL)
    {
        //variables usadas para separar lineas en 9 campos
        char* ptr = linea;
        char* campos[9];
        int campo_actual = 0;
        int dentro_comillas = 0;
        char* inicio_campo = ptr;

        //separa manualmente los campos del csv, manejando las comillas
        while (*ptr && campo_actual < 9)
        {
            if (*ptr == '"')
            {
                dentro_comillas = !dentro_comillas;
            }
            else if (*ptr == ',' && !dentro_comillas)
            {
                *ptr = '\0';
                campos[campo_actual++] = trim(inicio_campo);
                inicio_campo = ptr + 1;
            }
            ptr++;
        }
        if (campo_actual < 9)
        {
            campos[campo_actual] = trim(inicio_campo);
        }

        //crea un nuevo nodo, y asigna su id, nombre, descripcion y los items.
        NodoGrafo* nodo = malloc(sizeof(NodoGrafo));
        nodo->id = atoi(campos[0]);
        strncpy(nodo->nombre, campos[1], 49);
        nodo->nombre[49] = '\0';
        strncpy(nodo->descripcion, campos[2], 299);
        nodo->descripcion[299] = '\0';
        nodo->numItems = 0;
        nodo->items = NULL;

        //almacena los id de los nodos vecinos y marca si es nodo final
        for (int i = 0; i < 4; i++)
        {
            nodo->id_conexiones[i] = atoi(campos[4 + i]);
            nodo->conexiones[i] = NULL;
        }
        nodo->esFinal = (strcasecmp(campos[8], "Si") == 0) ? 1 : 0;
        nodo->siguiente = NULL;

        //separa el cambio se item, si se cumple la condicion de que aun vean
        if (strlen(campos[3]) > 0 && strcmp(campos[3], " ") != 0)
        {
            int items_count = 0;
            char** items = split_string(campos[3], ";", &items_count);
            nodo->numItems = items_count;
            nodo->items = malloc(sizeof(Item) * items_count);

            //separa cada item, en nombre valor y peso respectivamente y luego libera memoria de los auxiliares
            for (int i = 0; i < items_count; i++)
            {
                int partes_count = 0;
                char** partes = split_string(items[i], ",", &partes_count);
                if (partes_count == 3)
                {
                    strncpy(nodo->items[i].nombre, partes[0], 49);
                    nodo->items[i].nombre[49] = '\0';
                    nodo->items[i].peso = atoi(partes[2]);
                    nodo->items[i].valor = atoi(partes[1]);
                }
                free_split(partes, partes_count);
            }
            free_split(items, items_count);
        }
        //inserta el nuevo nodo en la lista ordenada del grafo
        insertarNodo(grafo, nodo);
    }
    //cierra el arhivo
    fclose(f);

    //remplaza los id, por punteros a nodos ya cargados
    NodoGrafo* actual = grafo->lista;
    while (actual)
    {
        for (int i = 0; i < 4; i++)
        {
            if (actual->id_conexiones[i] > 0)
            {
                NodoGrafo* dest = buscarNodo(grafo, actual->id_conexiones[i]);
                if (dest) actual->conexiones[i] = dest;
            }
        }
        actual = actual->siguiente;
    }

    //identifica el nodo inicio y final de la lista del grafo
    actual = grafo->lista;
    while (actual)
    {
        if (strcmp(actual->nombre, "Entrada principal") == 0)
            grafo->inicio = actual;
        if (actual->esFinal)
            grafo->fin = actual;
        actual = actual->siguiente;
    }

    return grafo;
}

/*--- SISTEMA DE MENÚ (5 OPCIONES) ---*/
//muestra el menu y el estado actual de cada escenario
void mostrar_menu(NodoGrafo* actual, Inventario* inv, float* tiempo)
{
    //muesta nuestro tiempo y la descripcion del escenario y nombre del escenario
    printf("\n=== %s (Tiempo: %.2f) ===\n", actual->nombre, *tiempo);
    printf("Descripcion: %s\n", actual->descripcion);
    
    //muestra los item disponibles. con su valor y peso
    if (actual->numItems > 0)
    {
        printf("\nItems disponibles:\n");
        for (int i = 0; i < actual->numItems; i++)
        {
            printf("%d. %s (Peso: %d, Valor: %d)\n", 
                i+1, actual->items[i].nombre, 
                actual->items[i].peso, 
                actual->items[i].valor);
        }
    }
    
    //muestra el peso y valor de nuestro inventario
    printf("\nInventario (Peso: %d, Valor: %d):\n", inv->peso_total, inv->valor_total);
    for (int i = 0; i < inv->cantidad; i++)
    {
        printf("- %s\n", inv->items[i].nombre);
    }
    
    //te muestra las dirreciones que podemos tomar
    printf("\nDirecciones disponibles:\n");
    const char* dirs[] = {"Arriba", "Abajo", "Izquierda", "Derecha"};
    for (int i = 0; i < 4; i++)
    {
        if (actual->conexiones[i])
        {
            printf("%d. %s\n", i+1, dirs[i]);
        }
    }
    
    //muestra las opciones del menu
    printf("\nOpciones:\n1. Recoger item\n2. Descartar item\n3. Moverse\n4. Reiniciar\n5. Salir\n> ");
}

//recoge los item disponibles en cada escenario, elegido por el jugador
void recoger_item(NodoGrafo* actual, Inventario* inv, float* tiempo)
{
    //cuando escenario actual, no ai objetos muestra un mensaje
    if (actual->numItems == 0)
    {
        printf("No hay items para recoger!\n");
        return;
    }
    
    //pide al jugador, el item que quiere escoger
    printf("Item a recoger (0 para cancelar): ");
    int idx;
    scanf("%d", &idx);
    
    /*verifica si el numero elegido es valido, añade el objeto o item al inventario,
    actualizando peso y valor total, y al final resta 1 de tiempo*/
    if (idx > 0 && idx <= actual->numItems)
    {
        Item item = actual->items[idx-1];
        
        inv->items = realloc(inv->items, sizeof(Item) * (inv->cantidad + 1));
        inv->items[inv->cantidad++] = item;
        inv->peso_total += item.peso;
        inv->valor_total += item.valor;
        
        for (int i = idx-1; i < actual->numItems-1; i++)
        {
            actual->items[i] = actual->items[i+1];
        }
        actual->numItems--;
        
        *tiempo -= 1;
        printf("Item recogido: %s\n", item.nombre);
    }
}

void descartar_item(Inventario* inv, float* tiempo)
{
    //si no ai nada que descartar, muestra un mensaje
    if (inv->cantidad == 0)
    {
        printf("Inventario vacio!\n");
        return;
    }
    
    //muestra una lista enumerada de los item (objetos) del inventario
    printf("Item a descartar (0 para cancelar):\n");
    for (int i = 0; i < inv->cantidad; i++)
    {
        printf("%d. %s (Peso: %d)\n", i+1, inv->items[i].nombre, inv->items[i].peso);
    }
    
    //pide el numero del item a descartar
    int idx;
    scanf("%d", &idx);
    //verifica si es valido, resta el valor y peso del inventario, y resta 1 al tiempo del jugador
    if (idx > 0 && idx <= inv->cantidad)
    {
        printf("Descartado: %s\n", inv->items[idx-1].nombre);
        inv->peso_total -= inv->items[idx-1].peso;
        inv->valor_total -= inv->items[idx-1].valor;
        
        for (int i = idx-1; i < inv->cantidad-1; i++)
        {
            inv->items[i] = inv->items[i+1];
        }
        inv->cantidad--;
        *tiempo -= 1;
    }
}

void mover_jugador(NodoGrafo** actual, Inventario* inv, float* tiempo)
{
    const char* dirs[] = {"Arriba", "Abajo", "Izquierda", "Derecha"};
    
    printf("\nDireccion (1-4): ");
    int dir;
    scanf("%d", &dir);
    dir--;//Convertir a indice 0-3
    
    //verifica que la direccion sea valida, si no lo es, muestra un mensaje
    if (dir >= 0 && dir < 4 && (*actual)->conexiones[dir])
    {
        //calcula el tiempo y descuenta el costo del tiempo
        float costo_tiempo = (inv->peso_total + 1) / 10.0;
        *tiempo -= costo_tiempo;
        
        //mueve al jugador del nuevo escenario y muestra la informacion
        *actual = (*actual)->conexiones[dir];
        printf("\nTe moviste a: %s (Tiempo usado: %.2f)\n", (*actual)->nombre, costo_tiempo);
        
        //si llega al nodo final, termina el juego
        if ((*actual)->esFinal)
        {
            printf("\n¡Has Ganado!\nPuntaje final: %d\n", inv->valor_total);
            exit(0);
        }
    } else{
        printf("\n¡Camino no válido! (No se gastó tiempo)\n");
    }
}

//esta tiene la funcion de reiniciar el juego, e iniciar todo desde cero
void reiniciar_partida(Grafo** grafo, NodoGrafo** actual, Inventario* inv, float* tiempo)
{
    //Libera memoria de el grafo actual
    NodoGrafo* nodo = (*grafo)->lista;
    while (nodo) {
        NodoGrafo* siguiente = nodo->siguiente;
        if (nodo->items) free(nodo->items);
        free(nodo);
        nodo = siguiente;
    }
    free(*grafo);

    //vuelve a cargar el grafo desde el archivo
    *grafo = leer_escenarios("graphquest.csv");
    *actual = (*grafo)->inicio;

    //Vuelve todo lo que se haya avanzado al inicio, volviendo a cero y el tiempo volviendo a ser 10
    if (inv->items) free(inv->items);
    inv->cantidad = 0;
    inv->peso_total = 0;
    inv->valor_total = 0;
    *tiempo = 10.0;
    printf("\nPartida reiniciada!\n");
}

int main() 
{
    Grafo* grafo = leer_escenarios("graphquest.csv");
    if (!grafo)
    {
        printf("Error al cargar grafo\n");
        return 1;
    }

    //declarar el tiempo en float para ver cualquier gasto de tiempo
    float tiempo = 10.0;
    Inventario inv = {NULL, 0, 0, 0};
    NodoGrafo* actual = grafo->inicio;

    while (1)
    {
        mostrar_menu(actual, &inv, &tiempo);
        
        int opcion;
        scanf("%d", &opcion);
        
        switch(opcion)
        {
            case 1:
                recoger_item(actual, &inv, &tiempo);
                break;
            case 2:
                descartar_item(&inv, &tiempo);
                break;
            case 3:
                mover_jugador(&actual, &inv, &tiempo);
                break;
            case 4:
                reiniciar_partida(&grafo, &actual, &inv, &tiempo);
                break;
            case 5:
                printf("Saliendo del juego");
                return 0;
            default:
                printf("Opcion invalida\n");
        }
        
        if (tiempo <= 0)
        {
            printf("\n¡TIEMPO AGOTADO! Perdiste.\n");
            return 0;
        }
    }
    return 0;
}
