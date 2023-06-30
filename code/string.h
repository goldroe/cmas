#ifndef STRING_H
#define STRING_H

#include <stdlib.h>

class String;

String str_make(const char *cstr);
bool str_equals(String str1, String str2);
void str_append(String str1, String str2);

class String {
public:
    char *text;
    size_t length;

    String () {
        text = nullptr;
        length = 0;
    }
    
    void push(char c) {
        length++;
        text = (char *)realloc(text, (length + 1) * sizeof(char));
        text[length - 1] = c;
        text[length] = '\0';
    }
    
    void operator +=(char c) {
        push(c);
    }
    bool operator ==(const char *s) {
        return str_equals(*this, str_make(s));
    }
    bool operator ==(String s) {
        return str_equals(*this, s);
    }
    char operator [](size_t index) {
        return text[index];
    }
};

#endif // STRING_H
