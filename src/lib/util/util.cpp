#include <avr/io.h>

void uint16_to_str(uint16_t n, char* buf) {
    uint16_t i = 0;
    char tmp[6];

    if (n == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    // estrai le cifre al contrario
    while (n > 0) {
        tmp[i++] = '0' + (n % 10);
        n /= 10;
    }

    // invertile nel buffer finale
    uint16_t j = 0;
    while (i > 0) {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';
}

void str_concat(char* dst, const char* left, const char* right) {
    while (*left != '\0') *dst++ = *left++;
    while (*right != '\0') *dst++ = *right++;
    *dst = '\0';
}