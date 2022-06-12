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


char big_end[33] = {0};
char little_end[33] = {0};
static char* lib_name = "libkesN3246.so";
static char* lib_action = "ÐŸÐ¾Ð¸ÑÐº Ñ„Ð°Ð¹Ð»Ð¾Ð², ÑÐ¾Ð´ÐµÑ€Ð¶Ð°Ñ‰Ð¸Ñ… Ð·Ð°Ð´Ð°Ð½Ð½Ð¾Ðµ Ð²ÐµÑ‰ÐµÑÑ‚Ð²ÐµÐ½Ð½Ð¾Ðµ Ñ‡Ð¸ÑÐ»Ð¾ Ð¾Ð´Ð¸Ð½Ð°Ñ€Ð½Ð¾Ð¹ Ñ‚Ð¾Ñ‡Ð½Ð¾ÑÑ‚Ð¸ Ð² Ð±Ð¸Ð½Ð°Ñ€Ð½Ð¾Ð¹ (little-endian Ð¸Ð»Ð¸ big-endian) Ñ„Ð¾Ñ€Ð¼Ðµ";
static char* lib_author = "ÐšÐ¾Ñ€ÑÐ°ÐºÐ¾Ð² Ð•Ð³Ð¾Ñ€ Ð¡ÐµÑ€Ð³ÐµÐµÐ²Ð¸Ñ‡";

#define  FLOAT_TO_BIN_STR "bug-float-bin"		//Ð½Ð°Ð·Ð²Ð°Ð½Ð¸Ðµ Ð´Ð»Ð¸Ð½Ð½Ð¾Ð¹ Ñ„ÑƒÐ½ÐºÑ†Ð¸Ð¸

