#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

void *malloc(int req_size)
{
	void*(*actual_malloc)(int size);
	void *m_ptr;

	actual_malloc = dlsym(RTLD_NEXT, "malloc");
	m_ptr = actual_malloc(req_size);
	return m_ptr;

}

void free(void *req_ptr)
{
	void *(*actual_free)(void *ptr);

	actual_free = dlsym(RTLD_NEXT, "free");
	free(req_ptr);

}
