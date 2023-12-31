
#include <time.h>
#include <stdio.h>

#include "Publicaciones.h"
#include "List.h"
#include "List.c"

#ifndef DBG_HELP
#define DBG_HELP 0
#endif

#if DBG_HELP > 0
#define DBG_PRINT( ... ) do{ fprintf( stderr, "DBG:" __VA_ARGS__ ); } while( 0 )
#else
#define DBG_PRINT( ... ) ;
#endif

// Estructura de la tabla hash
User* hashTable[TABLE_SIZE];

Graph* graph;


//                  FUNCIONES DEL GRAFO                  //

static int find( User users[], int size, int userid )
{
   for( int i = 0; i < size; ++i )
   {
      if( users[ i ].graph_index == userid ) return i;
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

      printf( "[%d]User%d- %s=> Friend ", i, user->graph_index, user->username );
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


void Insert_User_Graph(char *username, Graph* g)
{
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

   // obtenemos los Indices correspondientes:
   int index_start = find( g->users, g->size, userstart );
   int index_finish = find( g->users, g->size, userfinish );

   DBG_PRINT( "AddEdge(): from:%d (with index:%d), to:%d (with index:%d)\n", start, start_idx, finish, finish_idx );

   if( index_start == -1 || index_finish == -1 ) return false;
   // uno o ambos vetices no existen

   //Dadas las condiciones de amistad, el grafo ya es UNDIRECTED
   insert( &g->users[ index_start ], index_finish, 0.0 );
   //Se agrega la relacion inversa
   insert( &g->users[ index_finish ], index_start, 0.0 );

   return true;
}

//                  FIN DE FUNCIONES DEL GRAFO                  //



//                 FUNCIONES DE CREACION DE JSON                   //
void Create_Json_User(User* user) {
    if (!user->post || List_Is_empty(user->post)) {
        printf("No hay publicaciones para el usuario %s.\n", user->username);
        return;
    }

    char file_name[100];
    sprintf(file_name, "Publicaciones_%s.json", user->username);

    FILE* json_output = fopen(file_name, "w");
    if (!json_output) {
        printf("Error abriendo el archivo %s\n", file_name);
        return;
    }

    fprintf(json_output, "{\n");
    fprintf(json_output, "  \"username\": \"%s\",\n", user->username);
    fprintf(json_output, "  \"publicaciones\": [\n");

    for (List_Cursor_front(user->post); !List_Cursor_end(user->post); ) {
        Data Data_publication = List_Cursor_get(user->post);
        Post* publication = &Data_publication.post;

        fprintf(json_output, "    {\n");
        fprintf(json_output, "      \"content\": \"%s\",\n", publication->content);
        fprintf(json_output, "      \"likes\": %d,\n", publication->likes);
        fprintf(json_output, "      \"shares\": %d,\n", publication->shares);
        fprintf(json_output, "      \"num_comments\": %d,\n", publication->num_comments);
        fprintf(json_output, "      \"date\": \"%s\"\n", publication->date);
        fprintf(json_output, "      \"hour\": \"%s\"\n", publication->hour);
        fprintf(json_output, "    }%s\n", List_Cursor_next(user->post) ? "," : "");
    }

    fprintf(json_output, "  ]\n");
    fprintf(json_output, "}\n");

    fclose(json_output);

    printf("Archivo %s creado con �xito.\n", file_name);
}


void Read_json_post(char* file_name) {
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


// Funcion para limpiar el buer de entrada debido a problemas con el buffer
//Necesaria para escribir publicaciones (REVISAR)
void CleanBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


void Make_Publication(User* user) {
    assert(user);

    // Verificar que la lista de publicaciones no sea nula
    if (!user->post) {
        user->post = List_New();  // Inicializa la lista de publicaciones si es nula
    }

    if (user->post) {
        Post New_publication;

        printf("Ingrese el contenido de la publicacion (max 199 caracteres):\n");
        fgets(New_publication.content, sizeof(New_publication.content), stdin);
        CleanBuffer();
        New_publication.content[strcspn(New_publication.content, "\n")] = '\0';


        // Inicializar las otras propiedades del Post
        New_publication.likes = 0;
        New_publication.shares = 0;
        New_publication.num_comments = 0;

        // Obtener la fecha y hora actual
        time_t tiempo_actual;
        time(&tiempo_actual);
        struct tm* tiempo_desglosado = localtime(&tiempo_actual);

        // Formatear la fecha y hora
        sprintf(New_publication.date, "%02d/%02d/%04d",
        tiempo_desglosado->tm_mday, tiempo_desglosado->tm_mon + 1, tiempo_desglosado->tm_year + 1900);
        sprintf(New_publication.hour, "%02d:%02d:%02d",
        tiempo_desglosado->tm_hour, tiempo_desglosado->tm_min, tiempo_desglosado->tm_sec);

        // Añadir la publicacion a la lista de publicaciones del usuario
        List_Push_back_post(user->post, &New_publication);

        printf("Publicacion realizada con exito!\n");
        getchar();
        // Creacion del json
        Create_Json_User(user);

    } else {
        printf("Error: La lista de publicaciones es nula.\n");
    }
}


// Funcion para mostrar las publicaciones de un usuario
void Show_Post(User* user) {
    assert(user);

    printf("=== Publicaciones de %s ===\n", user->username);

    if (user->post) {
        for (List_Cursor_front(user->post); !List_Cursor_end(user->post); List_Cursor_next(user->post)) {
            Data Data_publication = List_Cursor_get(user->post);
            Post* Publication = &Data_publication.post;

            printf("Fecha: %s\n", Publication->date);
            printf("Contenido: %s\n", Publication->content);

            char file_name[100];
            sprintf(file_name, "Publicaciones_%s.json", user->username);
            Read_json_post(file_name);
            printf("\n");
        }
    } else {
        printf("No hay publicaciones disponibles.\n");
    }
}

//                  FIN DE CREACION DE PUBLICACIONES                  //




//                 FUNCIONES DE PETICION DE AMISTAD Y ENLACE       //
//                       REVISAR ADMINISTRACION                    //

//Funcion para andar solicitudes de amistad
void Send_FriendRequest(User* sender, User* receiver) {
    if (!sender->pending_friends ) {
        sender->pending_friends = List_New();
    }
    if (!receiver->pending_friends ) {
        receiver->pending_friends = List_New();
    }
    if (!receiver->friend_requests) {
        receiver->friend_requests = List_New();
    }
    if (!sender->friend_requests) {
        sender->friend_requests = List_New();
    }

    // Verifica si ya son amigos
    if (find_neighbor(sender, receiver->graph_index)) {
        printf("Ya son amigos.\n");
        return;
    }
    // Verifica si ya hay una solicitud pendiente
    if (List_Find(sender->friend_requests, receiver->graph_index)) {
        printf("Ya has enviado una solicitud de amistad a este usuario.\n");
        return;
    }

    // Agrega la solicitud a la lista del remitente y del destinatario
    List_Push_back(sender->friend_requests, receiver->graph_index, 0.0);
    List_Push_back(receiver->pending_friends, sender->graph_index, 0.0);

    printf("Solicitud de amistad enviada a %s.\n", receiver->username);
}

//Funcion para aceptar solicitudes de amistad
void Accept_FriendRequest(User* receiver, User* sender, Graph* graph) {
    // Verificar si la solicitud existe
    int requestIndex = List_Find(receiver->pending_friends, sender->graph_index);

    if (requestIndex != -1) {
        // Eliminar la solicitud de amistad de la lista del receptor
        List_Remove(receiver->pending_friends, requestIndex);

        // Verificar si el remitente tambi�n tiene una solicitud pendiente
        int senderRequestIndex = List_Find(sender->friend_requests, receiver->graph_index);
        if (senderRequestIndex != -1) {
            // Eliminar la solicitud de amistad del remitente
            List_Remove(sender->friend_requests, senderRequestIndex);

            //Creamos relacion de amistad
            if (Graph_AddFriendship(graph, receiver->graph_index, sender->graph_index)) {
                printf("Amistad agregada correctamente.\n");
            } else {
                printf("Error al agregar la amistad: Usuarios no encontrados en el grafo.\n");
            }
            printf("Ahora eres amigo de %s.\n", sender->username);
        } else {
            printf("Error: La solicitud de amistad del remitente no fue encontrada.\n");
        }
    } else {
        printf("Error: No hay ninguna solicitud de amistad pendiente de este usuario.\n");
    }
}

//Funcion para denegar solicitudes
void DenyFriendRequest(User* receiver, User* sender) {
    // Elimina la solicitud de amistad de la lista del receptor
    List_Remove(receiver->pending_friends, sender->graph_index);
    List_Remove(sender->friend_requests, receiver->graph_index);
    printf("Has denegado la solicitud de amistad de %s.\n", sender->username);
}

//Funcion de administracion de solicitudes
void Show_FriendRequest(User* user, Graph* graph) {
    assert(user);

    printf("=== Solicitudes de amistad pendientes para %s ===\n", user->username);

    if (!List_Is_empty(user->pending_friends)) {
        for (List_Cursor_front(user->pending_friends); !List_Cursor_end(user->pending_friends); List_Cursor_next(user->pending_friends)) {
            Data Request = List_Cursor_get(user->pending_friends);
            int friend_idx = Request.index;

            if (friend_idx >= 0 && friend_idx < graph->len) {
                User* friend_user = &graph->users[friend_idx];
                printf("Usuario pendiente: %s\n", friend_user->username);
            } else {
                printf("Error: Usuario no encontrado con el índice %d.\n", friend_idx);
            }
        }
        int choice;
        printf("1. Aceptar solicitud\n2. Denegar solicitud\nElija una opci�n: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                // Aceptar solicitud de amistad
                printf("Ingrese el nombre de usuario al que desea aceptar la solicitud de amistad: ");
                char friend_username[50];
                scanf("%s", friend_username);
                User* friend_user = find_user(friend_username);
                if (friend_user != NULL) {
                    Accept_FriendRequest(user, friend_user, graph);
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
            default:
                printf("Opci�n no v�lida.\n");
        }

    }
    else {
        printf("No hay solicitudes de amistad pendientes.\n");
    }
}

void printHistorial(HistorialChat *historial ) {
    for (int i = 0; i < historial ->count; i++) {
        printf("%s: %s\n", historial ->messages[i].usuario, historial ->messages[i].mensaje);
    }
}

void addMessage(HistorialChat *historial , const char *usuario, const char *mensaje) {
    if (historial ->count < MAX_MESSAGES) {
        strcpy(historial ->messages[historial ->count].usuario, usuario);
        strcpy(historial ->messages[historial ->count].mensaje, mensaje);
        historial ->count++;
    } else {
        printf("El historial del chat esta lleno. No se pueden añadir mas mensajes.\n");
    }
}

//==== FUNCIONES LOGICAS ====//

// Función para calcular el índice en la tabla hash
int hash(char *str) {
    int hash = 0;
    int c;
    while ((c = *str++)) {
        hash = (hash * 31) + c;
    }
    return hash % TABLE_SIZE;
}

// Función para agregar un usuario a la tabla hash
void insert_user(char *username, char *password, Graph* graph) {
    int index = hash(username);
    User* new_user = (User*)malloc(sizeof(User));
    if (new_user == NULL) {
        fprintf(stderr, "Error al asignar memoria para el usuario.\n");
        return;
    }
    strncpy(new_user->username, username, sizeof(new_user->username));
    strncpy(new_user->password_hash, password, sizeof(new_user->password_hash));
    new_user->next = hashTable[index];
    hashTable[index] = new_user;
    Insert_User_Graph(username, graph);
}


// Función para verificar la contraseña de un usuario
int verify_password(User *user, char *password) {
    // En un entorno de producción, deberías utilizar una función de hash segura aquí
    return strcmp(user->password_hash, password) == 0;
}

//==== FUNCIONES ESTETICAS ====//
void Display_LoadingScreen(){
    // Barra de carga
    char loading[] = "|/-\\";
    int i;
    for (i = 0; i < 100; i++) {
        printf("\rCargando... [%c] %d%%", loading[i % 4], i + 1);
        fflush(stdout);
        system("sleep 0.75");
    }
    printf("\nCarga completa!");

    // Espera 1 segundos antes de borrar la pantalla
    system("sleep 1");
    system("cls");

    printf(" _\n|_) o  _ __    _ __  o  _| _\n|_) | (/_| |\\_/(/_| | | (_|(_)");
    printf("\n\n             _\n            (_|\n\n");
    printf("nombre red social");

    system("sleep 2");
    system("cls");
}


// Función para mostrar el perfil de un usuario
void Show_Profile(User *user) {
    printf("\n\n=== Perfil de %s ===\n", user->username);
}

void chat(User *stored_user, User* destinatary_user, Graph* graph, HistorialChat *historial) {
    char destinatary[MAX_USERNAME_LENGTH];

    printf("Introduce el nombre de usuario al que deseas enviar un mensaje: ");
    getchar(); // Limpiar búfer del teclado
    fgets(destinatary, MAX_USERNAME_LENGTH, stdin);
   destinatary[strcspn(destinatary, "\n")] = '\0'; // Eliminar el carácter de nueva línea

   destinatary_user = find_user(destinatary);
    if (destinatary_user == NULL) {
        printf("Usuario no encontrado.\n");
        return;
    }

    printf("Bienvenido al chat con %s, %s!\n", destinatary, stored_user->username);

    while (1) {
        char mensaje[MAX_MESSAGE_LENGTH];

        printf("Introduce tu mensaje (escribe 'salir' para terminar el chat): ");
        fgets(mensaje, MAX_MESSAGE_LENGTH, stdin);
        mensaje[strcspn(mensaje, "\n")] = '\0'; // Eliminar el carácter de nueva línea

        if (strcmp(mensaje, "salir") == 0) {
            break;
        }

        addMessage(historial, stored_user->username, mensaje);

        printf("\nHistorial de Chat con %s:\n", destinatary);
       printHistorial(historial);
        printf("\n");
    }

    printf("Saliendo del Chat con %s. Adiós, %s!\n", destinatary, stored_user->username);
}

// Función para manejar la opción de chat
void handle_option_chat(User *stored_user, User* destinatary_user, Graph* grafo) {
    HistorialChat historial;
    historial.count = 0;
    chat(stored_user, destinatary_user, grafo, &historial);
}


// Función para buscar un usuario en la tabla hash y devolver un puntero al usuario encontrado
User* find_user(char *username) {
    int index = hash(username);
    User* current = hashTable[index];
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int Start(){
    char username[50];
    char password[50];
    int choice;

    while (1) {
        printf("\n=== BIENVENIDO ===\n");
        printf("1. Iniciar sesi�n\n");
        printf("2. Crear cuenta\n");
        printf("3. Salir\n");
        printf("====================\n");
        printf("Elija una opci�n: ");
        scanf("%d", &choice);
        CleanBuffer();
        User *stored_user;
        switch (choice) {
            case 1:
                printf("\n=== Iniciar sesi�n ===\n");
                printf("Usuario: ");
                scanf("%s", username);
                printf("Contrase�a: ");
                scanf("%s", password);
                CleanBuffer();
                stored_user = find_user(username);
                if (stored_user != NULL && verify_password(stored_user, password)) {
                    int loggedIn = 1;
                    while (loggedIn) {
                        system("cls");

                        Show_Profile(stored_user);
                        printf("\n1. Realizar publicación\n2. Ver publicaciones de feed\n3. Ver perfil personal\n");
                        printf("4. Ver solicitudes de amistad\n5. Agregar amigo\n6. Chat\n7. Cerrar sesión\n8. Salir\n");
                        printf("Elija una opción: ");

                        scanf("%d", &choice);
                        CleanBuffer();
                        switch (choice) {
                            case 1:
                                //Publicar
                                Make_Publication(stored_user);
                                //goto cuenta_abierta;
                                system("cls");
                                break;
                            case 2:
                                //Ver publicacione
                                //Revisar_feed();
                                //aumentara likescoment, compart
                                system("cls");
                                break;
                            case 3:
                                //Ver perfil propio
                                Show_Profile(stored_user);
                                Show_Post(stored_user);
                                //goto cuenta_abierta;
                                system("cls");
                                break;
                            case 4:
                                //Agregar amigo
                                Show_FriendRequest(stored_user,graph);
                                system("cls");
                                break;
                            case 5:
                                //Se envia solicitud de amistad
                                Graph_Print(graph,0.0);
                                printf("Ingrese el nombre de usuario al que desea enviar una solicitud de amistad: ");
                                char friend_username[50];
                                scanf("%s", friend_username);
                                User* friend_user = find_user(friend_username);
                                if (friend_user != NULL) {
                                    Send_FriendRequest(stored_user, friend_user);
                                } else {
                                    printf("Usuario no encontrado.\n");
                                }
                                system("cls");
                                break;
                            case 6:
                                //Chat
                                printf("Introduce el nombre de usuario con el que deseas chatear: ");
                                char chat_username[50];
                                scanf("%s", chat_username);
                                fflush(stdin);
                                User* chat_user = find_user(chat_username);
                                if (chat_user != NULL) {
                                    handle_option_chat(stored_user, chat_user, graph);
                                } else {
                                    printf("Usuario no encontrado.\n");
                                }
                                system("cls");
                                break;
                            case 7:
                                //Cerrar sesion
                                loggedIn = 0;
                                system("cls");
                                break;
                            case 8:
                                //Salir de la aplicacion
                                printf("Gracias por usar nuestra aplicaci�n. �Hasta pronto!\n");
                                return 0;
                            default:
                                printf("Opci�n no v�lida. Por favor, elija una opci�n v�lida.\n");
                        }
                    }
                } else {
                    printf("Usuario o contrase�a incorrectos.\n");
                }
                break;
            case 2:
                printf("\n=== Crear cuenta ===\n");
                printf("Nuevo usuario: ");
                scanf("%s", username);
                printf("Nueva contraseña: ");
                scanf("%s", password);

                if (find_user(username) == NULL) {
                    insert_user(username, password, graph);
                    printf("Cuenta creada con Éxito para %s.\n", username);
                    printf("Vuelva a iniciar sesion para terminar");
                } else {
                    printf("El usuario ya existe. Intente con un nombre de usuario diferente.\n");
                }
                break;
            case 3:
                printf("Gracias por usar nuestra aplicaci�n. �Hasta pronto!\n");
                return 0;
            default:
                printf("Opci�n no v�lida. Por favor, elija una opci�n v�lida.\n");
                break;
        }
    }
}
