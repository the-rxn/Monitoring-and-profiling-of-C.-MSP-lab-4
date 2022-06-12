#include <stdlib.h>

#include <stdio.h>

#include <string.h>

#include <math.h>

#include <sys/mman.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <unistd.h>

#include <errno.h>

#include "plugin_api.h"

// A flag used to get verbose information 
// on library execution.

#define DEBUGFLAG "LAB1DEBUG"
#define OPT_IPV4 "ipv4-addr-bin"

// Some static variables to give information
// about library's purpose and author.

static char * lib_name = "libaotN3246.so";

static char * plugin_purpose = "Check if a file contains a specified IPv4 address";

static char * plugin_author = "TEAMfromBIT";

static struct plugin_option long_options[] = {
    {
        {
            OPT_IPV4,
            required_argument,
            0,
            0,
        },
        "IPv4 address"
    },
};

static int long_options_len = sizeof(long_options) / sizeof(long_options[0]);

//gcc -fsanitize=address -g -o ipv4lib ipv4lib.c 
//https://en.wikipedia.org/wiki/AddressSanitizer

//
//  Private functions
//

static char * change_endianness(char * );
//static char * convert_to_IPv4(char * );
static int check(char * , const char * );

//
//  API functions
//
int size_int(int x)
{
    if (x==0) return 1;
    int digit=0;
    for (int i=1;i<=x;i*=10) digit++;
    return digit;
}

char * convert_to_IPv4(char * address) {
    char * DEBUG = getenv(DEBUGFLAG);

    uint a, b, c, d;
    //char* ss=strtok(address, "\n");
    //printf("DEBUGE %s\n",ss);
    a=atoi(address);
    int len=size_int(a);
    b=atoi(address+len+1);
    len+=size_int(b);
    c=atoi(address+len+2);
    len+=size_int(c);
    d=atoi(address+len+3);
    
   if (!(a<256 &&b < 256 && c < 256 && d < 256)) 
    {
       if (DEBUG) {
           fprintf(stderr, "DEBUG: %s: Address value should be [0-255].[0-255].[0-255].[0-255]\n",
               lib_name);
       }
       return NULL;
   }
    // a=127; b=0; c=0; d=1;

    char * resultChar = malloc(5);
    if (resultChar) {
        resultChar[0] = a;
        resultChar[1] = b;
        resultChar[2] = c;
        resultChar[3] = d;
        resultChar[4] = 0;

        if(DEBUG){
            printf("DEBUG: %s: Converted to IPv4 succesfully\n", lib_name);
            printf("DEBUG: %s: resultchar = %s\n", lib_name, resultChar);
        }
        return resultChar;
    }
    else{ 
        if (DEBUG)
            printf("DEBUG: %s: memory allocation failed\n", lib_name);
        return NULL;
    }
}


int plugin_get_info(struct plugin_info * ppi) {
    if (!ppi) {
        fprintf(stderr, "ERROR: invalid argument\n");
        return -1;
    }

    ppi -> plugin_purpose = plugin_purpose;
    ppi -> plugin_author = plugin_author;
    ppi -> sup_opts_len = long_options_len;
    ppi -> sup_opts = long_options;

    return 0;
}

int plugin_process_file(const char * fname,
    struct option in_opts[],
    size_t in_opts_len) {
    // Return error by default
    int ret = -1;

    char * DEBUG = getenv(DEBUGFLAG);
    if(DEBUG) printf("DEBUG: %s: Entered plugin_process_file()\n", lib_name);
    if (!fname || !in_opts || !in_opts_len) {
        errno = EINVAL;
        return -1;
    }

    if (DEBUG) {
        for (size_t i = 0; i < in_opts_len; i++) {
            fprintf(stderr, "DEBUG: %s: Got option '%s' with arg '%s' in_opts_len = '%ld'\n",
                lib_name, in_opts[i].name, (char * ) in_opts[i].flag, in_opts_len);
        }
    }

    char * address = (char *)calloc(4*4, sizeof(char));
    if(!address) return -1;

    address[4] = 0;

    // Check if option is supplied multiple times 
    #define OPT_CHECK(opt_var)\
    if (got_##opt_var) {\
        if (DEBUG) {\
            \
            fprintf(stderr, "DEBUG: %s: Option '%s' was already supplied\n", \
                lib_name, in_opts[i].name);\
        }\
        errno = EINVAL;\
        return -1;\
    }\
    else {\
        \
        opt_var = (char * ) in_opts[i].flag;\
        got_##opt_var = 1;\
    }\

    for (size_t i = 0; i < in_opts_len; i++) {
        if (!strcmp(in_opts[i].name, OPT_IPV4)) {
            //OPT_CHECK(address)
            memcpy(address, (char *) in_opts[i].flag, 4*4);
        } else {
            errno = EINVAL;
            return -1;
        }
    }

    if (DEBUG) {
        fprintf(stderr, "DEBUG: %s: Input address = %s\n",
            lib_name, address);
    }

    // The main thing
    ret = check(address, fname);
    if(address) free(address);
    
    return ret;
}