static struct plugin_option lon_op_arr[] = {
        {
            {
                FLOAT_TO_BIN_STR,
                required_argument,
                0, 0
            },
            "looking for float numbers in file"
        },
};
//Ñ„Ð°Ð¹Ð»Ð¾Ð²Ñ‹Ðµ Ð±Ð°Ð¹Ñ‚Ñ‹ Ð² ÑÑ‚Ñ€Ð¾ÐºÑƒ
/*char* filebytetostring_bin(char *input,int len){
    char *tmp=calloc(len*8+1,sizeof(char));
    char n;
    for (int i=0,m=8;i<m && m<len;i++)
    {
        n=input[i];
        for(int j=0;j<8;j++) // Ð¸ÑÐ¿Ð¾Ð»ÑŒÐ·ÑƒÐµÐ¼ strncat Ñ‡Ñ‚Ð¾Ð±Ñ‹ Ðº ÑÑ‚Ñ€Ð¾ÐºÐµ "Ð½Ð° Ð²Ñ‹Ñ…Ð¾Ð´" Ð¿Ñ€Ð¸Ð¿Ð¸ÑÑ‹Ð²Ð°Ñ‚ÑŒ Ñ†Ð¸Ñ„Ñ€Ñ‹ Ð²Ð¾ Ð²Ñ€ÐµÐ¼Ñ Ð¿Ñ€Ð¾Ñ…Ð¾Ð¶Ð´ÐµÐ½Ð¸Ñ Ñ†Ð¸ÐºÐ»Ð°
        {
            if(n & 0x80000000)
                strncat(tmp, "1", 2);
            else
                strncat(tmp, "0", 2);
            n<<=1;
        }
    }
    return tmp;
}
*/
//int plugin_get_info
int plugin_get_info(struct plugin_info* ppi)
{
    	if (!ppi) {
        	fprintf(stderr, "ERROR: Ð½ÐµÐ¿Ñ€Ð°Ð²Ð¸Ð»ÑŒÐ½Ñ‹Ð¹ Ð°Ñ€Ð³ÑƒÐ¼ÐµÐ½Ñ‚\n");
        	return -1;
        }
	ppi->plugin_author = lib_author;
	ppi->plugin_purpose = lib_action;
	ppi->sup_opts_len = sizeof(lon_op_arr)/sizeof(lon_op_arr[0]);
	ppi->sup_opts = lon_op_arr;
	
	return 0;
}
//Ð¿ÐµÑ€ÐµÐ²Ð¾Ð´ float ->bin bin -> little-bin
void convert(char* chislo){  
    float chisloinfloat = strtof(chislo, NULL);
    unsigned char* bit = (unsigned char*)&chisloinfloat;
    int count = 0;
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 8; j++){
            if (*bit & 1){
                little_end[31-count] = '1';
                big_end[31-count] = '1';
            }
            else{
                little_end[31-count] = '0';
                big_end[31-count] = '0';
            }
            *bit >>= 1;
            count++;
        }
        bit++;
    }
    for(int i=0; i<16; i+=8){					//Ð¸Ð· big-endian Ð¿ÐµÑ€ÐµÐ²Ð¾Ð´Ð¸Ð¼ Ð² little-endian
    	int j=i;
    	for(int k=0;k<8;k++){
    	char a=little_end[j+k];
    	little_end[j+k]=little_end[24+k-j];
    	little_end[24+k-j]=a;
    	}
}
}
//KMP algorythm
int KMPAlg(char *string, char *substring, int N,int M){
    int ret = -1;
    int B = 256;
    int **dfa = calloc(B, sizeof(int*));
    for(int i = 0; i<B; i++){
        dfa[i] = calloc(M, sizeof(int));
    }
    dfa[(int)(substring[0])][0] = 1;  //https://stackoverflow.com/questions/30548170/dfa-construction-in-knuth-morris-pratt-algorithm
    for(int X=0, j=1; j<M; j++){
        for(int n=0; n<B; n++){
            dfa[n][j] = dfa[n][X];
        }
        dfa[(int)(substring[j])][j] = j+1;
        X = dfa[(int)(substring[j])][X];
    }
    int m, j;
    for(m=0, j=0; m<N && j<M; m++){
        j = dfa[(int)(string[m])][j];
    }
    if (j==M) ret = m-M;
    else ret = N;
    if(dfa){
        for(int i=0; i<B; i++){
            if(dfa[i]) free(dfa[i]);
        }
        free(dfa);
    }
    if(ret!=-1){
    ret=ret/8	;// Ð¸Ð· Ð±Ð¸Ñ‚ Ð² Ð±Ð°Ð¹Ñ‚
        return ret;
        }
    else
        return ret; // Ð²Ð¾Ð·Ð²Ñ€Ð°Ñ‰Ð°ÐµÑ‚ Ð¾ÑˆÐ¸Ð±ÐºÑƒ 
}
//int plugin_process_file
int plugin_process_file(const char* fname, struct option in_opts[], size_t in_opts_len){
	char* datfloat = NULL;
	char* file_p = NULL;
	char* filebinary_p = NULL;
	int ret = -1;
	char* DEBUG = getenv("LAB1DEBUG");
	 if (!fname || !in_opts || !in_opts_len) {
        errno = EINVAL;
        return -1;
    }
    if (DEBUG) {
        for (size_t i = 0; i < in_opts_len; i++) {				//Ð’Ñ‹Ð²Ð¾Ð´Ð¸Ð¼ Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸ÑŽ Ð¿Ð¾ Ð¿Ð¾Ð»ÑƒÑ‡ÐµÐ½Ð½Ð¾Ð¹ Ð¾Ð¿Ñ†Ð¸Ð¸
            fprintf(stderr, "DEBUG: %s: ÐŸÐ¾Ð»ÑƒÑ‡ÐµÐ½Ð° Ð¾Ð¿Ñ†Ð¸Ñ '%s' Ñ Ð°Ñ€Ð³ÑƒÐ¼ÐµÐ½Ñ‚Ð¾Ð¼ '%s'\n",lib_name, in_opts[i].name, (char*)in_opts[i].flag);
        }
    }
    
    for(size_t i=0; i<in_opts_len; i++){
        if(strncmp(in_opts[i].name, "bug-float-bin", 9)==0){
            datfloat = (char *)in_opts[i].flag;
            break;
        } 
       }
       
       	if(datfloat == NULL){
        fprintf(stderr, "ERROR: No option! Example: --float-bin 2.71\n");
        return -1;
    }
    
    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    
    struct stat st = {0};
    int res = fstat(fd, &st); //Ð¸ÑÐ¿Ð¾Ð»ÑŒÐ·ÑƒÐµÐ¼ fstat, Ñ‡Ñ‚Ð¾Ð±Ñ‹ Ð¿Ð¾Ð»ÑƒÑ‡Ð¸Ñ‚ÑŒ Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸ÑŽ Ð¾ Ð½ÐµÐ¼
    if (res < 0) {
        ret = errno;
        goto END;
    }

    
    if (st.st_size == 0) {			//Ð¿Ñ€Ð¾Ð²ÐµÑ€ÑÐµÐ¼, Ñ‡Ñ‚Ð¾ Ñ„Ð°Ð¹Ð» Ð½ÐµÐ½ÑƒÐ»ÐµÐ²Ð¾Ð¹
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: Ð Ð°Ð·Ð¼ÐµÑ€ Ñ„Ð°Ð¹Ð»Ð° Ð´Ð¾Ð»Ð¶ÐµÐ½ Ð±Ñ‹Ñ‚ÑŒ  Ð±Ð¾Ð»ÑŒÑˆÐµ 0\n",lib_name);
        }
        ret = ERANGE;
        goto END;
    }
    
    convert(datfloat); // Ð¿Ð¾Ð»ÑƒÑ‡Ð¸Ð¼ Ð·Ð½Ð°Ñ‡ÐµÐ½Ð¸Ñ little_end Ð¸ big_end

    
    file_p = mmap(NULL, st.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0);//Ð¾Ñ‚Ð¾Ð±Ñ€Ð°Ð¶Ð°ÐµÐ¼ Ñ„Ð°Ð¹Ð» Ð½Ð° Ð¾Ð¿ÐµÑ€Ð°Ñ‚Ð¸Ð²Ð½ÑƒÑŽ Ð¿Ð°Ð¼ÑÑ‚ÑŒ, Ñ‡Ñ‚Ð¾Ð±Ñ‹ Ð¿Ñ€Ð°Ð²Ð¸Ð»ÑŒÐ½Ð¾ ÐµÐ³Ð¾ "Ñ‡Ð¸Ñ‚Ð°Ñ‚ÑŒ" (#include <sys/mman.h>)
    if(file_p == MAP_FAILED){
        fprintf(stderr, "ÐžÑˆÐ¸Ð±ÐºÐ°: Mapping Ð¿Ñ€Ð¾ÑˆÐµÐ» Ð½ÐµÑƒÐ´Ð°Ñ‡Ð½Ð¾: %s\n", strerror(errno));
        return ret;
    }
    
    char *tmp=calloc(st.st_size*8+1,sizeof(char));
    char n;
    int i=0;
    int m=256;
    for (;i<m && m<st.st_size;i++)
    {
     
        n=file_p[i];
        for(int j=0;j<8;j++) 
        {
            if(n & 0x80000000)
                strncat(tmp, "1", 2);
            else
                strncat(tmp, "0", 2);
            n<<=1;
           
        }
    	}
    	filebinary_p = tmp;
    	int big_end_chislo_found=KMPAlg(filebinary_p,big_end,st.st_size*8,32); 
    	int lit_end_chislo_found=KMPAlg(filebinary_p,little_end,st.st_size*8,32);
    		
    		if (big_end_chislo_found==st.st_size) {
    		ret=1;
    		}
    		else {
    		ret=0;
    		}
    		
    			if(DEBUG && ret==0) {
			fprintf(stderr,"DEBUG: %s: Ð½Ð°Ð¹Ð´ÐµÐ½ %s big-endian Ð² %s\n",lib_name,big_end,fname);
			
			}
    			if (ret!=0){
        			if (lit_end_chislo_found==st.st_size) {
        			ret=1;	
        			}
        			else ret=0;
        			
        			}
        		if(DEBUG && ret==0)  {
        		fprintf(stderr,"DEBUG: %s: Ð½Ð°Ð¹Ð´ÐµÐ½ %s little-endian Ð² %s\n",lib_name, little_end,fname);
        		
        		}
        		
    			if(DEBUG && ret==1)  {
    			fprintf(stderr,"DEBUG: %s: Ð½Ðµ Ð½Ð°Ð¹Ð´ÐµÐ½Ð¾!\n", lib_name);
    			
    			}
    			
    	
    	



//END
END:
	if(fd)
	{
        close(fd);
        }
    	if(filebinary_p) {
        free(filebinary_p);
    }
    return ret;
}
