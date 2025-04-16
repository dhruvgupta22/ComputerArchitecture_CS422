#include <stdio.h>
// code to include lwl, lwr, swr, swl
int main(){
    int n;
    char* arr;
    int i;
    int* p, *q, *r;
    scanf("%d", &n);
    arr = (char*)malloc(n*sizeof(char)); 
    for(i=0; i<n; i++){
        scanf("%c", &arr[i]);
    }
    for(i=0; i<n; i++){
        printf("%c", arr[i]);
    }
    p = (int*)(arr+1);
    q = (int*)(arr+2);
    r = (int*)(arr+3);
    printf("%d %d %d\n", *p, *q, *r);
    return 0;
}
