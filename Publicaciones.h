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
    List* friend_requests; //Lista de solicitudes enviadas
    List* pending_friends;  //Lista de solicitudes recibidas
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

//                  FUNCIONES DEL GRAFO                  //                          
static int find( User users[], int size, int userid );
static bool find_neighbor( User* user, int index );
Graph* Graph_New( int size, eGraphType type );
void Graph_Delete( Graph** g );
static void insert( User* user, int index, float weigth );
void Graph_Print( Graph* g, int depth );
void Insert_User_Graph(char *username, Graph* g);
bool Graph_AddFriendship( Graph* g, int userstart, int userfinish );

//            FUNCIONES DE PUBLICACIONES / ARCHIVOS           // 
void Create_Json_User(User* user);
void Read_json_post(char* file_name);
void CleanBuffer();
void Make_Publication(User* user);
void Show_Post(User* user);

//           FUNCIONES DE SOLICITUDES DE AMISTAD          // 
void Send_FriendRequest(User* sender, User* receiver);
void Accept_FriendRequest(User* receiver, User* sender, Graph* grafo);
void Show_FriendRequest(User* user, Graph* grafo);
