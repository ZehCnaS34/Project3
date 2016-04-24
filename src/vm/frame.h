#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdbool.h>
#include <stdint.h>
#include <list.h>

struct lock FTL; /* Frame table lock */
struct list FT; /* Frame table */

struct FE{ /* Frame Entry */
    struct list_elem elem;
    void*frame;
    tid_t tid;
    uint32_t* pte;
};

void FT_init(void);
void* F_allocate(enum palloc_flags flags); 
bool F_add(void* frame); // adding frame to table
void F_remove(void* frame);
bool F_evict(void* frame);


#endif /* VM_FRAME_H */