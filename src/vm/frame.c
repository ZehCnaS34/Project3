#include "frame.h"
// thread headers
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/synch.h"
#include "threads/thread.h"

void FT_init(void)
{
    list_init(&FT);
    lock_init(&FTL);
}
void* F_allocate(enum palloc_flags flags)
{
    if(!(PAL_USER & flags)
    {
        return NULL;
    }
    void *frame = palloc_get_page(flags);
    if(!frame)
    {
        if(!F_evict(frame))
        {
            PANIC ("Frame could not be evicted because swap is full!");
        }
    }
    else
    {
        F_add(frame);
    }
    return frame;
}
void F_add(void* frame)
{
   struct FE *temp = malloc(sizeof(struct FE));
   temp->frame = frame;
   temp->tid = thread_tid();
   
   // using lock to avoid race conditions
   lock_acquire(&FTL);
   list_push_back(&FT, &temp->elem);
   lock_release(&FTL);
}
void F_remove(void * frame)
{
    struct list_elem* etemp;
    struct list_elem* etemp2;
    
    lock_acquire(FTL);
    
    /*  for efficiency ( instead of calling function 
    multiple times)*/
    etemp2 = list_end(&FT);
    struct frame_entry *temp;
    // finding the entry in the list
     
    for(etemp = list_begin(&FT); etemp != etemp2; e = list_next(&FT))
    {
        temp = list_entry(etemp,struct FE,elem);
        if(temp->frame == frame)
        {
            list_remove(e);
            free(temp);
            break;
        }
    }
    lock_release(&FT);
    palloc_free_page(frame);
}
bool F_evict(void* frame)
{
    return false;
}