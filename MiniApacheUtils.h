/*
Copyright (c) 2015, Szymon Zmilczak
*/
#ifndef MiniApacheUtils_h
#define MiniApacheUtils_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

void ClearStr(char *str, char length);                   // Set all bytes in str to 0
char* JoinStr(char* str1, char* str2);                   // Join str1 and str2 [after processing, 'delete []' should be called on returned value to prevent memory leak]
int FindChar(char* str1, char str2, int start, int end); // Find index of first occurence of str2 in str1, starting on start and ending on end; return -1 if char was not found  
char* SubStr(char* str1, int start, int end);            // Return part of str1: bytes with indexes from start to end
int freeRam();                                           // Return amount of free bytes
char* Int2Str(int number);                               // Convert int to char*[]

    
#endif
