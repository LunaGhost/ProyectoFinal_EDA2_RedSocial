// Publicaciones.h
#ifndef PUBLICACIONES_H
#define PUBLICACIONES_H

#include "List.h"

#define MAX_MESSAGES 100
#define MAX_MESSAGE_LENGTH 256
#define MAX_USERNAME_LENGTH 30

#define TABLE_SIZE 100

typedef struct {
    char username[50];
    char password_hash[64];
    struct User* next;
    int graph_index;
    List* post;
    List* friends;
    List* friend_requests;
    List* pending_friends;
} User;

typedef enum
{
   eGraphType_UNDIRECTED,
   eGraphType_DIRECTED
} eGraphType;

typedef struct {
    User* users;
    int size;
    int len;
    eGraphType type;
} Graph;

typedef struct MessageChat {
    char usuario[MAX_USERNAME_LENGTH];
    char mensaje[MAX_MESSAGE_LENGTH];
} MessageChat;

typedef struct HistorialChat {
    MessageChat messages[MAX_MESSAGES];
    int count;
} HistorialChat;

User* find_user(char* username);

Graph* Graph_New(int size, eGraphType type);
void Graph_Delete(Graph** g);
void Insert_User_Graph(char* username, Graph* g);
bool Graph_AddFriendship(Graph* g, int userstart, int userfinish);
void Graph_Print(Graph* g, int depth);
void Create_Json_User(User* user);
void Read_json_post(char* file_name);
void CleanBuffer();
void Make_Publication(User* user);
void Show_Post(User* user);
void Send_FriendRequest(User* sender, User* receiver);
void Accept_FriendRequest(User* receiver, User* sender, Graph* grafo);
void Show_FriendRequest(User* user, Graph* grafo);
void chat(User* stored_user, User* destinatary_user, Graph* graph, HistorialChat* historial);
void Show_Profile(User* user);
void Display_LoadingScreen();
int verify_password(User* user, char* password);
int hash(char* str);
void addMessage(HistorialChat* historial, const char* usuario, const char* mensaje);
void printHistorial(HistorialChat* historial);
void DenyFriendRequest(User* receiver, User* sender);
void Accept_FriendRequest(User* receiver, User* sender, Graph* graph);
void handle_option_chat(User* stored_user, User* destinatary_user, Graph* grafo);
int Start();

#endif // PUBLICACIONES_H
