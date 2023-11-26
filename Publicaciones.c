#include "Publicaciones.h"
#include <time.h>
#include <stdio.h>

#ifndef DBG_HELP
#define DBG_HELP 0
#endif

#if DBG_HELP > 0
#define DBG_PRINT( ... ) do{ fprintf( stderr, "DBG:" __VA_ARGS__ ); } while( 0 )
#else
#define DBG_PRINT( ... ) ;
#endif

//                  FUNCIONES DEL GRAFO                  //

User* find_user(char *username);
static int find( User usuarios[], int size, int userid )
{
   for( int i = 0; i < size; ++i )
   {
      if( usuarios[ i ].graph_index == userid ) return i;
   }

   return -1;
}

static bool find_neighbor( User* user, int index )
{
   if( user->friends )
   {
      return List_Find( user->friends, index );
   }
   return false;
}


Graph* Graph_New( int size, eGraphType type )
{
   assert( size > 0 );

   Graph* g = (Graph*) malloc( sizeof( Graph ) );
   if( g )
   {
      g->size = size;
      g->len = 0;
      g->type = type;

      g->users = (User*) calloc( size, sizeof( User ) );

      if( !g->users )
      {
         free( g );
         g = NULL;
      }
   }

   return g;
   // el cliente es responsable de verificar que el grafo se haya creado correctamente
}



void Graph_Delete( Graph** g )
{
   assert( *g );

   Graph* graph = *g;

   for( int i = 0; i < graph->size; ++i )
   {
      User* user = &graph->users[ i ];

      if( user->friends )
      {
         List_Delete( &(user->friends) );
      }
   }

   free( graph->users );
   free( graph );
   *g = NULL;
}

static void insert( User* user, int index, float weigth )
{
   // crear la lista si no existe!

   if( !user->friends )
   {
      user->friends = List_New();
   }

   if( user->friends && !find_neighbor( user, index ) )
   {
      List_Push_back( user->friends, index, weigth );

      DBG_PRINT( "insert():Inserting the neighbor with idx:%d\n", index );
   }
   else DBG_PRINT( "insert: duplicated index\n" );
}

void Graph_Print( Graph* g, int depth )
{
   for( int i = 0; i < g->len; ++i )
   {
      User* user = &g->users[ i ];

      printf( "[%d]User%d=> Friend ", i, user->graph_index );
      if( user->friends )
      {
         for( List_Cursor_front( user->friends );
              ! List_Cursor_end( user->friends );
              List_Cursor_next( user->friends ) )
         {

            Data d = List_Cursor_get( user->friends );
            int friend_idx = d.index;

            printf( "%s->", g->users[ friend_idx ].username);
         }
      }
      printf( "Nil\n" );

   }
   printf( "\n" );
}


void Insert_User_Graph(char *username, Graph* g){
    assert(g->len < g->size);

    User* new_user = &g->users[g->len];
    new_user->post = List_New();
    new_user->friends = List_New();
    new_user->friend_requests = List_New();
    new_user->pending_friends = List_New();
    strncpy(new_user->username, username, sizeof(new_user->username));
    new_user->graph_index = g->len;
    ++g->len;
}

bool Graph_AddFriendship( Graph* g, int userstart, int userfinish )
{
   assert( g->len > 0 );

   // obtenemos los índices correspondientes:
   int index_start = find( g->users, g->size, userstart );
   int index_finish = find( g->users, g->size, userfinish );

   DBG_PRINT( "AddEdge(): from:%d (with index:%d), to:%d (with index:%d)\n", start, start_idx, finish, finish_idx );

   if( index_start == -1 || index_finish == -1 ) return false;
   // uno o ambos vértices no existen

   //Dadas las condiciones de amistad, el grafo ya es UNDIRECTED
   insert( &g->users[ index_start ], index_finish, 0.0 );
   //Se agrega la relacion inversa
   insert( &g->users[ index_finish ], index_start, 0.0 );

   return true;
}

//                  FIN DE FUNCIONES DEL GRAFO                  //



