#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define MAX 10


typedef unsigned char byte;

char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

/*
 * Print a digest of MD5 hash.
 */
void print_digest(byte * hash){
    int x;
    
    for(x = 0; x < MD5_DIGEST_LENGTH; x++)
        printf("%02x", hash[x]);
    printf("\n");
}

/*
 * This procedure generate all combinations of possible letters
 */
void iterate(byte * hash1, byte * hash2, char *str, int idx, int len, int *ok) {
    int c;
    // 'ok' determines when the algorithm matches.
    if(*ok) return;
    if (idx < (len - 1)) {
#pragma omp parallel firstprivate(hash1, hash2, idx, len) shared(ok)
        {
            // Iterate for all letter combination.
        for (c = 0; c < strlen(letters) && !(*ok); ++c) {
                     char *local_str = (char *)malloc((MAX + 1) * sizeof(char));
                     memcpy(local_str, str, MAX + 1);

                     local_str[idx] = letters[c];
                   //  printf("thread: %d, len: %d, idx: %d, str: %s, c: %d, char: %c\n", omp_get_thread_num(), len, idx, local_str, c, local_str[idx]);
            #pragma omp task firstprivate(local_str)
                {
                     iterate(hash1, hash2, local_str, idx + 1, len, ok);
                     free(local_str);
                 }
              }
            }
    } else {
        // Include all last letters and compare the hashes.
        //#pragma omp for
        for (c = 0; c < strlen(letters); ++c) {
            if (*ok == 1) {
                c = 37;
            }
            byte curHash[MD5_DIGEST_LENGTH];
            hash2 = curHash;
            str[idx] = letters[c];
            MD5((byte *) str, strlen(str), hash2);
            
            // Compara hash1 com curHash
            if (memcmp(hash1, hash2, MD5_DIGEST_LENGTH) == 0) {
                printf("found: %s\n", str);
                *ok = 1;
            }
            
        }
    }
}

/*
 * Convert hexadecimal string to hash byte.
 */
void strHex_to_byte(char * str, byte * hash){
    char * pos = str;
    int i;
    
    for (i = 0; i < MD5_DIGEST_LENGTH/sizeof *hash; i++) {
        sscanf(pos, "%2hhx", &hash[i]);
        pos += 2;
    }
}

int main(int argc, char **argv) {
    char str[MAX+1];
    int lenMax = MAX;
    int len;
    int ok = 0, r;
    char hash1_str[2*MD5_DIGEST_LENGTH+1];
    byte hash1[MD5_DIGEST_LENGTH]; // password hash
    byte hash2[MD5_DIGEST_LENGTH]; // string hashes
    
    omp_set_num_threads(16);

    // Input:
    r = scanf("%s", hash1_str);
    // Check input.
    if (r == EOF || r == 0)
    {
        fprintf(stderr, "Error!\n");
        exit(1);
    }
    
    // Convert hexadecimal string to hash byte.
    strHex_to_byte(hash1_str, hash1);
    
    memset(hash2, 0, MD5_DIGEST_LENGTH);
    
    // Generate all possible passwords of different sizes
    
    double start_time = omp_get_wtime();
   

        for(len = 1; len <= lenMax; len++){
            memset(str, 0, len+1);
            iterate(hash1, hash2, str, 0, len, &ok);
            
        }
    
    
    double end_time = omp_get_wtime();
       printf("Time taken: %f seconds\n", end_time - start_time);
    
}
