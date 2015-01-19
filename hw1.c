//George Maratos (gmarat2)
#include<stdlib.h>
#include<stdio.h>

/* refrences
http://www.amiga.org/forums/archive/index.php/t-20341.html
http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#connect
*/
void 
main(int argc, char **argv)
{
  //variables
  //ops
  if(argc != 2)
  {
    printf("Improper number of agruments\n");
    printf("Usage: ./hw1 <url>\n");
    return;
  }
  else interface(argv[1]);
}
