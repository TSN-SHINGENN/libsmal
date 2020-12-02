/**
 *	Copyright 2020 TSN-SHINGENN All Rights Reserved.
 *	Basic Author: Seiichi Takeda  '2014-March-01 Active
 */

#if 1 /* for WIN32 */
#pragma warning(disable:4996)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "smal_stdint.h"
#include "smal_pow.h"
#include "smal_sprintf.h"

#define _isnumc(x) ( (x) >= '0' && (x) <= '9' )
#define _ctoi(x)   ( (x) -  '0' )

#define ZERO_PADDING         (1<<1)
#define ALTERNATIVE          (1<<2)
#define THOUSAND_GROUP       (1<<3)
#define CAPITAL_LETTER       (1<<4)
#define WITH_SIGN_CHAR       (1<<5)
#define LEFT_JUSTIFIED       (1<<6)

typedef enum _enum_INTEGER {
    LL =  2,    //Long Long
    L  =  1,    //Long
    I  =  0,    //Int
    S  = -1,    //Short
    C  = -2     //Char
} enum_INTEGER_t;

static smal_llong_t get_signed(va_list *const , const enum_INTEGER_t);
static smal_ullong_t get_unsigned(va_list *const , const enum_INTEGER_t);
static void put_integer(void (*__putc)(char), const smal_ullong_t v, const smal_int_t radix, const smal_int_t put_len, const smal_char_t sign, const uint8_t flags);
static void(*_this_out_method_cb)(const char);
static void put_float(void (*__putc)(char), const float f, const smal_int_t put_len, const smal_char_t sign, const uint8_t flags);
static size_t modp_dtoa(smal_char_t *const buf, const size_t bufsz, const float f);

static smal_ullong_t get_unsigned(va_list *const ap_p, const enum_INTEGER_t type)
{
    static enum_INTEGER_t t;

    t = type;
    if(t >=  LL) {
	t = LL;
    } else if(t <= C) {
	t = C;
    }
    switch(t) {
	case LL: return (smal_ullong_t)va_arg(*ap_p, smal_ullong_t); break;
	case  L: return (smal_ullong_t)va_arg(*ap_p, smal_ulong_t);      break;
	case  I: return (smal_ullong_t)va_arg(*ap_p, smal_uint_t);       break;
	case  S: return (smal_ullong_t)va_arg(*ap_p, smal_ushort_t); break;
	case  C: return (smal_ullong_t)va_arg(*ap_p, smal_uchar_t);  break;
    }

    return (smal_ullong_t) va_arg(*ap_p, smal_uint_t);
}


static smal_llong_t get_signed(va_list *const ap_p, const enum_INTEGER_t type)
{
    static enum_INTEGER_t t;

    t = type;
    if(t >=  LL) {
	t = LL;
    } else if(t <=   C) {
	t = C;
    }

    switch(t) {
	case LL: return (smal_llong_t)va_arg(*ap_p, smal_llong_t); break;
	case  L: return (smal_llong_t)va_arg(*ap_p, smal_long_t);      break;
	case  I: return (smal_llong_t)va_arg(*ap_p, smal_int_t);       break;
	case  S: return (smal_llong_t)va_arg(*ap_p, smal_short_t); break;
	case  C: return (smal_llong_t)va_arg(*ap_p, smal_char_t);  break;
    }

    return (smal_llong_t) va_arg(*ap_p, smal_int_t);
}

#ifdef USE_FLOAT_FORMAT
static double PRECISION = 0.00001;
/**
 * Double to ASCII
 */
