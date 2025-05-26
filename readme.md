# GraphQuest
# Hecho por Hans Gonzalez - Paralelo 2

Este programa permite recorrer distintos escenarios que son cargados de un archivo CSV. Utilizando un grafo para poder avanzar por los escenarios, donde el objetivo es ir recogiendo los objetos de mayor valor posible e intentando llegar a la salida, antes de que se agote el tiempo, debes tomar las decisiones correctas, con el tiempo pisando tus talones.

## Funcionalidades Implementadas

1. **leer escenarios** desde un archivo CSV.
2. **mostrar menu** mostrar el estado actual.
3. **recoger item** permite al jugador recoger un objeto.
4. **descartar item**, permite al jugador descartar los items.
5. **mover jugador**, permite que el jugador se mueva a distintos escenarios.
6. **reiniciar partida**, reinicia la partida:
7. **Salir del juego**.

## Funcionalidades con errores conocidos
El codigo tiene todo en el main, lo cual hace notar que el codigo en ciertas
zonas no se vea tan agradable a la vista. (post. esto paso por tema de salud, el dejar el codigo de esta manera)

## Requisitos del sistema
- Archivos fuente del proyecto:
  - `main.c`
- Archivo CSV graphquest.csv

## Cómo ejecutar el programa
1. Copiar el link de github y clonarlo en visual studio

2. Abrir la terminal, utilizando powershell

3. Ejecutar el programa con:
  gcc main.c -o main
  ./main


# Ejemplo de uso
//
=== Entrada principal (Tiempo: 10.00) ===
Descripcion: "Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver."

Inventario (Peso: 0, Valor: 0):

Direcciones disponibles:
2. Abajo

Opciones:
1. Recoger item
2. Descartar item
3. Moverse
4. Reiniciar
5. Salir
> 3

Direccion (1-4): 2

Te moviste a: Cocina (Tiempo usado: 0.10)

=== Cocina (Tiempo: 9.90) ===
Descripcion: "Restos de una batalla culinaria. Cuchillos por aqui, migajas por alla. Alguien cocinoÔÇª o fue cocinado."

Items disponibles:
1. "Cuchillo (Peso: 1, Valor: 3)
2. Pan (Peso: 1, Valor: 2)

Inventario (Peso: 0, Valor: 0):

Direcciones disponibles:
1. Arriba
2. Abajo
4. Derecha

Opciones:
1. Recoger item
2. Descartar item
3. Moverse
4. Reiniciar
5. Salir
//

## Problemas conocidos
- Liberación de memoria: Ocasionalmente ocurre un segmentation fault al salir del programa.
- Al reiniciar, los objetos tomados, no se reinician
