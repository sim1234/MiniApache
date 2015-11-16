/*
Copyright (c) 2015, Szymon Zmilczak
*/


#include "MiniApacheUtils.h"

int freeRam() {
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void ClearStr(char *str, char length){
    for (int i = 0; i < length; i++){
        str[i] = 0;
    }
}

char* JoinStr(char* str1, char* str2){
	int length = strlen(str1) + strlen(str2);
	char* retVal = new char[length + 1];
	*retVal = {0};
	strcat(retVal, str1);
	strcat(retVal, str2);
	retVal[length] = 0;
	return retVal;
}

int FindChar(char* str1, char str2, int start, int end){
   for (int i = start; i < end; i++) {
        if (str1[i] == str2) {
            return i;
        }
    }
    return -1;
}

char* SubStr(char* str1, int start, int end){
    int length = end - start;
    char* retVal = new char[length + 1];
    *retVal = 0;
    for (int i = 0; i < length; i++) {
    	retVal[i] = str1[i+start];
    }
    retVal[length] = 0;
    return retVal;
}

//char buffer[7];         //the ASCII of the integer will be stored in this char array
//itoa(-31596,buffer,10); //(integer, yourBuffer, base)
char* Int2Str(int number){
	int neg = 0;
	if (number < 0){
		number = -number;
	    neg = 1;
	}
	int n = 0;
	while (pow(10, n) <= number){
		n++;
	}
	if (number == 0){
	    n = 1;
	}
	n--;
	char* buf = new char[n + neg + 2];
	int i = 0;
	if (neg == 1){
	   buf[i] = '-';
	   i++;
	}
	int l, p;
	while (n >= 0){
		p = pow(10, n);
		l = int(number / p);
	    buf[i] = l + 48; // int <0, 9> to char <'0', '9'>
		number -= p*l;
	    i++;
	    n--;
	}
	buf[i] = 0;
	return buf;
}
