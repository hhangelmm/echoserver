#include "escommon.h"

void err_sys(char * str){
	printf("%s",str);
	exit(0);
}
void err_quit(char * str){
    printf("%s",str);
    exit(0);
}
int max(int a, int b)
{ 
	return a<b?b:a; 
} 

