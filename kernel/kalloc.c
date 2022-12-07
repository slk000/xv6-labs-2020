// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  uint refcnt[PHYSTOP>>PGSHIFT]; // 0x88000(557056)
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  for(int i = 0; i < PHYSTOP>>PGSHIFT; i++){
    kmem.refcnt[i] = 1;
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // check the refcnt of pa
  acquire(&kmem.lock);
  int newcnt = --kmem.refcnt[(uint64)pa>>PGSHIFT];
  if(newcnt > 0){
    release(&kmem.lock);
    return;
  }
  // else if(newcnt < 0){
  //   panic("kfree: over free");
  // }
  release(&kmem.lock);
  
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){

  
      kmem.freelist = r->next;
    // init refcnt of pa
    // the instruction says init the count to 1 when call kalloc
    // if(kmem.refcnt[(uint64)r>>PGSHIFT] != 0) {
    //   panic("kalloc: refcnt not 0 before alloc");
    // }
    kmem.refcnt[(uint64)r>>PGSHIFT] = 1;
  }
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

// increase or decrease the ref cnt of PAGE
// which contains addr pa
// return the new cnt
uint
incref(uint64 pa)
{
  if (pa < KERNBASE) {
    return 0;
  }
  pa = PGROUNDDOWN(pa);
  uint newcnt = 0;
  acquire(&kmem.lock);
  newcnt = ++kmem.refcnt[(uint64)pa>>PGSHIFT];
  release(&kmem.lock);
  return newcnt;
}
uint
decref(uint64 pa)
{
  if (pa < KERNBASE) {
    return 0;
  }
  pa = PGROUNDDOWN(pa);
  uint newcnt = 0;
  acquire(&kmem.lock);
  newcnt = --kmem.refcnt[(uint64)pa>>PGSHIFT];
  release(&kmem.lock);
  // if(newcnt==0){
  //   kfree((void*)pa);
  // }
  return newcnt;
}