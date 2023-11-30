#ifndef  LIST_INC
#define  LIST_INC

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>


typedef struct {
   char content[200];
   int likes;
   int shares;
   int num_comments;
   char date[20];
   char hour[20];
} Post;


typedef struct
{
    Post post;
   int   index;
   float weight;
} Data;

typedef struct Node {
   Data data;
   struct Node* next;
   struct Node* prev;
} Node;

typedef struct {
   Node* first;
   Node* last;
   Node* cursor;
} List;

List* List_New();
void List_Delete(List** p_list);
Node* new_node_post(Post* post);
Node* new_node( int index, float weight );
void List_Push_back(List* list, int index, float weight);
void List_Push_back_post(List* list, Post* post);
void List_Push_front(List* list, int index, float weight);
void List_Pop_front(List* list);
void List_Pop_back(List* list);
bool List_Is_empty(List* list);
bool List_Find(List* list, int key);
bool List_Remove(List* list, int key);
void List_Cursor_front(List* list);
bool List_Cursor_next(List* list);
bool List_Cursor_end(List* list);
Data List_Cursor_get(List* list);

#endif   /* ----- #ifndef LIST_INC  ----- */
