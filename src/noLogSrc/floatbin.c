#include <stdlib.h>

#include <stdio.h>

#include <string.h>

#include <fcntl.h>

#include <unistd.h>

#include <sys/mman.h>

#include <stdint.h>

#include <errno.h>

#include <sys/stat.h>

#include <sys/types.h>

#include <math.h>

#include "plugin_api.h"

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

char big_end[33] = {
  0
};
char little_end[33] = {
  0
};
static char * lib_name = "libkesN3246.so";
static char * lib_action = "ÐŸÐ¾Ð¸ÑÐº Ñ„Ð°Ð¹Ð»Ð¾Ð², ÑÐ¾Ð´ÐµÑ€Ð¶Ð°Ñ‰Ð¸Ñ… Ð·Ð°Ð´Ð°Ð½Ð½Ð¾Ðµ Ð²ÐµÑ‰ÐµÑÑ‚Ð²ÐµÐ½Ð½Ð¾Ðµ Ñ‡Ð¸ÑÐ»Ð¾ Ð¾Ð´Ð¸Ð½Ð°Ñ€Ð½Ð¾Ð¹ Ñ‚Ð¾Ñ‡Ð½Ð¾ÑÑ‚Ð¸ Ð² Ð±Ð¸Ð½Ð°Ñ€Ð½Ð¾Ð¹ (little-endian Ð¸Ð»Ð¸ big-endian) Ñ„Ð¾Ñ€Ð¼Ðµ";
static char * lib_author = "ÐšÐ¾Ñ€ÑÐ°ÐºÐ¾Ð² Ð•Ð³Ð¾Ñ€ Ð¡ÐµÑ€Ð³ÐµÐµÐ²Ð¸Ñ‡";

#define FLOAT_TO_BIN_STR "float-bin" //Ð½Ð°Ð·Ð²Ð°Ð½Ð¸Ðµ Ð´Ð»Ð¸Ð½Ð½Ð¾Ð¹ Ñ„ÑƒÐ½ÐºÑ†Ð¸Ð¸

static struct plugin_option lon_op_arr[] = {
  {
    {
      FLOAT_TO_BIN_STR,
      required_argument,
      0,
      0
    },
    "looking for float numbers in file"
  },
};
//int plugin_get_info
int plugin_get_info(struct plugin_info * ppi) {
  if (!ppi) {
    fprintf(stderr, "ERROR: Ð½ÐµÐ¿Ñ€Ð°Ð²Ð¸Ð»ÑŒÐ½Ñ‹Ð¹ Ð°Ñ€Ð³ÑƒÐ¼ÐµÐ½Ñ‚\n");
    return -1;
  }
  ppi -> plugin_author = lib_author;
  ppi -> plugin_purpose = lib_action;
  ppi -> sup_opts_len = sizeof(lon_op_arr) / sizeof(lon_op_arr[0]);
  ppi -> sup_opts = lon_op_arr;

  return 0;
}
//Ð¿ÐµÑ€ÐµÐ²Ð¾Ð´ float ->bin bin -> little-bin
void convert(char * chislo) {
  float chisloinfloat = strtof(chislo, NULL);
  unsigned char * bit = (unsigned char * ) & chisloinfloat;
  int count = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 8; j++) {
      if ( * bit & 1) {
        little_end[31 - count] = '1';
        big_end[31 - count] = '1';
      } else {
        little_end[31 - count] = '0';
        big_end[31 - count] = '0';
      }
      * bit >>= 1;
      count++;
    }
    bit++;
  }
  for (int i = 0; i < 16; i += 8) { //Ð¸Ð· big-endian Ð¿ÐµÑ€ÐµÐ²Ð¾Ð´Ð¸Ð¼ Ð² little-endian
    int j = i;
    for (int k = 0; k < 8; k++) {
      char a = little_end[j + k];
      little_end[j + k] = little_end[24 + k - j];
      little_end[24 + k - j] = a;
    }
  }
}

