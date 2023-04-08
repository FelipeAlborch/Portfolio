#ifndef TLV_H_
#define TLV_H_

typedef enum t_type {
    NOTSET = -1,
    INTEGER,
    DOUBLE,
    STRING
} t_type;

typedef struct t_tlv {
    t_type type;
    int length;
    void* value;
} t_tlv;

t_tlv *tlv_create();

void tlv_destroy(t_tlv *);

void *tlv_serialize(t_tlv *);

t_tlv *tlv_deserialize(void *);

int tlv_get_int(t_tlv *);

double tlv_get_double(t_tlv *);

char *tlv_get_string(t_tlv *);

#endif