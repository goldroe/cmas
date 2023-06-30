#include "string.h"

#include <stdlib.h>

size_t str_len(const char *cstr) {
    if (cstr == nullptr) return 0;
    size_t len = 0;
    while (*cstr) {
        cstr++;
        len++;
    }
    return len;
}

String str_make(const char *cstr) {
    String result;
    result.length = str_len(cstr);
    result.text = (char *)calloc((result.length + 1), sizeof(char));
    for (size_t i = 0; i < result.length; i++) {
        result.text[i] = cstr[i];
    }
    return result;
}

bool str_equals(String str1, String str2) {
    if (str1.length != str2.length) return false;
    for (size_t i = 0; i < str1.length; i++) {
        if (str1[i] != str2[i]) return false;
    }
    return true;
}

void str_append(String str1, String str2) {
    size_t new_len = str1.length + str1.length;
    str1.text = (char *)realloc(str1.text, (new_len + 1) * sizeof(char));
    for (size_t i = 0; i < str2.length; i++) {
        str1.text[i + str1.length] = str2[i];
    }
    str1.text[new_len] = '\0';
    str1.length = new_len;
}