//                 FUNCIONES DE CREACION DE JSON                   //
void create_json_user(User* user) {
    if (!user->post || List_Is_empty(user->post)) {
        printf("No hay publicaciones para el usuario %s.\n", user->username);
        return;
    }

    char file_name[100];
    sprintf(file_name, "publicaciones_%s.json", user->username);

    FILE* json_output = fopen(file_name, "w");
    if (!json_output) {
        printf("Error abriendo el archivo %s\n", file_name);
        return;
    }

    fprintf(json_output, "{\n");
    fprintf(json_output, "  \"username\": \"%s\",\n", user->username);
    fprintf(json_output, "  \"publicaciones\": [\n");

    for (List_Cursor_front(user->post); !List_Cursor_end(user->post); ) {
        Data data_publicacion = List_Cursor_get(user->post);
        Post* publicacion = &data_publicacion.post;

        fprintf(json_output, "    {\n");
        fprintf(json_output, "      \"content\": \"%s\",\n", publicacion->content);
        fprintf(json_output, "      \"likes\": %d,\n", publicacion->likes);
        fprintf(json_output, "      \"shares\": %d,\n", publicacion->shares);
        fprintf(json_output, "      \"num_comments\": %d,\n", publicacion->num_comments);
        fprintf(json_output, "      \"date\": \"%s\"\n", publicacion->date);
        fprintf(json_output, "      \"hour\": \"%s\"\n", publicacion->hour);
        fprintf(json_output, "    }%s\n", List_Cursor_next(user->post) ? "," : "");
    }

    fprintf(json_output, "  ]\n");
    fprintf(json_output, "}\n");

    fclose(json_output);

    printf("Archivo %s creado con éxito.\n", file_name);
}


void leer_json_post(char* file_name) {
    FILE* json_input = fopen(file_name, "r");
    if (!json_input) {
        printf("Error abriendo el archivo JSON\n");
        return;
    }

    char buffer[256];
    size_t len = sizeof(buffer);
    while (fgets(buffer, len, json_input) != NULL) {
        printf("%s", buffer);
    }

    fclose(json_input);
}

//                  FIN DE CREACION DE JSON                    //


//                  FUNCIONES DE CREACION DE PUBLICACIONES                  //


// Función para limpiar el búfer de entrada debido a problemas con el buffer
//Necesaria para escribir publicaciones (REVISAR)
void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


void Realizar_Publicacion(User* user) {
    assert(user);

    // Verificar que la lista de publicaciones no sea nula
    if (!user->post) {
        user->post = List_New();  // Inicializa la lista de publicaciones si es nula
    }

    if (user->post) {
        Post nueva_publicacion;

        printf("Ingrese el contenido de la publicacion (max 199 caracteres):\n");
        limpiarBuffer();
        fgets(nueva_publicacion.content, sizeof(nueva_publicacion.content), stdin);
        nueva_publicacion.content[strcspn(nueva_publicacion.content, "\n")] = '\0';

        // Inicializar las otras propiedades del Post
        nueva_publicacion.likes = 0;
        nueva_publicacion.shares = 0;
        nueva_publicacion.num_comments = 0;

        // Obtener la fecha y hora actual
        time_t tiempo_actual;
        time(&tiempo_actual);
        struct tm* tiempo_desglosado = localtime(&tiempo_actual);

        // Formatear la fecha y hora
        sprintf(nueva_publicacion.date, "%02d/%02d/%04d",
        tiempo_desglosado->tm_mday, tiempo_desglosado->tm_mon + 1, tiempo_desglosado->tm_year + 1900);
        sprintf(nueva_publicacion.hour, "%02d:%02d:%02d",
        tiempo_desglosado->tm_hour, tiempo_desglosado->tm_min, tiempo_desglosado->tm_sec);

        // Añadir la publicación a la lista de publicaciones del usuario
        List_Push_back_post(user->post, &nueva_publicacion);

        printf("Publicacion realizada con exito!\n");

        // Creacion del json
        create_json_user(user);

    } else {
        printf("Error: La lista de publicaciones es nula.\n");
    }
}


// Función para mostrar las publicaciones de un usuario
void Mostrar_Publicaciones(User* user) {
    assert(user);

    printf("=== Publicaciones de %s ===\n", user->username);

    if (user->post) {
        for (List_Cursor_front(user->post); !List_Cursor_end(user->post); List_Cursor_next(user->post)) {
            Data data_publicacion = List_Cursor_get(user->post);
            Post* publicacion = &data_publicacion.post;

            printf("Fecha: %s\n", publicacion->date);
            printf("Contenido: %s\n", publicacion->content);

            char file_name[100];
            sprintf(file_name, "Publicaciones_%s.json", user->username);
            leer_json_post(file_name);
            printf("\n");
        }
    } else {
        printf("No hay publicaciones disponibles.\n");
    }
}

//                  FIN DE CREACION DE PUBLICACIONES                  //




