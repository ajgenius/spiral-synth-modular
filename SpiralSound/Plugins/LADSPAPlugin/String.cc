#include <iostream>
#include <string.h>
#include <stdarg.h>
#include "String.h"
//#include "Exceptions.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if 0

char String::t[BUFSIZ];

String::String(const String & s) : str(strdup(s.str)) { }

String::String(char c) {
    char t[2];

    sprintf(t, "%c", c);
    str = strdup(t);
}

String::String(const char * s) : str(s ? strdup(s) : NULL) { } 

String::String(int i) {
    char t[20];

    sprintf(t, "%i", i);
    str = strdup(t);
}

String::String(double d) {
    char t[30];

    sprintf(t, "%g", d);
    str = strdup(t);
}

String::~String() {
    free(str);
}

char * String::set(char * s, ...) {
    va_list ap;

    va_start(ap, s);
    vsprintf(t, s, ap);
    free(str);
    str = strdup(t);
    va_end(ap);
    return t;
}

char * String::to_charp(size_t from, ssize_t to) const {
    if (to < 0) {
	strcpy(t, &(str[from]));
    } else {
	if (to >= strlen()) {
	    to = strlen() - 1;
	}
	if (to >= from) {
	    int i;
	    for (i = 0; i <= to - from; i++) {
		t[i] = str[i + from];
	    }
	    t[i] = '\0';
	} else {
	    t[0] = '\0';
	}
    }
    return t;
}

int String::to_int(void) const {
    int r;
    
    sscanf(str, "%i", &r);
    return r;
}

double String::to_double(void) const {
    double r;
    
    sscanf(str, "%lf", &r);
    return r;
}

string String::to_string(void) const {
    return string(str);
}

String & String::operator=(const String & s) {
    if (str != s.str) {
	// On évite l'autodestruction...
	free(str);
	str = strdup(s.str);
    }
    return *this;
}

String String::operator+(const String & s) const {
    strcpy(t, str);
    strcat(t, s.str);
    return String(t);
}

String & String::operator+=(const String & s) {
    strcpy(t, str);
    strcat(t, s.str);
    free(str);
    str = strdup(t);
    return (*this);
}

ostream & operator<<(ostream & os, const String & s) {
    return (os << s.to_charp());
}

istream & operator>>(istream & is, String & s) {
    char c = 0;

    s.set("");
    
    while (!is.eof() && (c != '\n') && (c != '\r')) {
	is >> c;
	s += c;
    }
    
    return is;
}

bool String::operator!=(const String & s) const {
    return (strcmp(str, s.str) != 0);
}

bool String::operator==(const String & s) const {
    return (strcmp(str, s.str) == 0);
}

bool String::operator<=(const String & s) const {
    return (strcmp(str, s.str) <= 0);
}

bool String::operator>=(const String & s) const {
    return (strcmp(str, s.str) >= 0);
}

bool String::operator<(const String & s) const {
    return (strcmp(str, s.str) < 0);
}

bool String::operator>(const String & s) const {
    return (strcmp(str, s.str) > 0);
}

size_t String::strlen() const {
    return (str ? ::strlen(str) : 0);
}

char String::operator[](size_t i) const {
    if (i >= strlen()) {
	return 0;
    } else {
        return str[i];
    }
}

ssize_t String::strchr(char c, size_t from) const {
    size_t s = strlen();

    for (size_t i = from; i < s; i++) {
	if (str[i] == c) return i;
    }
    
    return -1;
}

ssize_t String::strrchr(char c) const {
    size_t s = strlen();
    
    for (size_t i = s - 1; i >= 0; i--) {
	if (str[i] == c) return i;
    }
    
    return -1;
}

ssize_t String::strstr(const String & s) const {
    char * p = ::strstr(str, s.str);
    
    if (p) {
        return p - str;
    } else {
	return -1;
    }
}

int String::strchrcnt(char c) const {
    size_t i, cnt = 0;
    size_t s = strlen();
    
    for (i = 0; i < s; i++) {
	if (str[i] == c) cnt++;
    }
    
    return cnt;
}

String String::to_sqldate(void) const {
/* DD/MM/YYYY ==> YYYYMMMDD */
    return (is_date() ? String(to_charp(6, 9)) + to_charp(3, 4) + to_charp(0, 1) : "");
}

String String::to_sqltime(void) const {
/* h:m ==> h * 60 + m */
    int p = strchr(':');
    return (is_time() ? String(String(to_charp(0, p - 1)).to_int() * 60 + String(to_charp(p + 1)).to_int()) : "");
}

String String::from_sqldate(void) const {
/* YYYYMMDD ==> DD/MM/YYYY */
    return ((strlen() == 8) && is_number() ? String(to_charp(6, 7)) + '/' + to_charp(4, 5) + '/' + to_charp(0, 3) : "");
}

String String::from_sqltime(void) const {
/* t ==> (t / 60):(t % 60) */
    int t = to_int();
    return (is_number() ? String((int) (t / 60)) + ':' + (t % 60) : "");
}

bool String::is_date(void) const {
/* 'DD/MM/YYYY'
    0123456789 */

    if (strlen() != 10) return false;
    if ((str[2] != '/') || (str[5] != '/') ||
        (!String(to_charp(0, 1)).is_number()) ||
        (!String(to_charp(3, 4)).is_number()) ||
        (!String(to_charp(6, 9)).is_number())) {
	return false;
    }

    return true;
}

bool String::is_number(void) const {
    size_t s = strlen();
    
    
    for (size_t i = ((str[i] == '-') ? 1 : 0); i < s; i++) {
	if ((str[i] > '9') || (str[i] < '0')) return false;
    }
    return true;
}

bool String::is_float(void) const {
    size_t s = strlen();
    bool seendot = false;
    
    for (size_t i = ((str[i] == '-') ? 1 : 0); i < s; i++) {
	if ((str[i] > '9') || (str[i] < '0')) {
	    if ((str[i] == '.') && !seendot) {
		seendot = true;
	    } else {
		return false;
	    }
	}
    }
    
    return true;
}

bool String::is_time(void) const {
    int p = strchr(':');
    
    if (p == -1) return false;
    
    // On accepte les heures sous le format xxxxxx:yy pour pouvoir indiquer des durées.
    
    if ((!String(to_charp(0, p - 1)).is_number()) || (!String(to_charp(p + 1)).is_number()))
        return false;

    return (String(to_charp(p + 1)).to_int() < 60) ? true : false;
}
#endif
