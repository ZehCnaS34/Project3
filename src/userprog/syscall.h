#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "userprog/process.h"
#include <stdbool.h>
#include <stdint.h>

struct lock filesys_lock;

void syscall_init (void);

#endif /* userprog/syscall.h */
