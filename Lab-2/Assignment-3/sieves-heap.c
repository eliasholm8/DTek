#include <stdio.h>
#include <stdlib.h>

struct bool
{
    unsigned char boolean : 1;  // Creates a one bit bitfield in a unsigned char, Compiler can theoreticly use less memory
};

void print_number(int n){
  static int column_count = 0;

  if (column_count == 6)
  {
    printf("\n");
    column_count = 0;
  }
  printf("%10d ", n);
  column_count++;
}


void print_seives(int n) {
    // Dynamicly allocates memory using malloc
    // Memory needed is calculated: number of elements * size of each element (1 byte, smallest addressable)
    struct bool* numbers = malloc(n * sizeof(struct bool));     

    if (numbers == NULL)    // Malloc returns null if unable to allocate memory
    {
        printf("Failed");
    } 

    for (int i = 0; i < n-1; i++)
    {
        numbers[i].boolean = 1;   // Populates the array with bool
    }

    for (int i = 2; i*i <= n; i++) {   // Check values < sqrt(n)
        if (numbers[i - 2].boolean) {
            for (int j = i*i; j <= n; j+=i) {
                    numbers[j - 2].boolean = 0; 
                }                
            }
        }
     
    for (int i = 2; i < n; i++) {
        if (numbers[i - 2].boolean) {
            print_number(i);
        }
        
    }
    if (numbers[n - 2].boolean) { // Check if n itself is prime
        print_number(n); // Print n if it's a prime number
    }
    printf("\n");
    free(numbers);  // deallocate memory
}

int main(int argc, char *argv[]) {
  if(argc == 2) {
    print_seives(atoi(argv[1]));
  }
  else {
    printf("Please state an interger number.\n");
  }
  return 0;
}
