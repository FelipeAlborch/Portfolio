#ifndef TLV_H_
#define TLV_H_
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <string.h>
#include <stdlib.h>

typedef enum t_type {
    NOTSET = -1,
    INTEGER,
    DOUBLE,
    STRING,
    MAP,
    BLOCK,
} t_type;

typedef struct t_tlv {
    t_type type;
    int length;
    void* value;
} t_tlv;

t_tlv *tlv_create();

t_list *tlv_list_create();

void tlv_destroy(t_tlv *);

void tlv_list_destroy(t_list *);

void *tlv_serialize(t_tlv *);

t_tlv *tlv_deserialize(void *);

t_tlv *tlv_create_int(int value);

int tlv_get_int(t_tlv *);

t_tlv *tlv_create_double(double value);

double tlv_get_double(t_tlv *);

t_tlv *tlv_create_string(char *value);

char *tlv_get_string(t_tlv *);

t_tlv *tlv_create_string_map(t_dictionary *);

t_dictionary *tlv_get_string_map(t_tlv *);

#endif