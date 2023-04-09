#include <tlv.h>

t_tlv *tlv_create() {
    t_tlv *tlv = malloc(sizeof(t_tlv));
    tlv->type = NOTSET;
    tlv->length = 0;
    tlv->value = NULL;
    return tlv;
}

t_list *tlv_list_create() {
    return list_create();
}

void tlv_destroy(t_tlv *tlv) {
    free(tlv->value);
    free(tlv);
}

void tlv_list_destroy(t_list *tlv_list) {
    list_destroy_and_destroy_elements(tlv_list, (void *)tlv_destroy);
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

t_tlv *tlv_create_int(int value) {
    t_tlv *tlv = tlv_create();
    tlv->type = INTEGER;
    tlv->length = sizeof(int);
    tlv->value = malloc(tlv->length);
    memcpy(tlv->value, &value, tlv->length);
    return tlv;
}

int tlv_get_int(t_tlv *tlv) {
    return *(int *)tlv->value;
}

t_tlv *tlv_create_double(double value) {
    t_tlv *tlv = tlv_create();
    tlv->type = DOUBLE;
    tlv->length = sizeof(double);
    tlv->value = malloc(tlv->length);
    memcpy(tlv->value, &value, tlv->length);
    return tlv;
}

double tlv_get_double(t_tlv *tlv) {
    return *(double *)tlv->value;
}

t_tlv *tlv_create_string(char *string) {
    t_tlv *tlv = tlv_create();
    tlv->type = STRING;
    tlv->length = strlen(string) + 1;
    tlv->value = malloc(tlv->length);
    memcpy(tlv->value, string, tlv->length);
    ((char *)tlv->value)[tlv->length - 1] = '\0';

    return tlv;
}

char *tlv_get_string(t_tlv *tlv) {
    if (((char *)tlv->value)[tlv->length - 1] != '\0') {
        tlv->value = realloc(tlv->value, (++tlv->length) * sizeof(char));
        ((char *)tlv->value)[tlv->length - 1] = '\0';
    }
    return (char *)tlv->value;
}

t_tlv *tlv_create_string_map(t_dictionary *map) {
    t_tlv *tlv = tlv_create();
    tlv->type = MAP;
    tlv->length = 0;
    tlv->value = malloc(tlv->length);
    t_list *keys = dictionary_keys(map);
    t_list *values = dictionary_elements(map);
    int key_len, value_len;
    for (int i = 0; i < list_size(keys); i++) {
        char *key = list_get(keys, i);
        char *value = list_get(values, i);
        key_len = strlen(key);
        value_len = strlen(value);
        tlv->value = realloc(tlv->value, tlv->length + sizeof(int) + key_len + 1 + sizeof(int) + value_len + 1);
        memcpy(tlv->value + tlv->length, &key_len, sizeof(int));
        tlv->length += sizeof(int);
        memcpy(tlv->value + tlv->length, key, key_len + 1);
        tlv->length += key_len + 1;
        memcpy(tlv->value + tlv->length, &value_len, sizeof(int));
        tlv->length += sizeof(int);
        memcpy(tlv->value + tlv->length, value, value_len + 1);
        tlv->length += value_len + 1;
    }
    list_destroy(keys);
    list_destroy(values);

    return tlv;
}

t_dictionary *tlv_get_string_map(t_tlv *tlv) {
    t_dictionary *map = dictionary_create();
    int offset = 0;
    int key_len, value_len;
    char *key, *value;
    while (offset < tlv->length) {
        memcpy(&key_len, tlv->value + offset, sizeof(int));
        offset += sizeof(int);
        key = malloc(key_len + 1);
        memcpy(key, tlv->value + offset, key_len + 1);
        offset += key_len + 1;
        memcpy(&value_len, tlv->value + offset, sizeof(int));
        offset += sizeof(int);
        value = malloc(value_len + 1);
        memcpy(value, tlv->value + offset, value_len + 1);
        offset += value_len + 1;
        dictionary_put(map, key, value);
        free(key);
        free(value);
    }
    return map;
}