//                 FUNCIONES DE PETICION DE AMISTAD Y ENLACE       //
//                       REVISAR ADMINISTRACION                  //
/*
void SendFriendRequest(User* sender, User* receiver, Graph* grafo) {
    if (!sender->pending_friends ) {
        sender->pending_friends = List_New();
    }
    if (!receiver->friend_requests) {
        receiver->friend_requests = List_New();
    }

    // Verifica si ya son amigos
    if (find_neighbor(sender, receiver->graph_index)) {
        printf("Ya son amigos.\n");
        return;
    }
    if (sender->pending_friends == NULL) {
        printf("Error: La lista de solicitudes pendientes del remitente es nula.\n");
        return;
    }
    // Verifica si ya hay una solicitud pendiente
    if (List_Find(sender->pending_friends, receiver->graph_index)) {
        printf("Ya has enviado una solicitud de amistad a este usuario.\n");
        return;
    }

    // Agrega la solicitud a la lista del remitente y del destinatario
    List_Push_back(sender->pending_friends, receiver->graph_index, 0.0);
    List_Push_back(receiver->friend_requests, sender->graph_index, 0.0);

    printf("Solicitud de amistad enviada a %s.\n", receiver->username);
}




// Función para aceptar una solicitud de amistad
void AcceptFriendRequest(User* receiver, User* sender, Graph* grafo) {
    // Verificar si la solicitud existe
    int requestIndex = List_Find(receiver->pending_friends, sender->graph_index);

    if (requestIndex != -1) {
        // Eliminar la solicitud de amistad de la lista del receptor
        List_Remove(receiver->pending_friends, requestIndex);

        // Verificar si el remitente también tiene una solicitud pendiente
        int senderRequestIndex = List_Find(sender->friend_requests, receiver->graph_index);
        if (senderRequestIndex != -1) {
            // Eliminar la solicitud de amistad del remitente
            List_Remove(sender->friend_requests, senderRequestIndex);

            // Agregar a ambos usuarios como amigos
            if (Graph_AddFriendship(grafo, receiver->graph_index, sender->graph_index)) {
                printf("Amistad agregada correctamente.\n");
            } else {
                printf("Error al agregar la amistad.\n");
            }

            printf("Ahora eres amigo de %s.\n", sender->username);
        } else {
            printf("Error: La solicitud de amistad del remitente no fue encontrada.\n");
        }
    } else {
        printf("Error: No hay ninguna solicitud de amistad pendiente de este usuario.\n");
    }
}



void DenyFriendRequest(User* receiver, User* sender) {
    // Elimina la solicitud de amistad de la lista del receptor
    List_Remove(receiver->pending_friends, sender->graph_index);
    List_Remove(sender->friend_requests, receiver->graph_index);
    printf("Has denegado la solicitud de amistad de %s.\n", sender->username);
}

void MostrarSolicitudesAmistad(User* user, Graph* grafo) {
    assert(user);

    printf("=== Solicitudes de amistad pendientes para %s ===\n", user->username);

    if (!List_Is_empty(user->friend_requests)) {
        for (List_Cursor_front(user->friend_requests); !List_Cursor_end(user->friend_requests); List_Cursor_next(user->friend_requests)) {
            Data solicitud = List_Cursor_get(user->friend_requests);
            int friend_idx = solicitud.index;

            // Utilizar el índice directamente para obtener el nombre del usuario
            if (friend_idx >= 0 && friend_idx < grafo->len) {
                printf("Usuario pendiente: %s\n", grafo->users[friend_idx+1].username);
            } else {
                printf("Error: Usuario no encontrado con el índice %d.\n", friend_idx);
            }
        }
        // Aquí puedes agregar la opción de aceptar o denegar solicitudes
        int choice;
        printf("1. Aceptar solicitud\n2. Denegar solicitud\n3. Volver\nElija una opción: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                // Aceptar solicitud de amistad
                printf("Ingrese el nombre de usuario al que desea aceptar la solicitud de amistad: ");
                char friend_username[50];
                scanf("%s", friend_username);
                User* friend_user = find_user(friend_username);
                if (friend_user != NULL) {
                    AcceptFriendRequest(user, friend_user, grafo);
                } else {
                    printf("Usuario no encontrado.\n");
                }
                break;
            case 2:
                // Denegar solicitud de amistad
                printf("Ingrese el nombre de usuario al que desea denegar la solicitud de amistad: ");
                char friend_username_deny[50];
                scanf("%s", friend_username_deny);
                User* friend_user_deny = find_user(friend_username_deny);
                if (friend_user_deny != NULL) {
                    DenyFriendRequest(user, friend_user_deny);
                } else {
                    printf("Usuario no encontrado.\n");
                }
                break;
            case 3:
                // Volver al menú anterior
                break;
            default:
                printf("Opción no válida.\n");
        }
    } else {
        printf("No hay solicitudes de amistad pendientes.\n");
    }
}
*/
