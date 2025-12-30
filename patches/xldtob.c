#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wlogical-op-parentheses"

#include "patches.h"
#include "string.h"
#include "xstdio.h"

typedef	struct LDIV_T {
	long quot;
	long rem;
} ldiv_t;

ldiv_t ldiv(long num, long denom) {
    ldiv_t ret;

    ret.quot = num / denom;
    ret.rem = num - denom * ret.quot;

    if (ret.quot < 0 && ret.rem > 0) {
        ret.quot += 1;
        ret.rem -= denom;
    }

    return ret;
}

#define BUFF_LEN 0x20

static short _Ldunscale(short* pex, ldouble* px);
static void _Genld(_Pft* px, char code, unsigned char* p, short nsig, short xexp);

static const ldouble pows[] = {10e0L, 10e1L, 10e3L, 10e7L, 10e15L, 10e31L, 10e63L, 10e127L, 10e255L};

// float properties
#define _D0 0
#define _DBIAS 0x3ff
#define _DLONG 1
#define _DOFF 4
#define _FBIAS 0x7e
#define _FOFF 7
#define _FRND 1
#define _LBIAS 0x3ffe
#define _LOFF 15
// integer properties
#define _C2 1
#define _CSIGN 1
#define _ILONG 0
#define _MBMAX 8
#define NAN 2
#define INF 1
#define FINITE -1
#define _DFRAC ((1 << _DOFF) - 1)
#define _DMASK (0x7fff & ~_DFRAC)
#define _DMAX ((1 << (15 - _DOFF)) - 1)
#define _DNAN (0x8000 | _DMAX << _DOFF | 1 << (_DOFF - 1))
#define _DSIGN 0x8000
#define _D1 1 // big-endian order
#define _D2 2
#define _D3 3

// #define ALIGN(s, align) (((unsigned int)(s) + ((align)-1)) & ~((align)-1))

void _Ldtob(_Pft* px, char code) {
    char buff[BUFF_LEN];
    char *p;
    ldouble ldval;
    short err;
    short nsig;
    short xexp;

    p = buff;
    ldval = px->v.ld;

    if (px->prec < 0) {
        px->prec = 6;
    } else if (px->prec == 0 && (code == 'g' || code == 'G')) {
        px->prec = 1;
    }

    err = _Ldunscale(&xexp, &px->v.ld);
    if (err > 0) {
        memcpy(px->s, err == 2 ? "NaN" : "Inf", px->n1 = 3);
        return;
    } else if (err == 0) {
        nsig = 0;
        xexp = 0;
    } else {
        {
            int i;
            int n;

            if (ldval < 0) {
                ldval = -ldval;
            }

            // what
            if ((xexp = xexp * 30103 / 100000 - 4) < 0) {
                n = ALIGN(-xexp, 4), xexp = -n;

                for (i = 0; n > 0; n >>= 1, i++) {
                    if (n & 1) {
                        ldval *= pows[i];
                    }
                }
            } else if (xexp > 0) {
                ldouble factor = 1;
                
                xexp &= ~3;
                
                for (n = xexp, i = 0; n > 0; n >>= 1, i++) {
                    if (n & 1) {
                        factor *= pows[i];
                    }
                }

                ldval /= factor;
            }
        }
        {
            int gen = px->prec + ((code == 'f') ? 10 + xexp : 6);
            
            if (gen > 0x13) {
                gen = 0x13;
            }
            
            for (*p++ = '0'; gen > 0 && 0 < ldval; p += 8) {
                int j;
                long lo = ldval;
                
                if ((gen -= 8) > 0) {
                    ldval = (ldval - lo) * 1e8;
                }

                for (p += 8, j = 8; lo > 0 && --j >= 0;) {
                    ldiv_t qr;
                    qr = ldiv(lo, 10);
                    *--p = qr.rem + '0', lo = qr.quot;
                }
                
                while (--j >= 0) {
                    *--p = '0';
                }
            }

            gen = p - &buff[1];

            for (p = &buff[1], xexp += 7; *p == '0'; p++) {
                --gen, --xexp;
            }

            nsig = px->prec + ((code == 'f') ? xexp + 1 : ((code == 'e' || code == 'E') ? 1 : 0));
            
            if (gen < nsig) {
                nsig = gen;
            }

            if (nsig > 0) {
                const char drop = nsig < gen && '5' <= p[nsig] ? '9' : '0';
                int n;

                for (n = nsig; p[--n] == drop;) {
                    --nsig;
                }

                if (drop == '9') {
                    ++p[n];
                }
                
                if (n < 0) {
                    --p, ++nsig, ++xexp;
                }
            }
        }
    }

    _Genld(px, code, p, nsig, xexp);
}

