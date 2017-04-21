#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

typedef struct {
	void *mem_ptr;
	size_t size;
	char[6] canary;
}mem_track;

mem_track ins[999];


/*Returns -1 if memory access is denied
           size of the Memory chunk otherwise */
size_t check_memory_access(void *  ptr)
{

}

void
void *malloc(int req_size)
{
	void*(*actual_malloc)(int size);
	void *m_ptr;

	actual_malloc = dlsym(RTLD_NEXT, "malloc");
        fprintf(stdout, "hello\n");
	m_ptr = actual_malloc(req_size);
	return m_ptr;

}

void free(void *req_ptr)
{
	void *(*actual_free)(void *ptr);

	actual_free = dlsym(RTLD_NEXT, "free");
	actual_free(req_ptr);

}