static size_t ftoa(char *const buf, const size_t bufsz, const float f)
{
    static char *c;
    static double n;
    static size_t l;

    c = buf;
    n = f;
    l = 0;

    // handle special cases
    if (isnan(n)) {
        strcpy(c, "nan");
	return 3;
    } else if (isinf(n)) {
        strcpy(c, "inf");
	return 3;
    } else if (n == 0.0) {
        strcpy(c, "0");
	return 1;
    } else {
        static int digit, m, m1;
        static int useExp;
        static uint8_t neg;
        neg = (n < 0);
        if (neg) {
            n = -n;
	}
        // calculate magnitude
        m = log10(n);
        useExp = (m >= 14 || (neg && m >= 9) || m <= -9);

	if (neg) {
            *(c++) = '-';
	    l++;
	}
        // set up for scientific notation
        if (useExp) {
            if (m < 0) {
               m -= 1.0;
	    }
            // n = n / pow(10.0, m);
            n = n / smal_ipow10(m);
            m1 = m;
            m = 0;
        }
        if (m < 1.0) {
            m = 0;
        }
        // convert the number
        while (n > PRECISION || m >= 0) {
            // double weight = pow(10.0, m);
            static double weight;
            weight = smal_ipow10(m);
            if (weight > 0 && !isinf(weight)) {
                digit = floor(n / weight);
                n -= (digit * weight);
                *(c++) = '0' + digit;
		l++;
            }
            if (m == 0 && n > 0) {
                *(c++) = '.';
		l++;
	    }
            m--;
        }
        if (useExp) {
            // convert the exponent
            static int i, j;
            *(c++) = 'e';
            if (m1 > 0) {
                *(c++) = '+';
		l++;
            } else {
                *(c++) = '-';
		l++;
                m1 = -m1;
            }
            m = 0;
            while (m1 > 0) {
                *(c++) = '0' + m1 % 10;
		l++;
                m1 /= 10;
                m++;
            }
            c -= m;
	    l -= m;
            for (i = 0, j = m-1; i<j; i++, j--) {
                // swap without temporary
                c[i] ^= c[j];
                c[j] ^= c[i];
                c[i] ^= c[j];
            }
            c += m;
	    l += m;
        }
        *(c) = '\0';
    }
    return l;
}

static void put_float(void (*__putc)(char), const float f, const smal_int_t put_len, const smal_char_t sign, const uint8_t flags);
{
    static char fstr[80];
    static size_t i, n;
    static float num;

    i = ftoa(fstr, 80, f);
    for(n=0; ( i > 0 );) {
	i--;
	__putc(fstr[n]);
	++n;
    }

    return;
}
#endif /* end of USE_FLOAT_FORMAT */


static void put_integer(void (*__putc)(char), const smal_ullong_t v, const smal_int_t radix, const smal_int_t put_len, const smal_char_t sign, const uint8_t flags)
{
    static const char *const symbols_s = "0123456789abcdef";
    static const char *const symbols_c = "0123456789ABCDEF";
    static char buf[32];
    static int i, length;
    static char pad = ' ';
    static const char *symbols;
    static smal_ullong_t n;

    /* initialize */
    i=0;
    n=v;
    pad = ' ';
    symbols = symbols_s;
    length = put_len;

    if(flags & CAPITAL_LETTER) {
	symbols = symbols_c;
    }
    do {
        buf[i++] = symbols[n % radix];
        if( (flags & THOUSAND_GROUP) && (i%4)==3) {
	    buf[i++] = ',';
	}
    } while (n /= radix);

    length -= i;

    if (!(flags & LEFT_JUSTIFIED)) {
        if(flags & ZERO_PADDING) {
	    pad = '0';
	}
        while (length > 0) {
	    length--;
	    buf[i++] = pad;
	}
    }

    if (sign && radix == 10) {
	buf[i++] = sign;
    }
    if (flags & ALTERNATIVE) {
        if (radix == 8) {
	    buf[i++] = '0';
	} else if (radix == 16) {
           buf[i++] = 'x';
           buf[i++] = '0';
        }
    }

    while ( i > 0 ) {
	i--;
	__putc(buf[i]);
    }
    while ( length > 0 ) {
	length--;
	__putc(pad);
    }

    return;
}

/**
 * @fn char * smal_strchr(const char *const s, const int c)
 * @brief 文字列中の文字の位置を特定する 
 * @param s 文字列ポインタ
 * @param c 検索文字
 * @retval NULL 見つからない
 * @retval NULL以外　一致した文字列のポインタ
 **/
char * smal_strchr(const char *const s, const int c)
{
    static const char *r;

    /* initalize */
    r = s;

    while (*r) {
        if (*r == (char)c) {
            return (char *)r;
	}
        r++;
    }
    return NULL;
}

/**
 * @fn void smal_vprintf(void (*__putc)(char), const char *const fmt, const va_list va)
 * @brief va_listでのprintf
 */
