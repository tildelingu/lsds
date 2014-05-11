#ifndef I0STRING_H
#define I0STRING_H

#include "stddef.h"

// NOTE: n must be 8*N, use b:sl to optimize
// Consider using memcmp which is more efficient
// TODO: use register to optimize
long strncmp(char *s1, char *s2, long n)
{
    // tn = s1 + n
    // i1 = s1
    // i2 = s2
    // while i2 < tn {
    //    br:e:sq a i1, i2, continue
    //    br:sl:sq a i1, i2, less
    //    br:j more
    //  continue:
    //    i1 = i1 + 8
    //    i2 = i2 + 8
    // }
    // goto equal
    // less:
    //    return -1;
    // more:
    //    return 1;
    // equal:
    //    return 0;

    // long a;
    // output_str(s1);
    // output_char(C_n);
    // output_str(s2);
    // output_char(C_n);

    // i1
    *(char**)0x200000020 = s1;
    // i2
    *(char**)0x200000028 = s2;
    // t
    *(char**)0x200000030 = s2 + n;

#ifdef _STRNCMP_DEBUG_
    output_str(*(char**)0x200000020);
    output_char(C_n);
    output_str(*(char**)0x200000028);
    output_char(C_n);
#endif

#ifdef _STRNCMP_DEBUG_
    output_char('t');
    output_char(':');
    output_q(*(long*)0x200000030);
    output_char(C_n);
#endif

strncmp_while:
#ifdef _STRNCMP_DEBUG_
    output_char('s');
    output_char(':');
    output_q(*(long*)0x200000028);
    output_char(C_n);
#endif
    // while i2 < tn {
    //    => if tn <= i2, jump to strncmp_e
    // asm("br:le:sq a 0x200000030, 0x200000028, $0x400001832");
    asm("br:le:sq a 0x200000030, 0x200000028, $strncmp_e");
    //    br:e:sq a i1, i2, continue
    //    => jump to strncmp_cont
    //
    // if ((*(long*)0x210000000) == (*(long*)0x210000008)) {
    //     goto strncmp_cont;
    // }
    //
#ifdef _STRNCMP_DEBUG_
    output_q(**(long**)0x200000020);
    output_char(C_n);
    output_q(**(long**)0x200000028);
    output_char(C_n);
#endif
    // asm("br:e:sq a (0x200000020), (0x200000028), $0x400001894");
    asm("br:ne:sq a (0x200000020), (0x200000028), $strncmp_ne");
    //  continue:
    //    i1 = i1 + 8
    asm("add:sq 0x200000020, $8, 0x200000020");
    //    i2 = i2 + 8
    asm("add:sq 0x200000028, $8, 0x200000028");
    //   => jump to strncmp_while
    asm("br:j a $strncmp_while");
 
strncmp_ne:


    //    br:sl:sq a i1, i2, less
    //    => jump to strncmp_l
    // asm("br:sl:sq a 0x200000020, 0x200000028, $0x4000017AD");
    asm("br:sl:sq a 0x200000020, 0x200000028, $strncmp_l");

    //    br:j more
    //    => jump to strncmp_g
    // asm("br:j a $0x40000174B");
    asm("br:j a $strncmp_g");

    // goto strncmp_while;

    // *(long *)(0x100000208) = '!';

strncmp_g:
#ifdef _STRNCMP_DEBUG_
    *(long *)(0x100000208) = 'g';
#endif
    return 1;

strncmp_l:
#ifdef _STRNCMP_DEBUG_
    *(long *)(0x100000208) = 'l';
#endif
    return -1;

strncmp_e:
#ifdef _STRNCMP_DEBUG_
    *(long *)(0x100000208) = '=';
#endif
    return 0;
   
    // just make compiler print out addresses
    goto strncmp_g;
    goto strncmp_l;
    goto strncmp_e;
    goto strncmp_while;
    goto strncmp_ne;

    // for compiler
    return;
}

void memcpy(char* src, long len, char* dst)
{
    long src_n;
    long len8;

    src_n = 0;
    if (len > 8) {
        len8 = len/8*8;
        while (src_n < len8) {
            *(long*)(&(dst[src_n])) = *(long*)(&(src[src_n]));
            src_n = src_n + 8;
        }
    }
    while (src_n < len) {
        dst[src_n] = src[src_n];
        src_n = src_n + 1;
    }
 
    return;
}

// for internal implementation only for memcmp_se
long __memcmp_se(char* s1, char* s2, long n)
{
    long ret_value;

    reg1 = (long)s1;
    reg2 = (long)s2;
    reg3 = n;

    asm("strcmp reg1, reg3, reg2, reg3, reg4");

    ret_value = reg4;

    return ret_value;
}

// To use this function, the program should define 
// ENABLE_MEMCMP_SE
// before #include "libi0/string.h"; otherwise, the compilation fails.
// memcmp with side effect: registers are changed
// faster but need to save the registers by the program
#ifdef ENABLE_MEMCMP_SE
#define memcmp_se(s1, s2, n) __memcmp_se(s1, s2, n)
#else
#define memcmp_se(s1, s2, n) _Pragma ("GCC error \"To use memcmp_se, you must define ENABLE_MEMCMP_SE to indicate that you are aware its side effects.\"")
#endif

long memcmp(char* s1, char* s2, long n)
{
    long ret_value;

    long sr1;
    long sr2;
    long sr3;
    long sr4;

    // save registers
    sr1 = reg1;
    sr2 = reg2;
    sr3 = reg3;
    sr4 = reg4;

    reg1 = (long)s1;
    reg2 = (long)s2;
    reg3 = n;

    asm("strcmp reg1, reg3, reg2, reg3, reg4");

    ret_value = reg4;

    // restore registers
    reg1 = sr1;
    reg2 = sr2;
    reg3 = sr3;
    reg4 = sr4;

    return ret_value;
}

void memset0(char* m, long len)
{
    char *t;
    t = m + len;
    while(m < t) {
        *m = (char)0;
        m = m + 1;
    }
    return;
}


#endif // I0STRING_H
