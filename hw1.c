//George Maratos (gmarat2)
#include<stdlib.h>
#include<stdio.h>
#include"functions.h"

void 
main(int argc, char **argv)
{
  //variables
  //ops
  if(argc < 2 || argc > 2)
  {
    printf("Improper number of agruements\n");
    printf("Usage: ./hw1 <url>\n");
    return;
  }
  else interface(argv[1]);
}