void smal_vprintf(void (*__putc)(char), const char *const fmt, const va_list va)
{
    static smal_ullong_t ui;
    static smal_llong_t i;
    static smal_int_t length;
    static smal_int_t precision;
    static enum_INTEGER_t int_type;
    static va_list ap;
    static uint8_t flags;
    static char sign;
    static const char *f_p;

    va_copy(ap, va);
    for (f_p=fmt, sign=0, flags=0, length=0, precision=0, int_type=0;;) {

        while (*f_p && *f_p != '%') {
	    __putc(*f_p++);
	}
        if (*f_p == '\0') { 
	    va_end(ap);
	    break;
	}
        f_p++;

        while (smal_strchr("'-+ #0", *f_p)) {
            switch (*f_p++) {
            case '\'': {
		     flags |= THOUSAND_GROUP;
		} break;
            case  '-': {
		     flags |= LEFT_JUSTIFIED;
		} break;
            case  '+': {
		     flags |= WITH_SIGN_CHAR;
		     sign = '+';
		} break;
            case  '#': {
		     flags |= ALTERNATIVE;
		} break;
            case  '0': {
		     flags |= ZERO_PADDING;
		} break;
            case  ' ': {
		     flags |= WITH_SIGN_CHAR;
		     sign = ' ';
		} break;
            }
        }

        if(*f_p == '*') {
	    length = va_arg(ap,int);
	    f_p++;
	} else {
	    while( _isnumc(*f_p) ) {
		length = (length*10)+_ctoi(*f_p++);
	    }
	} 

        if (*f_p == '.') {
            f_p++;
            if (*f_p == '*'){
		f_p++;
		precision = va_arg(ap, int);
	    } else { 
		while (_isnumc(*f_p) ) {
		    precision = precision * 10 + _ctoi(*f_p++);
		}
	    }
        }

        while (smal_strchr("hljzt", *f_p)) {
            switch (*f_p++) {
                  case 'h': int_type--;  break;
                  case 'l': int_type++;  break;
                  case 'j': /*intmax   : long      */
                  case 'z': /*size     : long      */
                  case 't': /*ptrdiff  : long      */
                            int_type=L;  break;
            }
        }

        switch (*f_p) {
            case 'd':
            case 'i':
                i = get_signed(&ap, int_type);
                if (i < 0) { i = -i; sign = '-'; }
                put_integer(__putc, i, 10, length, sign, flags);
                break;

            case 'u':
                ui = get_unsigned(&ap, int_type);
                put_integer(__putc,  ui, 10, length, sign, flags);
                break;

            case 'o':
                ui = get_unsigned(&ap, int_type);
                put_integer(__putc,  ui, 8, length, sign, flags);
                break;
#ifdef USE_FLOAT_FORMAT
            case 'f': {
		static float f;
                f = (float)va_arg(ap, double);
		put_float(__putc, f, length, sign, flags);
	    } break;
#endif /* end of USE_FLOAT_FORMAT */
            case 'p':
                length = sizeof(long) * 2;
                int_type = L;
                sign = 0;
                flags = ZERO_PADDING | ALTERNATIVE;
            case 'X':
                flags |= CAPITAL_LETTER;
            case 'x':
                ui = get_unsigned(&ap, int_type);
                put_integer(__putc, ui, 16, length, sign, flags);
                break;

            case 'c': 
                i = (long long)get_unsigned(&ap, C);
                __putc((char)i);
	        break;

            case 's': {
		static size_t tmp;
		static const char *s;
                s = va_arg(ap, const char *);
                if (NULL == s) {
		    s = "(null)";
		}
                tmp = strlen(s);
                if (precision && precision < tmp) {
		    tmp = precision;
		}
                length = length - (int)tmp;
                if (!(flags & LEFT_JUSTIFIED)) {
		    while ( length > 0   ) {
			length--;
			__putc(' ');
		    }
		}
		while (tmp--) {
		    __putc(*s++);
		}
		while ( length-- > 0) {
		    __putc(' ');
		}
	    } break;
	    case '%':
		__putc('%');
	        break;

	    default:
		while (*f_p != '%') {
		    f_p--;
		}
        }

        ++f_p;
    }
}

void smal_sprintf_init( void(*_putch_cb)(const char)) {
    _this_out_method_cb = _putch_cb;
}

void smal_printf(const char *const fmt, ...)
{
    static va_list ap;

    va_start(ap, fmt);
    smal_vprintf( _this_out_method_cb, fmt, ap);
    va_end(ap);

    return;
}

