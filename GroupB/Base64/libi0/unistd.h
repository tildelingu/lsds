#ifndef UNISTD_H
#define UNISTD_H

#include "stdint.h"
#include "stddef.h"

// The type useconds_t is an unsigned integer type
#define useconds_t uint64_t

// sleep for us useconds
void usleep(useconds_t us)
{
    // set system call id
    *(useconds_t*) SYSCALL_ID_ADDR = (useconds_t) SYSCALL_ID_USLEEP;

    // set system call input arguments
    *(useconds_t*) SYSCALL_COMM_AREA_ADDR = us;

    // issue system call
    asm("int 0x80");

    return;
}

#endif // UNISTD_H
