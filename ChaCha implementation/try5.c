/**
 * @file try5.c
 * @author Sanchita Saha (you@domain.com)
 * @brief this is the final program for classical
 * @version 0.1
 * @date 2022-03-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>

#define MAX_LEN 1000
#define MIN(a,b) (a)<(b)?(a):(b)
#define ROUNDS 20
#define ROTL(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
#define QR(a, b, c, d)(		\
	a += b,  d ^= a,  d = ROTL(d,16),	\
	c += d,  b ^= c,  b = ROTL(b,12),	\
	a += b,  d ^= a,  d = ROTL(d, 8),	\
	c += d,  b ^= c,  b = ROTL(b, 7))

void printStream(uint8_t * stream, int length);
uint8_t * stringToHex(char *str, int length);
void printHexStream(uint8_t * stream, int length);
uint8_t * XORStreams(uint8_t * plaintext, uint8_t * keystream, int plain_length);
uint8_t* chacha(int num_blocks);
uint32_t * getNonceBlocks(int n);
void printMatrixState(uint32_t state[4][4]);
void serialize(uint32_t state[4][4], uint8_t*arr, int start_length);
void oneBlock(uint32_t state[4][4]);
void printMatrixState(uint32_t state[4][4]);

int main(){
    uint8_t *plaintext, *keystream, *ciphertext;
    
    // reading plaintext
    char pt[MAX_LEN];
    printf("Enter plain text (as string): ");
    scanf("%[^\n]s", pt);
    int length=strlen(pt);
    plaintext = stringToHex(pt, length);

    int num_blocks = ceil(length/64.0);
    keystream = chacha(num_blocks);

    ciphertext = XORStreams(plaintext, keystream, length);

    // printf("\n--------------------------------------------------\n");
    // printf("\nPlaintext (%d)\n",length);
    // printHexStream(plaintext, length);
    // printf("\nKeystream (%d)\n",length);
    // printHexStream(keystream, length);
    // printf("\nCiphertext (%d)\n",length);
    // printHexStream(ciphertext, length);
    // printf("\n\n--------------------------------------------------\n");
}

uint8_t* chacha(int num_blocks){
    // setting initial state
    uint32_t state[4][4];

    // setting first row of constants
    char constant[]="expand 32-byte k";
    for(int i=0;i<4;i++)                        
        state[0][i]=(*(uint32_t*)(constant+i*4));
    
    // setting 2nd and 3rd row of key values
    printf("Do you want to enter key in hex? [0]|1: ");
    int ch;
    scanf("%d%*c",&ch);
    if(ch==1){
        for(int i=0,k=1;i<8;i++)  
        {
            printf("Enter hex code for block no. %d: ", i);
            uint32_t temp;
            scanf("%x",&temp);
            if(i==4)
                ++k;
            state[k][i%4]=ntohl(temp);
        }
    }
    else{
        printf("Enter key (32 characters) Eg: \"this is chacha cipher classical.\": ");
        size_t n=32+1;                                  // extra 1 for null character
        char key[n];                                
        fgets(key, n, stdin);
        fflush(stdin);
        for(int k=1;k<3;k++)                            // 2nd and 3rd row is key
            for(int i=0;i<4;i++)
                state[k][i]=(*(uint32_t*)(key+i*4+(k-1)*4*4));
    }
        
    int n_nonce=3;                                      // no of blocks for nonce
    uint32_t *nonce=getNonceBlocks(n_nonce);
    for(int i=0;i<n_nonce;i++)                          // 4th row is remaining block nonce
        state[3][i+1]=nonce[i];            

    uint8_t *arr=(uint8_t*)malloc(4*4*4*sizeof(uint8_t)*num_blocks);
    int length=4*4*4;

    printf("\n\nKeystream generation----------------------------\n");

    for(int i=0;i<num_blocks;i++){
        
        uint32_t new_state[4][4];

        for(int k=0;k<4;k++){
            for(int j=0;j<4;j++){
                if(k==3 && j==0)
                    continue;
                new_state[k][j]=state[k][j];
            }
        }

        oneBlock(new_state);
        serialize(new_state, arr, length*i);

        printf("Keystream (num_block:%d):\n",i);
        printHexStream(arr+length*i, length);
    }
    return arr;
}

void oneBlock(uint32_t state[4][4]){
    // setting 4th row of counter and nonce
    static uint32_t counter = 1;                        // make counter static
    state[3][0]=counter++;

    printf("\nInitial state\n");
    printMatrixState(state);

    uint32_t init_state[4][4];

    // printf("copying initial state...");
    for(int k=0;k<4;k++){                               // printing
        for(int i=0;i<4;i++)
            init_state[k][i]=state[k][i];
        }
    // printf("copied.\n");

    for(int i = 0; i < ROUNDS; i += 2) {
        QR(state[0][0], state[1][0], state[2][0], state[3][0]);
        printf("\n\ncolumn 0 Round:%d\n",i);
        printMatrixState(state);
        QR(state[0][1], state[1][1], state[2][1], state[3][1]);
        printf("\n\ncolumn 1 Round:%d\n",i);
        printMatrixState(state);
        QR(state[0][2], state[1][2], state[2][2], state[3][2]);
        QR(state[0][3], state[1][3], state[2][3], state[3][3]);

        printf("\n\ncolumn Round:%d\n",i);
        printMatrixState(state);

        QR(state[0][0], state[1][1], state[2][2], state[3][3]);
        QR(state[0][1], state[1][2], state[2][3], state[3][0]);
        QR(state[0][2], state[1][3], state[2][0], state[3][1]);
        QR(state[0][3], state[1][0], state[2][1], state[3][2]);

        printf("\n\ndiagonal Round:%d\n",i);
        printMatrixState(state);
    }

    printf("\n\nAfter 20 rounds rotation\n");
    printMatrixState(state);
    printf("\n\n");

    // printf("Final addition\n");
    for(int k=0;k<4;k++){                               // printing
        for(int i=0;i<4;i++)
            state[k][i]+=init_state[k][i];
        }
    
    printf("Final state\n");
    printMatrixState(state);
}

void serialize(uint32_t state[4][4], uint8_t*arr, int start_length){
    // printf("Serializing the keystream\n");
    
    int z=start_length;
    for(int k=0;k<4;k++){                               // each row
        for(int i=0;i<4;i++){                           // each column
            uint8_t *temp=(uint8_t*)&(state[k][i]);
            // printf("%08x::::: \n",state[k][i]);
            for(int j=0;j<4;j++){
                // printf("%x %c\n", temp[j], temp[j]);
                arr[z]=temp[j];
                z+=1;
            }
        }
    }
}

uint8_t * XORStreams(uint8_t * plaintext, uint8_t * keystream, int plain_length){
    uint8_t * ciphertext=(uint8_t*)malloc(plain_length*sizeof(uint8_t));
    for(int i=0;i<plain_length;i++){
        ciphertext[i]=plaintext[i]^keystream[i];
    }
    return ciphertext;
}

uint8_t * stringToHex(char *str, int length){
    uint8_t * plaintext=(uint8_t*)malloc(length*sizeof(uint8_t));
    for(int i=0;str[i]!='\0';i++){
        plaintext[i]=(uint8_t)str[i];
    }
    return plaintext;
}

uint32_t * getNonceBlocks(int n){
    uint32_t * nonce=(uint32_t*)malloc(n*sizeof(uint32_t));
    printf("Do you want to enter nonce? [0]|1: ");
    int ch;
    scanf("%d",&ch);
    fflush(stdin);
    if(ch==1){
        for(int i=0;i<n;i++)  
        {
            printf("Enter hex code for block no. %d: ", i);
            uint32_t temp;
            scanf("%x",&temp);
            nonce[i]=ntohl(temp);
        }
        return nonce;
    }
    for(int i=0;i<n;i++)                        
        nonce[i]=(uint32_t)rand();
    return nonce;
}

void printMatrixState(uint32_t state[4][4]){
    for(int k=0;k<4;k++){                       // printing
        for(int i=0;i<4;i++)
            printf("%08x | ",state[k][i]);
        printf("\n");
        }
}

void printHexStream(uint8_t * stream, int length){
    int z=0;
    while(z<length){
        printf("%02x:",stream[z]);
        ++z;
        if(z%16==0)
            printf("\n");
    }
}