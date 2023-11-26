
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

/**
 * @brief Busca la primer ocurrencia con la llave key y si la encuentra coloca ah� al cursor.
 *
 * @param list Referencia a una lista.
 * @param key La llave de b�squeda.
 *
 * @return true si encontr� el elemento; false en caso contrario.
 * @post Si hubo una ocurrencia el cursor es movido a esa posici�n; en caso contrario el cursor no se mueve.
 */
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

bool List_Remove( List* list, int key )
{
   // terminar
   return false;
}

void List_Cursor_front( List* list )
{
   assert( list );

   list->cursor = list->first;
}

void List_Cursor_back( List* list )
{
   assert( list );

   list->cursor = list->last;
}

bool List_Cursor_next( List* list )
{
   assert( list );

   list->cursor = list->cursor->next;
   return list->cursor;
}

bool List_Cursor_prev( List* list )
{
   assert( list );

   // terminar
   return false;
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

/**
 * @brief Elimina el elemento apuntado por el cursor.
 *
 * @param list Referencia a una lista.
 *
 * @pre El cursor debe apuntar a una posici�n v�lida.
 * @post El cursor queda apuntando al elemento a la derecha del elemento eliminado; si
 * este hubiese sido el �ltimo, entonces el cursor apunta al primer elemento de la lista.
 */
void List_Cursor_erase( List* list );


/**
 * @brief Aplica la funci�n fn() a cada elemento de la lista. La funci�n fn() es una funci�n unaria.
 *
 * @param list Una lista.
 * @param fn Funci�n unaria que ser� aplicada a cada elemento de la lista.
 */
void List_For_each( List* list, void (*fn)( int, float ) )
{
   Node* it = list->first;
   // |it| es la abreviaci�n de "iterator", o  en espa�ol, "iterador"

   while( it != NULL )
   {
      fn( it->data.index, it->data.weight );

      it = it->next;
   }
}


