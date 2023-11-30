#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> // Incluye la biblioteca para soporte de caracteres especiales
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "Publicaciones.h"
#include "Publicaciones.c"

#include "List.h"

#ifndef DBG_HELP
#define DBG_HELP 0
#endif

#if DBG_HELP > 0
#define DBG_PRINT( ... ) do{ fprintf( stderr, "DBG:" __VA_ARGS__ ); } while( 0 )
#else
#define DBG_PRINT( ... ) ;
#endif

//==== MAIN ====//
int main() {
    setlocale(LC_ALL, ""); // Configura la localización para soporte de caracteres especiales
    //Display_LoadingScreen();

    graph = Graph_New(TABLE_SIZE, eGraphType_UNDIRECTED);
    Start();
    Graph_Delete(&graph);

    return 0;
}
