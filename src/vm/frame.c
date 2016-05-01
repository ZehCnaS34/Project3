#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "userprog/pagedir.h"
#include "vm/frame.h"
#include "vm/page.h"

/***
 * Initialize the frame table
 *
 */
void frame_table_init (void)
{
  list_init(&frame_table);
  lock_init(&frame_table_lock);
}

void* frame_alloc (enum palloc_flags flags)
{
  if ( !(flags & PAL_USER) )
    {
      return NULL;
    }
  void* frame = palloc_get_page(flags);
  if(frame)
  {
    frame_add_to_table(frame);
  }
  else
  {
    frame = frame_evict();
    if(!frame)
    {
      PANIC ("Frame couldn't be evicted as swap was full");
    }
  }
  return frame;
}

void frame_free (void* frame)
{
  struct list_elem* e;

  lock_acquire(&frame_table_lock);
  e = list_begin(&frame_table);
  while(e != list_end(&frame_table))
    {
      struct frame_entry* fte = list_entry(e, struct frame_entry, elem);
      if (fte->frame == frame)
      {
            list_remove(e);
            free(fte);
            break;
      }
           e = list_next(e);
    }
  lock_release(&frame_table_lock);
  palloc_free_page(frame);
}

void frame_add_to_table (void *frame)
{
  struct frame_entry *fte = malloc(sizeof(struct frame_entry));
  fte->frame = frame;

  lock_acquire(&frame_table_lock);
  list_push_back(&frame_table, &fte->elem);
  lock_release(&frame_table_lock);
}

void* frame_evict (void *frame)
{
  struct list_elem *e;
  
  lock_acquire(&frame_table_lock);
  e = list_begin(&frame_table);
  while(e != list_end(&frame_table))
  {
    struct frame_entry *fte = list_entry(e,struct frame_entry,elem);
    if (pagedir_is_accessed(thread_current()->pagedir,fte->spte->uva))
  	{
  	  pagedir_set_accessed(thread_current()->pagedir, fte->spte->uva, false);
  	}
        else
  	{
  	  if (pagedir_is_dirty(thread_current()->pagedir, fte->spte->uva) || fte->spte->type == SWAP)
  	    {
  	      fte->spte->swap_index = swap_out(fte->frame);
  	    }
  	  fte->spte->is_loaded = false;
  	  return fte->frame;
  	}
    e = list_next(e);
  }
  lock_release(&frame_table_lock);
  return NULL;
}
