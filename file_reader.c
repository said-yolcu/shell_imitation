#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    FILE *fp;

    if ((fp = fopen(argv[1], "r")) == NULL) // Open the file
    {
        printf("Cannot open the file %s\n", argv[1]);
        return 1;
    }

    printf("Press anything other than newline to quit viewing the file\n");

    int c;
    do
    {
        while ((c = getc(fp)) != EOF)
        {
            if (c == '\n')
            {
                // putc(c, stdout); // Break without printing newline
                break;
            }
            putc(c, stdout);
        }

        if (c == EOF) // If at EOF, return immediately
        {
            return 0;
        }
    } while ((c = getc(stdin)) == '\n');

    return 0;
}
