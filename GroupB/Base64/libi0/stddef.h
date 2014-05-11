
#ifndef I0STDDEF_H
#define I0STDDEF_H

#include "stdint.h"

// registers only supports 4 yet

register long reg1;
register long reg2;
register long reg3;
register long reg4;


// system confgiruation
#define PAGE_SIZE (4096)

#define N_LONG_PER_PAGE (512)
#define N_CHAR_PER_PAGE PAGE_SIZE

// sizes
#define size_t int64_t
#define sizeof_size_t 8

#define addr_t uint64_t

// #define void char

// Note: these configurations should be consistent with the sys_config.h of
// L0

// LIR_ADDRESS is the starting address of LIR
#define LIR_ADDRESS  (0x100004000)
#define LIRS_MAX_LEN (0xc000)

// LIRS is for users
#define LIRS ((char*)LIR_ADDRESS))

// the starting address of SR
#define SR_BEGIN	0x400000000
// the ending address of SR
#define SR_END		0x100000000000

// otherwise 8TB
#define PPM_12TB 1


// STDIN
#define STDIN     (0x100000200)

// STDOUT
#define STDOUT    (0x100000208)

// memory of address that "stores" runner ID
#define RUNNER_ID (0x100000120)

// NULL
#define NULL      0


#ifdef PPM_12TB

// // planetary memory range (12TB)
// #define PPM_START  0x40000000000
// #define PPM_SIZE   0xc0000000000
// #define PPM_END    0xfffffffffff


// planetary memory range (12TB)
#define PPM_BEGIN  0x40000000000
#define PPM_SIZE   0xc0000000000
#define PPM_END    0xfffffffffff

#define PPM_START  PPM_BEGIN

// further partitioning the PPM (persistent memory)

// normal SR (in physical memory) size
// 2TB
#define SR_N_SIZE  0x20000000000

// normal SR begin and end
#define SR_N_BEGIN (SR_BEGIN)
#define SR_N_END (SR_BEGIN + SR_N_SIZE - 1)

// normal AMR (in physical memory) size
// 2TB
#define AMR_N_OFFSET_SIZE 0x20000000000

#define AMR_N_OFFSET_BEGIN (SR_N_END + 1)
#define AMR_N_OFFSET_END (AMR_N_OFFSET_BEGIN + AMR_N_OFFSET_SIZE - 1)

// persistent AMR size
// 6TB
#define AMR_P_OFFSET_SIZE 0x60000000000


#define AMR_P_OFFSET_BEGIN (AMR_N_OFFSET_END + 1)
#define AMR_P_OFFSET_END (AMR_P_OFFSET_BEGIN + AMR_P_OFFSET_SIZE - 1)


// normal (not in AMR) persistent memory size
// 6TB
#define PMEM_N_SIZE       0x60000000000


#define PMEM_N_BEGIN (AMR_P_OFFSET_END + 1)
#define PMEM_N_END (PMEM_N_BEGIN + PMEM_N_SIZE - 1)


// Note: in total persistent memory size is PPM_SIZE

// Note: libi0 and KV+ need to be changed accordingly

// AMR ranges
// Option 1: all the SR except PPM
// #define AMR_OFFSET_BEGIN SR_BEGIN
#define AMR_OFFSET_BEGIN AMR_N_OFFSET_BEGIN
// #define AMR_OFFSET_END   PPM_BEGIN
// for KV+
// #define AMR_OFFSET_END   0x54000000000
#define AMR_OFFSET_END   AMR_P_OFFSET_END



// replication factor
#define MM_REPLICATION_FACTOR 3





// idle handler heap
#define IDLE_HANDLE_RUNNER_HEAP_BEGIN (0x3f800000000)
#define IDLE_HANDLE_RUNNER_HEAP_LEN   (0x1000)

#else // default 8TB

// planetary memory range (8TB)
#define PPM_START  0x80000000000
#define PPM_SIZE   0x80000000000
#define PPM_END    0xfffffffffff

// idle handler heap
#define IDLE_HANDLE_RUNNER_HEAP_BEGIN (0x7f800000000)
#define IDLE_HANDLE_RUNNER_HEAP_LEN (0x1000)

#endif

// system call
#define SYSCALL_ID_ADDR (0x100000420)
#define SYSCALL_COMM_AREA_ADDR (0x100001000)
#define SYSCALL_COMM_AREA_LEN (0x1000)

#define SYSCALL_ID_TYPE uint64_t

// syscall ID
#define SYSCALL_ID_MALLOC  (1)
#define SYSCALL_ID_MALLOC_EXT  (6)

#define SYSCALL_ID_USLEEP  (2)
#define SYSCALL_ID_GETTIME (7)

// streaming I/O
#define SYSCALL_ID_SREAD   (3)
#define SYSCALL_ID_SWRITE  (4)
#define SYSCALL_ID_SLISTEN (5)

// Note: should be consistent with the configurations in L0
// spaces
#define L0_SPACE0           (0x000000000000)
#define L0_SPACE1           (0x400000000000)
#define L0_SPACE2           (0x800000000000)
#define L0_SPACE3           (0xC00000000000)
#define L0_SPACE4          (0x1000000000000)
#define L0_SPACE5          (0x1400000000000)
#define L0_SPACE6          (0x1800000000000)
#define L0_SPACE7          (0x1C00000000000)
#define L0_SPACE8          (0x2000000000000)
#define L0_SPACE9          (0x2400000000000)
#define L0_SPACE10         (0x2800000000000)
#define L0_SPACE11         (0x2C00000000000)
#define L0_SPACE12         (0x3000000000000)

#define L0_SPACE_INC        (0x400000000000)

// exit code
#define NEW_RUNNER_ID_ADDR (0x100000410)
#define EC_RANGE           (0x1000)

// functions

// get current task's ID
long get_my_task_id()
{
    return *(long*)(RUNNER_ID);
}

// get the new tasks' ID.
// should be called just after invoking "runner" or spawn/spawnx
long get_new_task_id()
{
    return *(long*)(NEW_RUNNER_ID_ADDR);
}

// calculate the exit code range from the ID
// len in bytes
void task_id_to_ec_range(long id, long *base, long *len)
{
    *base = id;
    *len = EC_RANGE;
    return;
}

#endif // I0STDDEF_H

