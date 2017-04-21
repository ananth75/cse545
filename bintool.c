#include <stdio.h>
#include <dlfcn.h>

void *malloc(int size);
void free(void *ptr);

void *malloc(int req_size)
{
	static *(void **)(acutal_malloc)(int size);
	void *m_ptr;

	acutal_malloc = dlsym(RTLD_NEXT, "malloc");
	m_ptr = acutal_malloc(req_size);
	return m_ptr;

}

void free(void *req_ptr)
{
	static *(void **)(acutal_free)(void *ptr);

	acutal_free = dlsym(RTLD_NEXT, "free");
	free(req_ptr);

}