char * change_endianness(char * buf) {
   char * DEBUG = getenv("LAB1DEBUG");
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


//KMP algorythm
int KMPAlg(char * string, char * substring, int N, int M) {
  int ret = -1;
  int B = 256;
  int ** dfa = calloc(B, sizeof(int * ));
  for (int i = 0; i < B; i++) {
    dfa[i] = calloc(M, sizeof(int));
  }
  dfa[(int)(substring[0])][0] = 1; //https://stackoverflow.com/questions/30548170/dfa-construction-in-knuth-morris-pratt-algorithm
  for (int X = 0, j = 1; j < M; j++) {
    for (int n = 0; n < B; n++) {
      dfa[n][j] = dfa[n][X];
    }
    dfa[(int)(substring[j])][j] = j + 1;
    X = dfa[(int)(substring[j])][X];
  }
  int m, j;
  for (m = 0, j = 0; m < N && j < M; m++) {
    j = dfa[(int)(string[m])][j];
  }
  if (j == M) ret = m - M;
  else ret = N;
  if (dfa) {
    for (int i = 0; i < B; i++) {
      if (dfa[i]) free(dfa[i]);
    }
    free(dfa);
  }
  if (ret != -1) {
    ret = ret / 8; // Ð¸Ð· Ð±Ð¸Ñ‚ Ð² Ð±Ð°Ð¹Ñ‚
    return ret;
  } else
    return ret; // Ð²Ð¾Ð·Ð²Ñ€Ð°Ñ‰Ð°ÐµÑ‚ Ð¾ÑˆÐ¸Ð±ÐºÑƒ 
}

//int plugin_process_file
int plugin_process_file(const char * fname, struct option in_opts[], size_t in_opts_len) {
  char * datfloat = NULL;
  char * file_p = NULL;
  char * filebinary_p = NULL;
  int ret = -1;
  char * DEBUG = getenv("LAB1DEBUG");
  if (!fname || !in_opts || !in_opts_len) {
    errno = EINVAL;
    return -1;
  }
  if (DEBUG) {
    for (size_t i = 0; i < in_opts_len; i++) { //Ð’Ñ‹Ð²Ð¾Ð´Ð¸Ð¼ Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸ÑŽ Ð¿Ð¾ Ð¿Ð¾Ð»ÑƒÑ‡ÐµÐ½Ð½Ð¾Ð¹ Ð¾Ð¿Ñ†Ð¸Ð¸
      fprintf(stderr, "DEBUG: %s: ÐŸÐ¾Ð»ÑƒÑ‡ÐµÐ½Ð° Ð¾Ð¿Ñ†Ð¸Ñ '%s' Ñ Ð°Ñ€Ð³ÑƒÐ¼ÐµÐ½Ñ‚Ð¾Ð¼ '%s'\n", lib_name, in_opts[i].name, (char * ) in_opts[i].flag);
    }
  }

  for (size_t i = 0; i < in_opts_len; i++) {
    if (strncmp(in_opts[i].name, "float-bin", 9) == 0) {
      datfloat = (char * ) in_opts[i].flag;
      break;
    }
  }

  if (datfloat == NULL) {
    fprintf(stderr, "ERROR: No option! Example: --float-bin 2.71\n");
    return -1;
  }

  int fd = open(fname, O_RDONLY);
  if (fd < 0) {
    return -1;
  }

  struct stat st = {
    0
  };
  int res = fstat(fd, & st); //Ð¸ÑÐ¿Ð¾Ð»ÑŒÐ·ÑƒÐµÐ¼ fstat, Ñ‡Ñ‚Ð¾Ð±Ñ‹ Ð¿Ð¾Ð»ÑƒÑ‡Ð¸Ñ‚ÑŒ Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸ÑŽ Ð¾ Ð½ÐµÐ¼
  if (res < 0) {
    ret = errno;
    goto END;
  }

  if (st.st_size == 0) { //Ð¿Ñ€Ð¾Ð²ÐµÑ€ÑÐµÐ¼, Ñ‡Ñ‚Ð¾ Ñ„Ð°Ð¹Ð» Ð½ÐµÐ½ÑƒÐ»ÐµÐ²Ð¾Ð¹
    if (DEBUG) {
      fprintf(stderr, "DEBUG: %s: Ð Ð°Ð·Ð¼ÐµÑ€ Ñ„Ð°Ð¹Ð»Ð° Ð´Ð¾Ð»Ð¶ÐµÐ½ Ð±Ñ‹Ñ‚ÑŒ  Ð±Ð¾Ð»ÑŒÑˆÐµ 0\n", lib_name);
    }
    ret = ERANGE;
    goto END;
  }

  float floatBin_ = atof(datfloat); // Ð¿Ð¾Ð»ÑƒÑ‡Ð¸Ð¼ Ð·Ð½Ð°Ñ‡ÐµÐ½Ð¸Ñ little_end Ð¸ big_end

  file_p = mmap(NULL, st.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0); //Ð¾Ñ‚Ð¾Ð±Ñ€Ð°Ð¶Ð°ÐµÐ¼ Ñ„Ð°Ð¹Ð» Ð½Ð° Ð¾Ð¿ÐµÑ€Ð°Ñ‚Ð¸Ð²Ð½ÑƒÑŽ Ð¿Ð°Ð¼ÑÑ‚ÑŒ, Ñ‡Ñ‚Ð¾Ð±Ñ‹ Ð¿Ñ€Ð°Ð²Ð¸Ð»ÑŒÐ½Ð¾ ÐµÐ³Ð¾ "Ñ‡Ð¸Ñ‚Ð°Ñ‚ÑŒ" (#include <sys/mman.h>)
  if (file_p == MAP_FAILED) {
    fprintf(stderr, "ÐžÑˆÐ¸Ð±ÐºÐ°: Mapping Ð¿Ñ€Ð¾ÑˆÐµÐ» Ð½ÐµÑƒÐ´Ð°Ñ‡Ð½Ð¾: %s\n", strerror(errno));
    return ret;
  }

  char * last_needle = NULL;
  char * last_needle_swapped = NULL;
  char * tmpFile = file_p;
  char *floatBinNewEnd = change_endianness((char*)&floatBin_);

  while (1) {
    char * p = memmem(file_p, st.st_size, (void*)&floatBin_, 4);
    if (!p) break;
    last_needle = p;
    st.st_size -= (p + 4) - file_p;
    file_p= p + 4;
  }

  file_p = tmpFile;
  while (1) {
    char * pp = memmem(file_p, st.st_size, floatBinNewEnd, 4);
    if (!pp) break;
    last_needle_swapped = pp;
    st.st_size -= (pp + 4) - file_p;
    file_p = pp + 4;
  }

    if (last_needle == NULL && last_needle_swapped == NULL) {
      // if (DEBUG)
          // printf("DEBUG: %s: Provided IPv4 address { %s } is NOT FOUND in { %s } file", lib_name, ipv4_bytes, input_file);
      ret = 1;
  } else {
      // if (DEBUG)
          // printf("DEBUG: %s: Provided IPv4 address { %s } is FOUND in { %s } file", lib_name, ipv4_bytes, input_file);
      ret = 0;
  }

 
  munmap(file_p, st.st_size);
  if(floatBinNewEnd) free(floatBinNewEnd);
  //END
  END:
    if (fd) {
      close(fd);
    }
  if (filebinary_p) {
    free(filebinary_p);
  }
  return ret;
}
