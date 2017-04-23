#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

#define MAXMEM 999
#define CANAARY_SIZE 6
typedef struct {
	void *mem_ptr;
	size_t alloc_size;
	char[CANAARY_SIZE] canary;
	boolean in_use;
}mem_track;

mem_track ins[MAXMEM];

for(int i = 0; i < MAXMEM; i++)
{
	ins[i].in_use = FALSE;
}
/*Returns -1 if memory access is denied
           size of the Memory chunk otherwise */
size_t check_memory_access(void *  ptr)
{

	for(int i = 0; i < MAXMEM; i++)
	{
		//Fetch the Allocated block's information
		if(ins[i].in_use == TRUE) 
		{
			if(ptr >= ins[i].mem_ptr)
			{
				if(ptr <= (ins[i].mem_ptr + ins[i].alloc_size) &&
				   !memcmp(ins[i].mem_ptr + ins[i].alloc_size, ins[i].canary, 6)
				{
					//Return the writable size that is allowed with this request
					return ((ins[i].mem_ptr + ins[i].alloc_size) - ptr);
				}
				else
				{
					fprintf(stderr, "ILLEGAL Memory Access");
				}
			}
		}
	}

}

void  del_memory_track(void *ptr)
{

	for(int i = 0; i < MAXMEM; i++)
	{
		if(ins[i].in_use == TRUE && (ptr > ins[i].mem_ptr) &&
		  (ptr < ins[i].mem_ptr + ins[i].alloc_size))
		{
			ins[i].in_use = FALSE;
			ins[i].mem_ptr = 0;
			ins[i].alloc_size = 0;
		}
	}

}

void update_memory_track(void *ptr, size_t size)
{
	
	for(int i = 0; i < MAXMEM; i++)
	{
		if(ins[i].in_use == TRUE && (ptr > ins[i].mem_ptr) &&
		  (ptr < ins[i].mem_ptr + ins[i].alloc_size))
		{
			ins[i].mem_ptr = (void *)ptr;
			ins[i].alloc_size = size;
		}
	}
}
void *malloc(int req_size)
{
	void*(*actual_malloc)(int size);
	void *m_ptr;

	actual_malloc = dlsym(RTLD_NEXT, "malloc");
    fprintf(stdout, "hello\n");
	m_ptr = actual_malloc(req_size + CANAARY_SIZE);

	int i = 0;
	while(i < MAXMEM)
	{
		if(ins[i].in_use == FALSE)
		{
			ins[i].mem_ptr = m_ptr;
			ins[i].alloc_size = req_size;
			memcpy(ins[i].canary, "CSE545", CANAARY_SIZE);
			break;
		}
		else
		{
			i++;
			continue;
		}
	}
	memcpy()
	return m_ptr;

}

void free(void *req_ptr)
{
	void *(*actual_free)(void *ptr);

	actual_free = dlsym(RTLD_NEXT, "free");
	actual_free(req_ptr);

}


