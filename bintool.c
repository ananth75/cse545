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
					return -1;
				}
			}
		}
	}
	return -1;

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
		    ins[i].in_use = true;
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

char* gets(char* src)
{
	size_t s = check_memory_access(src);
        //assert(s!= -1);
     if(s == -1)
     {
         return NULL;
     } 
    int j = 0;
    int c;
    while(j < s-1)
	{
		if(feof(stdin))
			break;

        if ((c = getchar()) == '\n')
		{
			break;
		}
		else
		{
			*src++ = c;
		}

	   j++;
	}
	if(feof(stdin) && (j == 0))
	{
		return NULL;
	}
    *(src) = '\0';
    return *src;    
}

char *fgets(char *s, int size, FILE *stream) 
{
	size_t s1 = check_memory_access(s);
	if(s1 == -1)
		return NULL;

	if(s1 < size) 
		size = s1;

    int j = 0;
    int c;
	while(j < size -1 && (c = getc(stream)) != EOF)
	{
		/*Assign the character read from file to dest string
		Man Page suggests that we store the NEWLINE too into
		the buffer before terminating */
		if((*s++ = c) == '\n')
		{
			break;
		}
		j++;
	}
	*s = '\0';
	if(j == 0 && c == EOF)
		return NULL;
	return s;
}

char* strcat(char *dest, const char *src)
{
	size_t ds = check_memory_access(dest);
	//size_t ss = check_memory_access(src);

	int j = 0;

	// Navigate to the end of First String
	while(*dest != '\0')
	{
		j++;
		*dest++;
	}

	/* Now Fill the destination until you encounter
       a NULL character or reach the Size limit of
       Destination string */

	while(j < ds -1 && *src != '\0')
	{
		*dest++ = *src++;
	}
	*dest = '\0';
	return dest;
}

