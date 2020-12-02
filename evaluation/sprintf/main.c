#include <stdio.h>

#include "smal_pow.h"
#include "smal_sprintf.h"

static void putch(const char ascii)
{
    fputc(ascii, stdout);
}


int
main(int ac, char **av)
{
    int a = 12345, b = 987654;
    char C = 'C';
    const char *txt="chat put acbde";
    size_t n;

    double d[] = {
        0.0,
        42.0,
        1234567.89012345,
        0.000000000000018,
        555555.55555555555555555,
        -888888888888888.8888888,
        111111111111111111111111.2222222222
    };

    smal_sprintf_init(putch);

#if 1
    for(n=0;n<100;n++) {
        smal_printf("%d %c %d %s %d\n", a, n, b, txt, a);
        smal_printf("%f %f %f\n", d[0], d[1], d[2]);
    }
#endif

#if 0
    for(i=-10;i<50;i++) {
	printf("%d:%40.10f\n", i, smal_ipow10(i));
    }
#endif

    return 0;
}
