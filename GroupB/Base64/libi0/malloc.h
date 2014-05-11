#ifndef I0MALLOC_H
#define I0MALLOC_H

#include "stddef.h"
#include "stdio.h"

// whether the shalloc is made standalone
// if defined, each shalloc'ed range will be standalone
#define STANDALONE_SHALLOC

//remove following comment to get some debug output.
// #define _DEBUG_MLC_

//flag that whether shalloc area has been used. 
//It is in auto init area. Its init value is 0.
#define PR_IS_USED (0x200100000)

//---------------------Shalloc Defination----------------------------
//any requested size to scheduler should be aligned to 64M
#define S_ALIGN_TO_SIZE (0x4000000)

// 1 page
#define S_ALIGN_TO_SIZE_EXT (4096)

//the base address of shalloc area range array
//is a const value
#define PR_SAR_ARRAY_BASE (0x300003008)

//the end address of shalloc area range array
//is a const value
//0x10000 elements in sar_array
#define PR_SAR_ARRAY_END (0x300183008)

//the last element in shalloc area range array
//is a variable
#define PR_SAR_ARRAY_LAST_ELEMENT (0x300003000)

//malloc system call id
// defined in stddef.h
// #define SYSCALL_ID_MALLOC (1)
//---------------------End of Shalloc Defination---------------------

//---------------------Pralloc Defination----------------------------
//any requested size should be aligned to page size, namely 4K bytes.
#define P_ALIGN_TO_PAGE (0x1000)

//the base address of shalloc area range array
//is a const value
#define PR_PA_BASE (0x3FFFF0000)

//the end address of shalloc area range array
//is a const value
//0x10000 elements in sar_array
#define PR_PA_END (0x300200000)

//the last element in shalloc area range array
//is a variable
#define PR_PA_TP (0x3FFFFFFF8)
//---------------------End of Pralloc Defination---------------------


#define shalloc_option_t  uint64_t
#define sizeof_shalloc_option_t sizeof_uint64_t

#define SHALLOC_1WAY 1
#define SHALLOC_3WAY 3

void _output_debug(long x)
{
    long t;
    long i;
    long vec[16];

    if(x == 0)
        *(long*) STDOUT = '0';

    i = 0;
    while(x != 0)
    {
        t = x / 16;
        if(x - t * 16 < 10)
            vec[i] = x - t * 16 + '0';
        else
            vec[i] = x - t * 16 - 10 + 'A';
        
        x = t;
        i = i + 1;
    }
    i = i - 1;
    for(; i >= 0; i = i - 1)
    {
        *(long*)STDOUT = vec[i];
    }

    *(long*) STDOUT = 10;
    return;
}

void init_PR_var()
{
    *(long*) PR_SAR_ARRAY_LAST_ELEMENT = (long) PR_SAR_ARRAY_BASE;
    *(long*) PR_PA_TP = (long) PR_PA_BASE;
    *(long*) PR_IS_USED = 1;

    return; 
}

//---------------------Shalloc----------------------------
//Shalloc Area Range = SAR
/*struct Shalloc_Area_Range {
    char *BASE;
    char *END; //first addr that out of the area range 
    char *LTP; //local top pointer
};*/

char *_syscall_malloc_ext(long size, shalloc_option_t option)
{
    //set system call id
    *(long*) SYSCALL_ID_ADDR = (long) SYSCALL_ID_MALLOC_EXT;
    
    //set system call input arguments
    *(long*) SYSCALL_COMM_AREA_ADDR = size;
    
    *(long*) ((long)SYSCALL_COMM_AREA_ADDR + sizeof_shalloc_option_t) = option;
    
    //call soft interrupt of system call
    asm("int 0x80");
   
    //get output value of system call
    return *(char **) SYSCALL_COMM_AREA_ADDR;
}


char *_syscall_malloc(long size)
{
    //set system call id
    *(long*) SYSCALL_ID_ADDR = (long) SYSCALL_ID_MALLOC;
    
    //set system call input arguments
    *(long*) SYSCALL_COMM_AREA_ADDR = size;
    
    //call soft interrupt of system call
    asm("int 0x80");
   
    //get output value of system call
    return *(char **) SYSCALL_COMM_AREA_ADDR;
}

//align size to a large number, which should always be a multiplier of PAGE_SIZE(512)
long s_align_to(long size)
{
#ifdef _DEBUG_MLC_
    _output_debug(size);
#endif
    return ((size - 1) / (S_ALIGN_TO_SIZE) + 1) * (S_ALIGN_TO_SIZE);
}

long s_align_to_ext(long size)
{
#ifdef _DEBUG_MLC_
    output_q_hex(size);
    output_char(C_n);
#endif
    return ((size - 1) / (S_ALIGN_TO_SIZE_EXT) + 1) * (S_ALIGN_TO_SIZE_EXT);
}