short _Ldunscale(short* pex, ldouble* px) {
    unsigned short* ps = (unsigned short*)px;
    short xchar = (ps[_D0] & _DMASK) >> _DOFF;


    if (xchar == _DMAX) {
        *pex = 0;

        return (ps[_D0] & _DFRAC) || ps[_D1] || ps[_D2] || ps[_D3] ? 2 : 1;
    } else if (xchar > 0) {
        ps[_D0] = (ps[_D0] & ~_DMASK) | 0x3FF0;
        *pex = xchar - 0x3FE;
        return -1;
    } else if (xchar < 0) {
        return 2;
    } else {
        *pex = 0;
        return 0;
    }
}

void _Genld(_Pft* px, char code, unsigned char* p, short nsig, short xexp) {
    const unsigned char point = '.';

    if (nsig <= 0) {
        nsig = 1, p = "0";
    }

    if (code == 'f' || (code == 'g' || code == 'G') && xexp >= -4 && xexp < px->prec) {
        xexp++;
        if (code != 'f') {
            if (((px->flags & 8) == 0) && nsig < px->prec) {
                px->prec = nsig;
            }

            if ((px->prec -= xexp) < 0) {
                px->prec = 0;
            }
        }

        if (xexp <= 0) {
            px->s[px->n1++] = '0';

            if (px->prec > 0 || (px->flags & 8)) {
                px->s[px->n1++] = point;
            }

            if (px->prec < -xexp) {
                xexp = -px->prec;
            }

            px->nz1 = -xexp;
            px->prec += xexp;

            if (px->prec < nsig) {
                nsig = px->prec;
            }

            memcpy(&px->s[px->n1], p, px->n2 = nsig);
            px->nz2 = px->prec - nsig;
        } else if (nsig < xexp) {
            memcpy(&px->s[px->n1], p, nsig);
            px->n1 += nsig;
            px->nz1 = xexp - nsig;
            if (px->prec > 0 || (px->flags & 8)) {
                px->s[px->n1] = point;
                px->n2++;
            }

            px->nz2 = px->prec;
        } else {
            memcpy(&px->s[px->n1], p, xexp);
            px->n1 += xexp;
            nsig -= xexp;

            if (px->prec > 0 || (px->flags & 8)) {
                px->s[px->n1++] = point;
            }

            if (px->prec < nsig) {
                nsig = px->prec;
            }

            memcpy(&px->s[px->n1], &p[xexp], nsig);
            px->n1 += nsig;
            px->nz1 = px->prec - nsig;
        }
    } else {
        if (code == 'g' || code == 'G') {
            if (nsig < px->prec) {
                px->prec = nsig;
            }

            if (--px->prec < 0) {
                px->prec = 0;
            }

            code = (code == 'g') ? 'e' : 'E';
        }

        px->s[px->n1++] = *p++;

        if (px->prec > 0 || (px->flags & 8)) {
            px->s[px->n1++] = point;
        }

        if (px->prec > 0) {
            if (px->prec < --nsig) {
                nsig = px->prec;
            }

            memcpy(&px->s[px->n1], p, nsig);
            px->n1 += nsig;
            px->nz1 = px->prec - nsig;
        }

        p = &px->s[px->n1];
        *p++ = code;

        if (xexp >= 0) {
            *p++ = '+';
        } else {
            *p++ = '-';
            xexp = -xexp;
        }

        if (xexp >= 100) {
            if (xexp >= 1000) {
                *p++ = (xexp / 1000) + '0', xexp %= 1000;
            }
            *p++ = (xexp / 100) + '0', xexp %= 100;
        }
        *p++ = (xexp / 10) + '0', xexp %= 10;

        *p++ = xexp + '0';
        px->n2 = (size_t)p - ((size_t)px->s + px->n1);
    }

    if ((px->flags & 0x14) == 0x10) {
        int n = px->n0 + px->n1 + px->nz1 + px->n2 + px->nz2;

        if (n < px->width) {
            px->nz0 = px->width - n;
        }
    }
}

