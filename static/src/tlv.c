#include <tlv.h>
#include <stdlib.h>
#include <string.h>

t_tlv *tlv_create() {
    t_tlv *tlv = malloc(sizeof(t_tlv));
    tlv->type = NOTSET;
    tlv->length = 0;
    tlv->value = NULL;
    return tlv;
}

void tlv_destroy(t_tlv *tlv) {
    free(tlv->value);
    free(tlv);
}

void *tlv_serialize(t_tlv *tlv) {
    void *buffer = malloc(sizeof(t_type) + sizeof(int) + tlv->length);
    int offset = 0;
    memcpy(buffer + offset, &tlv->type, sizeof(t_type));
    offset += sizeof(t_type);
    memcpy(buffer + offset, &tlv->length, sizeof(int));
    offset += sizeof(int);
    memcpy(buffer + offset, tlv->value, tlv->length);
    return buffer;
}

t_tlv *tlv_deserialize(void *buffer) {
    t_tlv *tlv = malloc(sizeof(t_tlv));
    memcpy(&tlv->type, buffer, sizeof(t_type));
    buffer += sizeof(t_type);
    memcpy(&tlv->length, buffer, sizeof(int));
    buffer += sizeof(int);
    tlv->value = malloc(tlv->length);
    memcpy(tlv->value, buffer, tlv->length);
    buffer += tlv->length;
    return tlv;
}

int tlv_get_int(t_tlv *tlv) {
    return *(int *)tlv->value;
}

double tlv_get_double(t_tlv *tlv) {
    return *(double *)tlv->value;
}

char *tlv_get_string(t_tlv *tlv) {
    return (char *)tlv->value;
}