long s_align_to_page(long size)
{
    return (size + PAGE_SIZE - 1) / (PAGE_SIZE) * (PAGE_SIZE);
}

char *_get_new_sar(long size, long align_size)
{
    char *ret;
    long last_element;

    //call a system call to ask for new SAR
    ret = _syscall_malloc(align_size);
      
    //push back this new SAR as the last element of SAR_ARRAY
    last_element = *(long*) PR_SAR_ARRAY_LAST_ELEMENT;

    *(char **) last_element = ret;
    *((char **) (last_element + 8)) = ret + align_size;
    *((char **) (last_element + 16)) = ret + size;

#ifdef _DEBUG_MLC_
    _output_debug(*(long*)last_element);
    _output_debug(*(long*)(last_element + 8));
    _output_debug(*(long*)(last_element + 16));
    *(long*)STDOUT = 'N';
    *(long*)STDOUT = 10;
#endif

    return ret;
}

char *_get_new_sar_ext(long size, long align_size, shalloc_option_t option)
{
    char *ret;
    long last_element;

    //call a system call to ask for new SAR
    ret = _syscall_malloc_ext(align_size, option);

#ifdef _DEBUG_MLC_
    output_char('s');
    output_char('y');
    output_char('s');
    output_char('_');
    output_char('r');
    output_char('t');
    output_char(':');
    output_q_hex(ret);
    output_char(C_n);
#endif
    //push back this new SAR as the last element of SAR_ARRAY
    last_element = *(long*) PR_SAR_ARRAY_LAST_ELEMENT;

    *(char **) last_element = ret;
    *((char **) (last_element + 8)) = ret + align_size;
    *((char **) (last_element + 16)) = ret + size;

#ifdef _DEBUG_MLC_
    output_q_hex(*(long*)last_element);
    *(long*)STDOUT = 10;
    output_q_hex(*(long*)(last_element + 8));
    *(long*)STDOUT = 10;
    output_q_hex(*(long*)(last_element + 16));
    *(long*)STDOUT = 10;
    *(long*)STDOUT = 'N';
    *(long*)STDOUT = 10;
#endif

    return ret;
}


//search in SAR_ARRAY
char *_get_from_sar_array(long size)
{
    long i;
    long last_element;
    long *sar_array_element;
    long old_value;

#ifdef _DEBUG_MLC_    
     _output_debug((long)PR_SAR_ARRAY_BASE);
     _output_debug(*(long*)(PR_SAR_ARRAY_LAST_ELEMENT));
     *(long*)STDOUT = 'Z';
     *(long*)STDOUT = 10;
#endif

    last_element = *(long*) PR_SAR_ARRAY_LAST_ELEMENT;
    

    for(sar_array_element = (long*)PR_SAR_ARRAY_BASE;
        (long)sar_array_element <= last_element;
        sar_array_element = sar_array_element + 24)
    { 
 
#ifdef _DEBUG_MLC_ 
        _output_debug(*sar_array_element);
        _output_debug(*(sar_array_element + 8));
        _output_debug(*(sar_array_element + 16));
        _output_debug((*(sar_array_element + 8)) - (*(sar_array_element + 16)));
        *(long*) STDOUT = 'Q';
        *(long*) STDOUT = 10;
#endif

        //if we find a SAR has enough space to allocate requested memory
        //first-fit strategy
        if(((*(sar_array_element + 8)) - (*(sar_array_element + 16))) >= size)
        {

#ifdef _DEBUG_MLC_
            *(long*) STDOUT = 'V';
            *(long*) STDOUT = 10;
#endif
            old_value = *(sar_array_element + 16);
            *(sar_array_element + 16) = *(sar_array_element + 16) + size;
            return  (char*) old_value;
        }
    }

    //if we do not find satisfied SAR
    return (char*)NULL;
}

void* shalloc(long size)
{
    long align_size;
    char* ret;

    //if input SIZE is not positive value.
    if(size <= 0)
        return (void*)NULL;

#ifdef STANDALONE_SHALLOC
    size = s_align_to_page(size);
#endif

    //align to a large area
    align_size = s_align_to(size);

#ifdef _DEBUG_MLC_
    output_q_hex(align_size);
    *(long*) STDOUT = 10;
    output_q(*(long*) PR_IS_USED);
    *(long*) STDOUT = 10;
    *(long*) STDOUT = 'K';
    *(long*) STDOUT = 10;
#endif

    //if SAR_ARRAY has not been initialized
    //then ask system for a shalloc area
    if((long)(*(long *) PR_IS_USED) == 0)
    {
        init_PR_var();

        //get new SAR from system.
        ret = _get_new_sar(size, align_size);
    }
    else //if SAR_ARRAY has been initialized
    {
        //search from SAR_ARRAY, to get requested memory
        ret = _get_from_sar_array(size);

#ifdef _DEBUG_MLC_        
        _output_debug((long)ret);
        *(long*) STDOUT = 'T';
        *(long*) STDOUT = 10;
#endif

        //if SIZE is too large to allocated in SAR_ARRAY,
        //then ask system for a shalloc area. 
        if(ret == (char*) NULL)
        {
            if ( (long)((*(long*) PR_SAR_ARRAY_LAST_ELEMENT) + 24) < (long)PR_SAR_ARRAY_END )
            {
                *(long*) PR_SAR_ARRAY_LAST_ELEMENT = (*(long*) PR_SAR_ARRAY_LAST_ELEMENT) + 24;
                //get new SAR from system.
                ret = _get_new_sar(size, align_size);
            }
            else 
                ret = (char *)NULL;
                //TODO: not return null. 
                //LATS_ELEMENT should restart from BASE, to overlap oldest element.
            
        }
    }

    return (void *) ret;
}