/*
 * The memmem() function finds the start of the first occurrence of the
 * substring 'needle' of length 'nlen' in the memory area 'haystack' of
 * length 'hlen'.
 *
 * The return value is a pointer to the beginning of the sub-fileContents, or
 * NULL if the substring is not found.
 */
void * memmem(const void * haystack, size_t hlen,
    const void * needle, size_t nlen) {
    int needle_first;
    const void * p = haystack;
    size_t plen = hlen;

    if (!nlen)
        return NULL;

    needle_first = * (unsigned char * ) needle;

    while (plen >= nlen && (p = memchr(p, needle_first, plen - nlen + 1))) {
        if (!memcmp(p, needle, nlen))
            return (void * ) p;

        p++;
        plen = hlen - (p - haystack);
    }

    return NULL;
}

//Incorrect file reading...
/*
char * readFile(const char * p) {
    char * DEBUG = getenv(DEBUGFLAG);
    // Read the file in rb mode and put evetything into the fileContents array.
    FILE * f = fopen(p, "rb");
    struct stat st;
    if (!f) {
        // errno is set by open()
        return NULL;
    }

    fstat(fileno(f), &st);
    char * fileContents = (char *)calloc(st.st_size + 1, sizeof(char));
    
    if (!fileContents) {
        if (DEBUG)
            printf("DEBUG: %s: memory allocation failed\n", lib_name);
        return NULL;
    }
    
        if(fread(fileContents, 1, st.st_size, f)){
        fclose(f);
        fileContents[st.st_size] = 0;
    }

    if (DEBUG)
            printf("DEBUG: %s: File is read successfully\n", lib_name);
    return fileContents;

}
*/
char * change_endianness(char * buf) {
    char * DEBUG = getenv(DEBUGFLAG);
    char * resultChar = malloc(5);
    if (resultChar) {
        resultChar[4] = 0;
        resultChar[0] = buf[3]; // move byte 3 to byte 0
        resultChar[1] = buf[2]; // move byte 1 to byte 2
        resultChar[2] = buf[1]; // move byte 2 to byte 1
        resultChar[3] = buf[0]; // byte 0 to byte 3

        if (DEBUG)
            printf("DEBUG: %s: Endianness is changed successfully\n", lib_name);

        return resultChar;
    } else {
        if (DEBUG)
            printf("DEBUG: %s: memory allocation failed", lib_name);
        return NULL;
    }
}



int check(char * input_address, const char * input_file) {
    char * DEBUG = getenv(DEBUGFLAG);

    // Return an error by default too :)
    int ret = -1;

    if (DEBUG) {
        printf("DEBUG: %s: Provided IP(in string):  %s\n", lib_name, input_address);
    }
    //Read file here
    FILE * f = fopen(input_file, "rb");
    struct stat st;
    if (!f) {
        // errno is set by open()
        return 0;
    }

    fstat(fileno(f), &st);
    char * fileContents = (char *)calloc(st.st_size + 1, sizeof(char));
    
    if (!fileContents) {
        if (DEBUG)
            printf("DEBUG: %s: memory allocation failed\n", lib_name);
        return 0;
    }
    
        if(fread(fileContents, 1, st.st_size, f)){
        fclose(f);
        fileContents[st.st_size] = 0;
    }

    if (DEBUG)
            printf("DEBUG: %s: File is read successfully\n", lib_name);

    // Split from . between octets
    char * ipv4_bytes = convert_to_IPv4(input_address);

    if (ipv4_bytes && fileContents) {

        // Change endianness of ipv4 address:
        char * changed;
        changed = change_endianness(ipv4_bytes);

        // Only for debug:
        if (DEBUG) {
            //printf("DEBUG: %s: fileContents: %s\n", lib_name, fileContents);
            printf("DEBUG: %s: ipv4_bytes: %s, st.st_size = %ld\n", lib_name, ipv4_bytes, st.st_size);
            printf("DEBUG: %s: ipv4_bytes with changed endianness: %s\n", lib_name, changed);
        }
/*
*/
        char * last_needle = NULL;
        char * last_needle_swapped = NULL;

        char * tmp = fileContents;

        while (1) {
            char * p = memmem(fileContents, st.st_size + 1, ipv4_bytes, 4);
            if (!p) break;
            last_needle = p;
            st.st_size -= (p + 4) - fileContents;
            fileContents = p + 4;
        }

        fileContents = tmp;

        // Do the same but with changed endianness
        while (1) {
            char * pp = memmem(fileContents, st.st_size + 1, changed, 4);
            if (!pp) break;
            last_needle_swapped = pp;
            st.st_size -= (pp + 4) - fileContents;
            fileContents = pp + 4;
        }

        fileContents = tmp;

        if (last_needle == NULL && last_needle_swapped == NULL) {
            if (DEBUG)
                printf("DEBUG: %s: Provided IPv4 address { %s } is NOT FOUND in { %s } file", lib_name, ipv4_bytes, input_file);
            ret = 1;
        } else {
            if (DEBUG)
                printf("DEBUG: %s: Provided IPv4 address { %s } is FOUND in { %s } file", lib_name, ipv4_bytes, input_file);
            ret = 0;
        }
        free(changed);
        free(fileContents);
        free(ipv4_bytes);
    }
    return ret;
};
