/**
 * @file classicalSmallGIFT.c
 * @author Sanchita Saha
 * @brief 
 * @version 0.1
 * @date 2023-01-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// #include <arpa/inet.h>
#include <winsock2.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define Len_x 8
#define MAX(a,b) ((a)>(b))?(a):(b)

uint8_t smallGIFT(uint8_t x);
uint8_t getInnerProduct(uint8_t x, uint8_t fx, uint8_t coeff[2]);
void bin_8(uint8_t n);

unsigned int pbox[8] ={ 0,3,5,7,2,4,6,1};//Permutation BP

int main(){
    uint8_t x=0, fx;
    uint8_t coeff[2]={0,0};   //coeff[0] is LSB, coeff[1] is MSB
    uint8_t target;
    uint8_t count_array[65536][2];
    int max=-1;

    // printf("Enter coefficient string c in space separated hex form like \"bc ff\": ");

    // scanf("%x %x", &coeff[0], &coeff[1]);


    do{     // loop for outer coefficient
        // printf("%4d ", coeff[1]);
        int sum=0;
        max=-1;
        uint8_t index_max=0;
        do{     // loop for inner coefficient
            int count1=0, count0=0;
            x=0;
            do{     // loop for all x for a particular coefficient
                fx=smallGIFT(x);
                // printf("%02x:%02x ",x,fx);

                target = getInnerProduct(x, fx, coeff);
                // printf("\nTarget: %x ",target);
                if(target!=0)
                    count1++;
                else
                    count0++;
                x++;
            }while(x!=0);

            // printf("\nCoefficient: %x %x\n",coeff[0], coeff[1]);
            // printf("count0:1\t%x %x %d %d %d\n", coeff[0], coeff[1], count0, count1, abs(count0-count1));

            unsigned int diff=-1;
            if(count0>count1)
                diff=count0-count1;
            else    
                diff=count1-count0;

            double z_u=diff/((long double)256.0); //256=2^8
            printf("\"");
            // printing binary
            bin_8(coeff[0]);
            printf(" ");
            bin_8(coeff[1]);
            printf("\" %d %lf\n", diff, (double)z_u);


            // printf("%d\n",count0);
            // printf("%4d ", count0-count1);
            if(coeff[0]!=0){
                if(abs(count0-count1)>=max){
                    max=abs(count0-count1);
                    index_max=coeff[0];
                }
            }
            sum+=(count0-count1);
            count_array[(int)coeff[0]+(int)coeff[1]*16][0]=count0;
            count_array[(int)coeff[0]+(int)coeff[1]*16][1]=count1;
            
            coeff[0]++;
        }while(coeff[0]!=0);
        // printf("%4d, ",max);
        // printf("\tsum:%4d\tmax_abs_except_1st:%4d\tmax_index:%4x\n",sum, max, index_max);
        coeff[0]=0;
        coeff[1]++;
    }while(coeff[1]!=0);

    // printf("\nMax:%d", max);

}

uint8_t smallGIFT(uint8_t x){
    // SBox
    uint8_t sbox_arr[] = {1,10,4,12,6,15,3,9,2,13,11,7,5,0,8,14};
    x= sbox_arr[x];

    // Permutation
    unsigned int pbox[8] ={0,3,5,7,2,4,6,1};

    uint8_t fx=0;
    for(int i=0;i<Len_x;i++){
        fx=fx|((x>>i)&1)<<pbox[i];
    }

    return fx;
}

uint8_t getInnerProduct(uint8_t x, uint8_t fx, uint8_t coeff[2]){
    x=x&coeff[0];
    fx=fx&coeff[1];
    uint8_t target=0;

    for(int i=0;i<Len_x;i++){
        target=target^(fx&1)^(x&1);
        fx=fx>>1;
        x=x>>1;
    }
    return target;
}

void bin_8(uint8_t n)
{
    uint8_t i;
    for (i = (uint8_t)(1 << 7); i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
}