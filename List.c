
#include "List.h"

static Node* new_node( int index, float weight )
{
   Node* n = (Node*) malloc( sizeof( Node ) );
   if( n != NULL )
   {
      n->data.index = index;
      n->data.weight = weight;

      n->next = NULL;
      n->prev = NULL;
   }

   return n;
}

static Node* new_node_post(Post* post) {
   Node* n = (Node*)malloc(sizeof(Node));
   if (n != NULL) {
      n->data.post = *post;
      n->next = NULL;
      n->prev = NULL;
   }
   return n;
}

List* List_New()
{
   List* lst = (List*) malloc( sizeof(List) );
   if( lst )
   {
      lst->first = lst->last = lst->cursor = NULL;
   }

   return lst;
}

void List_Delete( List** p_list )
{
   assert( *p_list );

   while( (*p_list)->first )
   {
      List_Pop_back( *p_list );
   }

   free( *p_list );
   *p_list = NULL;
}

void List_Push_back( List* list, int data, float weight )
{
   assert( list );

   Node* n = new_node( data, weight );
   assert( n );

   if( list->first != NULL )
   {
      list->last->next = n;
      n->prev = list->last;
      list->last = n;
   }
   else
   {
      list->first = list->last = list->cursor = n;
   }
}

void List_Push_back_post( List* list, Post* post){
    assert(list);

   Node* n = new_node_post(post);
   assert(n);

   if (list->first != NULL) {
      list->last->next = n;
      n->prev = list->last;
      list->last = n;
   } else {
      list->first = list->last = list->cursor = n;
   }
}

void List_Pop_back( List* list )
{
   assert( list );
   assert( list->first );
   // ERR: no podemos borrar nada de una lista vac�a

   if( list->last != list->first )
   {
      Node* x = list->last->prev;
      free( list->last );
      x->next = NULL;
      list->last = x;
   }
   else
   {
      free( list->last );
      list->first = list->last = list->cursor = NULL;
   }

}


bool List_Is_empty( List* list )
{
   //assert( list );

   return !list->first;
}

bool List_Find( List* list, int key )
{
   assert( list );

   Node* start = list->first;
   while( start )
   {
      if( start->data.index == key )
      {
         list->cursor = start;
         return true;
      }

      start = start->next;
   }
   return false;
}

//Funcion completada
bool List_Remove(List* list, int key) {
    assert(list);

    Node* current = list->first;

    //La lista está vacía
    if (!current) {
        return false;
    }

    // Elemento al principio de la lista
    if (current->data.index == key) {
        list->first = current->next;
        if (list->last == current) {
            // Actualizar el puntero al último nodo si es necesario
            list->last = current->next;
        }
        free(current);
        return true;
    }

    // Caso general
    while (current->next) {
        if (current->next->data.index == key) {
            Node* temp = current->next;
            current->next = temp->next;
            if (temp == list->last) {
                list->last = current;
            }
            free(temp);
            return true;
        }
        current = current->next;
    }

    return false;
}

void List_Cursor_front( List* list )
{
   assert( list );

   list->cursor = list->first;
}

bool List_Cursor_next( List* list )
{
   assert( list );

   list->cursor = list->cursor->next;
   return list->cursor;
}

bool List_Cursor_end( List* list )
{
   assert( list );

   return list->cursor == NULL;
}

Data  List_Cursor_get( List* list )
{
   assert( list );
   assert( list->cursor );

   return list->cursor->data;
}
