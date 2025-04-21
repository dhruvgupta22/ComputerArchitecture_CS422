#include<stdio.h>

int main(){
	// char* ptr1;
	char* ptr;
	// ptr1 = (char*)malloc(100*sizeof(char));
	ptr = (char*)malloc(50*sizeof(char));
	memcpy(ptr, "Live simply so that others may simply live\0", 43);
	printf("%s\n", ptr);
	exit(2);
}
