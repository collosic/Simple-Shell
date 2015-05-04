#include <stdio.h>

#define MAXLINE 8192
int main(void) 
{
    char str[MAXLINE];
  
    scanf("%[^\n]%*c",str);
    printf("This is from stdin: %s\n",str);
    
    return 0;
}

