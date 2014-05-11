#ifndef TIME_H
#define TIME_H

#define no_DEBUG_TIME_H_

#include "stdint.h"
#include "stddef.h"

#include "stdio.h"

#define time_t long
#define sizeof_time_t sizeof_long

/* gettime sets the amount time (in tv_sec seconds and tv_nsec
 * nanoseconds) since an unspecified point in the past (for example,
 * system start-up time, or the Epoch) on the VPC a task running on.
 *
 * Note: different VPCs possibly have different start point.
 */
long gettime(time_t *tv_sec, long *tv_nsec)
{
    long ret;

    // set system call id
    *(long*) SYSCALL_ID_ADDR = (long) SYSCALL_ID_GETTIME;

    // set system call input arguments: none here

    // call soft interrupt of system call
    asm("int 0x80");

    // get output value of system call
    ret = *(long*) ((long)SYSCALL_COMM_AREA_ADDR);
    *tv_sec = *(time_t*) ((long)SYSCALL_COMM_AREA_ADDR + sizeof_long);
    *tv_nsec = *(long*) ((long)SYSCALL_COMM_AREA_ADDR + sizeof_long + sizeof_time_t);

    return ret;
}

/*
 * substract time tv_sec, tv_nsec from  @tv_sec_from, @tv_nsec_from
 */
void time_subtract(time_t *tv_sec_from, long *tv_nsec_from,
        time_t tv_sec, long tv_nsec)
{
    if (*tv_nsec_from < tv_nsec) {
        *tv_sec_from = *tv_sec_from - 1;
        *tv_nsec_from = *tv_nsec_from + 1000000000L;
    }

    *tv_sec_from = *tv_sec_from - tv_sec;
    *tv_nsec_from = *tv_nsec_from - tv_nsec;

    return;
}
/*
 * add time tv_sec, tv_nsec to  @tv_sec_to, @tv_nsec_to
 */
void time_add(time_t *tv_sec_to, long *tv_nsec_to,
        time_t tv_sec, long tv_nsec)
{
    *tv_sec_to = *tv_sec_to + tv_sec;
    *tv_nsec_to = *tv_nsec_to + tv_nsec;

    if (*tv_nsec_to > 1000000000L) {
        *tv_nsec_to = *tv_nsec_to - 1000000000L;
        *tv_sec_to = *tv_sec_to + 1;
    }

    return;
}


/*
 * get current time; calculate the time difference; and update the
 * time by @tv_sec and @tv_nsec.
 *
 * return 0 on success. -1 on error.
 */
long print_time_diff_and_update(time_t *base_sec, long *base_nsec)
{
    time_t sec;
    long nsec;
    time_t tsec;
    long tnsec;
    long rt;

#ifdef _DEBUG_TIME_H_
    put4('d','p','1',C_n);
#endif

    // get current time
    rt = gettime(&sec, &nsec);

#ifdef _DEBUG_TIME_H_
    put4('d','p','2',C_n);
#endif

    tsec = sec;
    tnsec = nsec;

    if (rt == 0) {

#ifdef _DEBUG_TIME_H_
        put4('d','p','3',C_n);
#endif

        time_subtract(&tsec, &tnsec, *base_sec, *base_nsec);

        // update base
        *base_sec = sec;
        *base_nsec = nsec;

        // print tsec, tnsec
        output_q(tsec);
        putchar('.');
        output_q_digits(tnsec, 9);
    } else {
        put4('E', 'R', 'R', C_n);
        return rt;
    }

    return 0;
}

/*
 * get current time; calculate the time difference; and update the
 * time by @tv_sec and @tv_nsec with the difference.
 *
 * return 0 on success. -1 on error.
 */
long get_time_diff_and_update(time_t *base_sec, long *base_nsec)
{
    time_t sec;
    long nsec;
    long rt;

#ifdef _DEBUG_TIME_H_
    put4('d','p','1',C_n);
#endif

    // get current time
    rt = gettime(&sec, &nsec);

#ifdef _DEBUG_TIME_H_
    put4('d','p','2',C_n);
#endif

    if (rt == 0) {

#ifdef _DEBUG_TIME_H_
        put4('d','p','3',C_n);
#endif

        time_subtract(&sec, &nsec, *base_sec, *base_nsec);

        // update base
        *base_sec = sec;
        *base_nsec = nsec;
    } else {
        put4('E', 'R', 'R', C_n);
        return rt;
    }

    return 0;
}

#endif // TIME_H