//----------------------Pralloc-------------------------

//align size to a large number, which should always be a multiplier of PAGE_SIZE(512)
long p_align_to(long size)
{
#ifdef _DEBUG_MLC_
    _output_debug(size);
#endif
    return ((size - 1) / (P_ALIGN_TO_PAGE) + 1) * (P_ALIGN_TO_PAGE);
}

void *_get_new_pa(long size)
{
    long ret;

#ifdef _DEBUG_MLC_
    _output_debug(*(long*) PR_PA_TP);
    *(long*) STDOUT = 'P';
    *(long*) STDOUT = 10;
#endif

    if ((long)((*(long*)PR_PA_TP) - size) >= (long)PR_PA_END )
    {
        *(long*) PR_PA_TP = *(long*) PR_PA_TP - size;
        ret = *(long*) PR_PA_TP;
    }
    else
        ret = (long)NULL;

#ifdef _DEBUG_MLC_
    _output_debug(*(long*) PR_PA_TP);
#endif

    return (void*)ret;
}

void *pralloc(long size)
{
    long align_size;
    void* ret;

    //if input SIZE is not positive value.
    if(size <= 0)
        return (void*)NULL;

    //align to a large area
    align_size = p_align_to(size);

#ifdef _DEBUG_MLC_
    _output_debug(align_size);
    *(long*) STDOUT = 10;
    output_q(*(long*) PR_IS_USED);
    *(long*) STDOUT = 10;
    *(long*) STDOUT = 'K';
    *(long*) STDOUT = 10;
#endif

    //if pralloc area metadata has not been initialized
    if((long)(*(long *) PR_IS_USED) == 0)
    {
        init_PR_var();
    }
    
    //get new pralloc area from system.
    ret = _get_new_pa(align_size);
    
    return ret;
}

// allocate memory ranges according the the option
// on success, return the allocated address
// on fail, return NULL
addr_t shalloc_ext(size_t size, shalloc_option_t option)
{
    long align_size;
    char* ret;

    //if input SIZE is not positive value.
    if(size <= 0)
        return (void*)NULL;

#ifdef STANDALONE_SHALLOC
    size = s_align_to_page(size);
#endif

    //align to a large area
    align_size = s_align_to_ext(size);

#ifdef _DEBUG_MLC_
    output_q_hex(align_size);
    output_char(C_n);
    output_q_hex(*(long*) PR_IS_USED);
    output_char(C_n);
#endif

    //if SAR_ARRAY has not been initialized
    //then ask system for a shalloc area
    if((long)(*(long *) PR_IS_USED) == 0)
    {
        init_PR_var();

        //get new SAR from system.
        ret = _get_new_sar_ext(size, align_size, option);
    }
    else //if SAR_ARRAY has been initialized
    {
        // skip SAR_ARRAY
        ret = (char*)NULL;

        //then ask system for a shalloc area. 
        if(ret == (char*) NULL)
        {
            if ( (long)((*(long*) PR_SAR_ARRAY_LAST_ELEMENT) + 24) < (long)PR_SAR_ARRAY_END )
            {
                *(long*) PR_SAR_ARRAY_LAST_ELEMENT = (*(long*) PR_SAR_ARRAY_LAST_ELEMENT) + 24;
                //get new SAR from system.
                ret = _get_new_sar_ext(size, align_size, option);
            }
            else 
                ret = (char *)NULL;
                //TODO: not return null. 
                //LATS_ELEMENT should restart from BASE, to overlap oldest element.
            
        }
    }

    return (void *) ret;
}

// addr is the addr_t returned by shalloc_ext
// nth is the nth range in the replication group.
// addr is the 0th replica
addr_t shalloced_replicas(addr_t addr, size_t nth)
{
    long inc;

    inc = PMEM_N_SIZE / MM_REPLICATION_FACTOR;
    inc = addr + inc * nth;

    return inc;
}


#endif //I0MALLOC_H
