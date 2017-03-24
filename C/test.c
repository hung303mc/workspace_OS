#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct stag_str1{
    int * data;
}str1;

str1 config;
str1 * pConfig;
int a = 100;

int main(){
    printf ("a = %d \n",a);
    printf ("address of a = %d\n",&a);
    config.data = (int *)&a;
    sprintf(config.data,"%d",a);
    memcpy(config.data,)
    printf ("data = %d \n",*config.data);
    printf ("address of data = %d\n",config.data);

    pConfig = (str1 *)&config;
    printf ("pointer data = %d \n",*pConfig->data);
    printf ("address of pointer data = %d\n",pConfig->data);

    return 0;
}
