#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> // Incluye la biblioteca para soporte de caracteres especiales

//== ESTRUCTURAS ==//

// Estructura de usuario
typedef struct User {
    char username[50];
    char password_hash[64]; // Hash de contraseña (SHA-256)
    struct User* next;
} User;

// Estructura de la tabla hash
#define TABLE_SIZE 100
User* hashTable[TABLE_SIZE];

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
void insert_user(char *username, char *password) {
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
        Sleep(75);
    }
    printf("\nCarga completa!");

    // Espera 1 segundos antes de borrar la pantalla
    Sleep(1000);
    system("cls");

    printf(" _\n|_) o  _ __    _ __  o  _| _\n|_) | (/_| |\_/(/_| | | (_|(_)");
    printf("\n\n             _\n            (_|\n\n");
    printf("nombre red social");

    Sleep(2000);
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

void iniciar(){
    char username[50];
    char password[50];
    int choice;

    while (1) {
        printf("\n=== BIENVENIDO ===\n");
        printf("1. Iniciar sesión\n");
        printf("2. Crear cuenta\n");
        printf("3. Salir\n");
        printf("====================\n");
        printf("Elija una opción: ");
        scanf("%d", &choice);

        User *stored_user;
        switch (choice) {
            case 1:
                printf("\n=== Iniciar sesión ===\n");
                printf("Usuario: ");
                scanf("%s", username);
                printf("Contraseña: ");
                scanf("%s", password);

                stored_user = find_user(username);
                if (stored_user != NULL && verify_password(stored_user, password)) {
                    int loggedIn = 1; // Bandera para rastrear el estado de inicio de sesión
                    while (loggedIn) {
                        system("cls");

                        cuenta_abierta:
                        mostrar_perfil(stored_user);
                        printf("\n1. Cerrar sesión\n2. Salir\n");
                        printf("Elija una opción: ");
                        scanf("%d", &choice);

                        switch (choice) {
                            case 1:
                                loggedIn = 0; // Cerrar la sesión
                                system("cls");
                                break;
                            case 2:
                                printf("Gracias por usar nuestra aplicación. ¡Hasta pronto!\n");
                                return 0;
                            default:
                                printf("Opción no válida. Por favor, elija una opción válida.\n");
                        }
                    }
                } else {
                    printf("Usuario o contraseña incorrectos.\n");
                }
                break;
            case 2:
                printf("\n=== Crear cuenta ===\n");
                printf("Nuevo usuario: ");
                scanf("%s", username);
                printf("Nueva contraseña: ");
                scanf("%s", password);

                if (find_user(username) == NULL) {
                    insert_user(username, password);
                    printf("Cuenta creada con éxito para %s.\n", username);
                    Sleep(100);
                    system("cls");
                    stored_user = find_user(username);
                    goto cuenta_abierta;
                } else {
                    printf("El usuario ya existe. Intente con un nombre de usuario diferente.\n");
                }
                break;
            case 3:
                printf("Gracias por usar nuestra aplicación. ¡Hasta pronto!\n");
                return 0;
            default:
                printf("Opción no válida. Por favor, elija una opción válida.\n");
                break;
        }
    }
}

//==== MAIN ====//
int main() {
    setlocale(LC_ALL, ""); // Configura la localización para soporte de caracteres especiales
    //desplegar_pantalla_carga();

    iniciar();

    return 0;
}
