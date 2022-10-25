#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    FILE *fpr; // File pointer for reading
    FILE *fpw; // File pointer for writing

    if ((fpr = fopen(argv[1], "r")) == NULL) // Open the file for reading
    {
        printf("Cannot open the file %s\n", argv[1]);
        return 1;
    }

    if ((fpw = fopen(argv[2], "w")) == NULL) // Open the file for writing
    {
        printf("Cannot open the file %s\n", argv[1]);
        return 1;
    }

    int c;
    
    while ((c = getc(fpr)) != EOF)
    {
        putc(c, fpw);
    }

    return 0;
}
