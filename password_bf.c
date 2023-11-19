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

    if (*ok) return;

    if (idx < (len - 1)) {
        #pragma omp parallel for private(c)
        for (c = 0; c < strlen(letters); ++c) {
            if (!*ok) {
                str[idx] = letters[c];
                iterate(hash1, hash2, str, idx + 1, len, ok);
            }
        }
    } else {
        #pragma omp parallel for private(c)
        for (c = 0; c < strlen(letters); ++c) {
            if (!*ok) {
                str[idx] = letters[c];
                MD5((byte *) str, strlen(str), hash2);
                if (strncmp((char *) hash1, (char *) hash2, MD5_DIGEST_LENGTH) == 0) {
                    #pragma omp critical
                    {
                        printf("found: %s\n", str);
                        *ok = 1; // Terminate when the password is found.
                    }
                }
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
	int r;
	char hash1_str[2*MD5_DIGEST_LENGTH+1];
	byte hash1[MD5_DIGEST_LENGTH]; // password hash
	byte hash2[MD5_DIGEST_LENGTH]; // string hashes

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
	//print_digest(hash1);

	// Generate all possible passwords of different sizes.
    int len;
    int ok = 0;
    double start_time = omp_get_wtime();
        #pragma omp parallel for private(len) shared(ok)
        for (len = 1; len <= lenMax; len++) {
            char local_str[MAX + 1];
            byte local_hash2[MD5_DIGEST_LENGTH];

            memset(local_str, 0, len + 1);
            iterate(hash1, local_hash2, local_str, 0, len, &ok);
        }
    
       double end_time = omp_get_wtime();
          printf("Time taken: %f seconds\n", end_time - start_time);
        if (ok == 1) {
            printf("Password found.\n");
        } else {
            printf("Password not found.\n");
        }

        return 0;
}
