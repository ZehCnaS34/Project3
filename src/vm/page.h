#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>

#define FILE 0
#define SWAP 1
#define MMAP 2

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

bool add_file_to_page_table (struct file *file, int32_t ofs, uint8_t *upage,
                             uint32_t read_bytes, uint32_t zero_bytes,
                             bool writable);
bool load_file (struct sup_page_entry *spte);
bool load_swap (struct sup_page_entry *spte);
bool load_mmap (struct sup_page_entry *spte);
void page_table_destroy (struct hash *spt);
void page_table_init (struct hash *spt);

#endif /* vm/page.h */
