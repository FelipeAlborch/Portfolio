#ifndef FUN_H_
#define FUN_H_
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

/**
 * @NAME: list_find_elem
 * @DESC: Busca un elemento en una lista que cumpla con la condición dada.
 * @PARAMS:
 * - self: lista en la que se buscará el elemento.
 * - condition: función que determina si un elemento cumple con la condición.
 * - elem: puntero a un elemento de la lista que cumple con la condición.
 * @RETURN: el índice del elemento en la lista, o -1 si no se encontró.
 * @OBSERVATIONS: la función condition debe tener la siguiente firma:
 * bool condition(void* element);
 */
int list_find_first(t_list *self, bool (*condition)(void*), t_link_element **elem);

#endif