#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <dlfcn.h>

#define MAXMEM 999
#define CANAARY_SIZE 6
typedef struct {
	void *mem_ptr;
	size_t alloc_size;
	char canary[CANAARY_SIZE];
	bool in_use;
}mem_track;

mem_track ins[MAXMEM];
int i =0;

for(int i = 0; i < MAXMEM; i++)
{
	ins[i].in_use = FALSE;
}
/*Returns -1 if memory access is denied
           size of the Memory chunk otherwise */
size_t check_memory_access(void *  ptr)
{
	for(i = 0; i < MAXMEM; i++)
	{
		//Fetch the Allocated block's information
		if(ins[i].in_use == true) 
		{
			if(ptr >= ins[i].mem_ptr)
			{
				if(ptr <= (ins[i].mem_ptr + ins[i].alloc_size) &&
				   !memcmp(ins[i].mem_ptr + ins[i].alloc_size, ins[i].canary, 6))
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
	for(i = 0; i < MAXMEM; i++)
	{
		if(ins[i].in_use == true && (ptr > ins[i].mem_ptr) &&
		  (ptr < ins[i].mem_ptr + ins[i].alloc_size))
		{
			ins[i].in_use = false;
			ins[i].mem_ptr = 0;
			ins[i].alloc_size = 0;
		}
	}

}

void update_memory_track(void *ptr, size_t size)
{
	for(i = 0; i < MAXMEM; i++)
	{
		if(ins[i].in_use == true && (ptr > ins[i].mem_ptr) &&
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
		if(ins[i].in_use == false)
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
	return m_ptr;

}

void free(void *req_ptr)
{
	void *(*actual_free)(void *ptr);

	actual_free = dlsym(RTLD_NEXT, "free");
	actual_free(req_ptr);
}


char * strncpy(char *dest, char *src, int n)
{
	int i=0;

	for(i = 0;i<n ; i++)
	{
		if(src[i]!='\0')
			dest[i]=src[i];
		else
			dest[i]='\0';
	}
	dest[i-1]='\0';
}

char* strcpy(char *dest, char* src)
{

	size_t s = check_memory_access(dest);
	if(s == -1)
	{
		return dest;
	}
	return strncpy(dest,src,s);
}

char* gets(char *src)
{
	size_t n = check_memory_access(dest);
        if(n == -1)
        {
                return NULL;
        }

	int i=0;
	char j;
	char *ch = src;
	while((j = getchar ()) != '\n' && i<n-1) 
	{
        	if (j == EOF)
		{
			if (ch == src || !feof(stdin))
                		return NULL;
        		break;
        	}
        	*ch++ = j; // character is stored at address, and pointer is incremented
        	i++;
    	}
	*ch = '\0'; //add null terminating character
}


