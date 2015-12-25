#ifndef _MEM_H__
#define _MEM_H__

//MEM_SIZE is the total size 
#define MEM_SIZE	64000

typedef	unsigned int 	mem_size_t;
typedef	unsigned char	u8_t;
typedef unsigned short	u16_t;
typedef unsigned int 	u32_t;

void mem_init(void);
void *mem_malloc(mem_size_t size);
void mem_free(void *mem);
void *mem_realloc(void *mem, mem_size_t size);
void *mem_reallocm(void *mem, mem_size_t size);

#ifndef MEM_ALIGN_SIZE
    #define MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))
#endif

#ifndef MEM_ALIGN
   #define MEM_ALIGN(addr) ((void *)(((mem_ptr_t)(addr) + MEM_ALIGNMENT-1) & \
   ~(mem_ptr_t)(MEM_ALIGNMENT-1)))
#endif

#ifndef NULL
#define NULL	0
#endif

// ASSERT & DEBUGF primaryly print debug information, Here I make it do nothing at all,
#define ASSERT(x, y) 
#define DEBUGF(x, y)

#endif 
