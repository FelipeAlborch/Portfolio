#include <fun.h>

#define NULL ((void*)0)

int list_find_first(t_list *self, bool (*condition)(void*), t_link_element **elem) {
    t_list_iterator *iterator = list_iterator_create(self);
    while (list_iterator_has_next(iterator)) {
        void *data = list_iterator_next(iterator);
        if (condition(data)) {
            int index = list_iterator_index(iterator);
            if (elem != NULL) *elem = *iterator->actual;
            list_iterator_destroy(iterator);
            return index;
        }
    }
    list_iterator_destroy(iterator);
    return -1;
}
