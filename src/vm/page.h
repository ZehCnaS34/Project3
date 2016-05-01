#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>
#include "vm/frame.h"

#define FILE 0
#define SWAP 1
#define MMAP 2

/*
 * What the processor uses to translate from page to frame
 */
struct sup_page_entry {
  size_t offset; /// For files
  size_t read_bytes;
  size_t zero_bytes;
  size_t swap_index; /// For swap

  struct hash_elem elem;
  struct file *file;

  uint8_t type;
  bool writable;
  bool is_loaded;

  void *uva;
};

bool add_file_to_page_table (struct file*,
                             int32_t, uint8_t*,
                             uint32_t, uint32_t,
                             bool);
bool load_file (struct sup_page_entry*);
bool load_swap (struct sup_page_entry*);
bool load_page (void*);
bool load_mmap (struct sup_page_entry*);
void page_table_destroy (struct hash*);
static unsigned page_hash_func (const struct hash_elem*,
                                void* e UNUSED); // TODO: might take e out
static bool page_less_func (const struct hash_elem*,
                            const struct hash_elem*,
                            void* aux UNUSED);
static void page_action_func (struct hash_elem*, void *aux UNUSED);
void page_table_init (struct hash*);
static struct sup_page_entry* get_spte (void*);

bool add_mmap_to_page_table(struct file* file, int32_t ofs, uint8_t upage, uint32_t read_bytes,uint32_t zerobytes);
#endif /* vm/page.h */
