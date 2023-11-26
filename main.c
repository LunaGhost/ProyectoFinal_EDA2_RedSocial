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

typedef struct MensajesChat {
    char usuario[MAX_USERNAME_LENGTH];
    char mensaje[MAX_MESSAGE_LENGTH];
} MensajesChat;

typedef struct HistorialChat {
    MensajesChat messages[MAX_MESSAGES];
    int count;
} HistorialChat;

void ImprimirHistorial(HistorialChat *historial ) {
    for (int i = 0; i < historial ->count; i++) {
        printf("%s: %s\n", historial ->messages[i].usuario, historial ->messages[i].mensaje);
    }
}

void anadirMensaje(HistorialChat *historial , const char *usuario, const char *mensaje) {
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

Graph* grafo;

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
void insert_user(char *username, char *password, Graph* g) {
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
    Insert_User_Graph(username, g);
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
void desplegar_pantalla_carga(){
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

    printf(" _\n|_) o  _ __    _ __  o  _| _\n|_) | (/_| |\_/(/_| | | (_|(_)");
    printf("\n\n             _\n            (_|\n\n");
    printf("nombre red social");

    system("sleep 2");
    system("cls");
}

void mostrar_menu() {
    printf("\n===  BIENVENIDO  ===\n");
    printf("1. Iniciar sesión\n");
    printf("2. Crear cuenta\n");
    printf("3. Salir\n");
    printf("====================\n");
}

// Función para mostrar el perfil de un usuario
void mostrar_perfil(User *user) {
    printf("=== Perfil de %s ===\n", user->username);
}

void chat(User *stored_user) {
    HistorialChat historial;
    historial.count = 0;

    char usuario[MAX_USERNAME_LENGTH];

    printf("Introduce tu usuario: ");
    getchar(); // Limpiar búfer del teclado
    fgets(usuario, MAX_USERNAME_LENGTH, stdin);
    usuario[strcspn(usuario, "\n")] = '\0'; // Eliminar el carácter de nueva línea

    printf("Bienvenido al chat, %s!\n", usuario);

    while (1) {
        char mensaje[MAX_MESSAGE_LENGTH];

        printf("Introduce tu mensaje (escribe 'salir' para terminar el chat): ");
        fgets(mensaje, MAX_MESSAGE_LENGTH, stdin);
        mensaje[strcspn(mensaje, "\n")] = '\0'; // Eliminar el carácter de nueva línea

        if (strcmp(mensaje, "salir") == 0) {
            break;
        }

        anadirMensaje(&historial, usuario, mensaje);

        printf("\nHistorial de Chat:\n");
        ImprimirHistorial(&historial);
        printf("\n");
    }

    printf("Saliendo del Chat. Adiós, %s!\n", usuario);
}

// Función para manejar la opción de chat
void manejar_opcion_chat(User *stored_user) {
    chat(stored_user);
}

int iniciar(){
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
        limpiarBuffer();
        User *stored_user;
        switch (choice) {
            case 1:
                printf("\n=== Iniciar sesi�n ===\n");
                printf("Usuario: ");
                scanf("%s", username);
                printf("Contrase�a: ");
                scanf("%s", password);
                limpiarBuffer();
                stored_user = find_user(username);
                if (stored_user != NULL && verify_password(stored_user, password)) {
                    int loggedIn = 1; // Bandera para rastrear el estado de inicio de sesi�n
                    while (loggedIn) {
                        system("cls");

                        cuenta_abierta:
                        mostrar_perfil(stored_user);
                        printf("\n1. Realizar publicacion\n2. Ver publicaciones de feed\n3. Ver perfil personal\n4. Ver solicitudes de amistad\n5.Agregar amigo\n6. Chat\n7. Cerrar sesi�n\n8.Salir");
                        printf("Elija una opci�n: ");
                        
                        scanf("%d", &choice);
                        switch (choice) {
                            case 1:
                                //Publicar
                                Realizar_Publicacion(stored_user);
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
                                mostrar_perfil(stored_user);
                                Mostrar_Publicaciones(stored_user);
                                //goto cuenta_abierta;
                                system("cls");
                                break;
                            case 4:
                                //Agregar amigo
                                //MostrarSolicitudesAmistad(stored_user,grafo);
                                system("cls");
                                break;
                            case 5:
                                /*Se envia solicitud de amistad
                                printf("Ingrese el nombre de usuario al que desea enviar una solicitud de amistad: ");
                                char friend_username[50];
                                scanf("%s", friend_username);
                                User* friend_user = find_user(friend_username);
                                if (friend_user != NULL) {
                                    SendFriendRequest(stored_user, friend_user, grafo);
                                    // Aquí puedes mostrar un mensaje de confirmación
                                } else {
                                    printf("Usuario no encontrado.\n");
                                    // Aquí puedes mostrar un mensaje de error
                                }*/
                                system("cls");
                                break;
                            case 6:
                                //Chat
                                manejar_opcion_chat(stored_user);
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
                printf("Nueva contrase�a: ");
                scanf("%s", password);

                if (find_user(username) == NULL) {
                    insert_user(username, password, grafo);
                    printf("Cuenta creada con Exito para %s.\n", username);
                    system("sleep 0.1");
                    system("cls");
                    stored_user = find_user(username);
                    goto cuenta_abierta;
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
    //desplegar_pantalla_carga();

    grafo = Graph_New(TABLE_SIZE, eGraphType_UNDIRECTED);
    iniciar();
    Graph_Delete(&grafo);

    return 0;
}
