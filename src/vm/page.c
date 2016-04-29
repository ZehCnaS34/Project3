#include <string.h>
#include "filesys/file.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/process.h"
#include "vm/frame.h"
#include "vm/page.h"

bool add_file_to_page_table (struct file *file, int32_t ofs, uint8_t *upage,
                             uint32_t read_bytes, uint32_t zero_bytes,
                             bool writable)
{
  struct sup_page_entry *spte = malloc(sizeof(struct sup_page_entry));
  if (!spte)
    {
      return false;
    }
  spte->file       = file;
  spte->offset     = ofs;
  spte->uva        = upage;
  spte->read_bytes = read_bytes;
  spte->zero_bytes = zero_bytes;
  spte->writable   = writable;
  spte->is_loaded  = false;
  spte->type       = FILE;

  return (hash_insert(&thread_current()->spt, &spte->elem) == NULL);
}


bool load_file (struct sup_page_entry *spte)
{
  void* addr = pagedir_get_page (thread_current()->pagedir, spte->uva);
  uint8_t *frame = frame_alloc (PAL_USER);
  if (!frame)
    {
      return false;
    }
  if ((int) spte->read_bytes != file_read_at(spte->file, frame,
                                             spte->read_bytes, spte->offset))
    {
      frame_free(frame);
      return false;
    }
  memset(frame + spte->read_bytes, 0, spte->zero_bytes);

  if (!install_page(spte->uva, frame, spte->writable))
    {
      frame_free(frame);
      return false;
    }

  // Set frame->pte = spte
  spte->is_loaded = true;
  
  return true;
}


bool load_swap (struct sup_page_entry *spte)
{
  return false;
}


bool load_page (void *uva)
{
  struct sup_page_entry *spte = get_spte(uva);
  if (!spte)
    {
      return false;
    }
  
  bool success = false;
  switch (spte->type)
    {
    case FILE:
      success = load_file(spte);
      break;
    case SWAP:
      success = load_swap(spte);
      break;
    case MMAP:
      success = load_mmap(spte);
      break;
    }
  return success;
}

bool load_mmap (struct sup_page_entry *spte)
{
  return false;
}

void page_table_destroy (struct hash *spt)
{
  hash_destroy (spt, page_action_func);
}

static unsigned page_hash_func (const struct hash_elem *e, void *aux UNUSED)
{
  struct sup_page_entry *spte = hash_entry(e, struct sup_page_entry,
                                           elem);
  return hash_int((int) spte->uva);
}

static bool page_less_func (const struct hash_elem *a,
                            const struct hash_elem *b,
                            void *aux UNUSED)
{
  struct sup_page_entry *sa = hash_entry(a, struct sup_page_entry, elem);
  struct sup_page_entry *sb = hash_entry(b, struct sup_page_entry, elem);
  if (sa->uva < sb->uva)
    {
      return true;
    }
  return false;
}

static void page_action_func (struct hash_elem *e, void *aux UNUSED)
{
  struct sup_page_entry *spte = hash_entry(e, struct sup_page_entry, elem);
  free(spte);
}

void page_table_init (struct hash *spt)
{
  hash_init (spt, page_hash_func, page_less_func, NULL);
}


static struct sup_page_entry* get_spte (void *uva)
{
  struct sup_page_entry spte;
  spte.uva = pg_round_down(uva);

  struct hash_elem *e = hash_find(&thread_current()->spt, &spte.elem);
  if (!e)
    {
      return NULL;
    }
  return hash_entry (e, struct sup_page_entry, elem);
}


