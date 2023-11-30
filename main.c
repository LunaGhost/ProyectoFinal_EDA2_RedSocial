#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> // Incluye la biblioteca para soporte de caracteres especiales
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "Publicaciones.h"

#define MAX_MESSAGES 100
#define MAX_MESSAGE_LENGTH 256
#define MAX_USERNAME_LENGTH 30


#ifndef DBG_HELP
#define DBG_HELP 0
#endif

#if DBG_HELP > 0
#define DBG_PRINT( ... ) do{ fprintf( stderr, "DBG:" __VA_ARGS__ ); } while( 0 )
#else
#define DBG_PRINT( ... ) ;
#endif


//== ESTRUCTURAS ==//
typedef struct MessageChat {
    char usuario[MAX_USERNAME_LENGTH];
    char mensaje[MAX_MESSAGE_LENGTH];
} MessageChat;

typedef struct HistorialChat {
    MessageChat messages[MAX_MESSAGES];
    int count;
} HistorialChat;

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


// Estructura de la tabla hash
#define TABLE_SIZE 100
User* hashTable[TABLE_SIZE];

Graph* graph;

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

//==== MAIN ====//
int main() {
    setlocale(LC_ALL, ""); // Configura la localización para soporte de caracteres especiales
    //Display_LoadingScreen();

    graph = Graph_New(TABLE_SIZE, eGraphType_UNDIRECTED);
    Start();
    Graph_Delete(&graph);

    return 0;
}
