


#include <stdio.h>

char* text1 = "This is a string.";
char* text2 = "Yet another thing.";

int list1[20];  // int = 4 bytes, 80 Bytes = 20 instances of int
int list2[20];

int count = 0; 

void work(){
  copycodes(text1, list1, &count);  // Int needs to be passed as pointer to be modifiable
  copycodes(text2, list2, &count);  // text is defined as a pointer, the fist element of an array is a pointer to the first element
}

void copycodes(char* input, int* output, int* counter){
  while (*input != '\0')  // Run until null
  {
    *output = (int)*input;  // Cast int to char
    input++;  // Increments address by size of type (char = 1 byte)
    output++; // Do same for output
    (*counter)++; // Increments the value that the pointer points to
  }
  
}


void printlist(const int* lst){
  printf("ASCII codes and corresponding characters.\n");
  while(*lst != 0){
    printf("0x%03X '%c' ", *lst, (char)*lst);
    lst++;
  }
  printf("\n");
}

void endian_proof(const char* c){
  printf("\nEndian experiment: 0x%02x,0x%02x,0x%02x,0x%02x\n", 
         (int)*c,(int)*(c+1), (int)*(c+2), (int)*(c+3));
  
}

int main(void){
  work();

  printf("\nlist1: ");
  printlist(list1);
  printf("\nlist2: ");
  printlist(list2);
  printf("\nCount = %d\n", count);

  endian_proof((char*) &count);
}
