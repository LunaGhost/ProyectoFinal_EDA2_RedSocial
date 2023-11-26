#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> // Incluye la biblioteca para soporte de caracteres especiales
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "List.h"

typedef struct User {
    //Para la tabla hash
    char username[50];
    char password_hash[64]; // Hash de contrase�a (SHA-256)
    struct User* next;

    //Para el grafo
    int graph_index;
    List* post;  //Lista de publicaciones del perfil
    List* friends;  //Lista de amigos
    List* friend_requests;
    List* pending_friends;
} User;


//SOLOU SAREMOS GRAFO NO DIRIGINO PERO MANTENEMOS ESTRUCTURA
//De ambos tipos
typedef enum
{
   eGraphType_UNDIRECTED,
   eGraphType_DIRECTED
} eGraphType;

typedef struct
{
   User* users;
   int size;
   int len;

   eGraphType type;
} Graph;


static int find( User usuarios[], int size, int userid );
static bool find_neighbor( User* user, int index );
Graph* Graph_New( int size, eGraphType type );
void Graph_Delete( Graph** g );
static void insert( User* user, int index, float weigth );
void Graph_Print( Graph* g, int depth );
void Graph_AddUser( Graph* g, int user_id );
void Insert_User_Graph(char *username, Graph* g);
bool Graph_AddFriendship( Graph* g, int userstart, int userfinish );
void limpiarBuffer();
void Realizar_Publicacion(User* user);
void Mostrar_Publicaciones(User* user);
void SendFriendRequest(User* sender, User* receiver, Graph* grafo);
void AcceptFriendRequest(User* receiver, User* sender, Graph* grafo);
void MostrarSolicitudesAmistad(User* user, Graph* grafo);
