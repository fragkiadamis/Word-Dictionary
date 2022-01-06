#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

#define HASH_TABLE_SIZE 1024

int findHashPosition(char *word, int word_length) {
    int sum = 0;
    int characterCount = 0;
    while (*word)
    {
        sum += pow(2, (word_length - characterCount)) * (int) *word;
        characterCount++;
        word++;
    }

    return sum % 1024;
}

int main(void) {
    char ch, *path = "/home/fragkiadamis/CLionProjects/Assignment_2/files/"; // Files directory path
    struct dirent *de; // Directory pointer
    FILE *fptr; // File pointer
    char *hash[HASH_TABLE_SIZE];

    DIR *dr = opendir(path);
    if (dr == NULL)
    {
        printf("Could not open current directory" );
        exit(EXIT_FAILURE);
    }

    while ((de = readdir(dr)) != NULL)
    {
        // Filter out current and parent directory entries
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;
        else
        {
            char *filePath = (char*)malloc((strlen(path) + strlen(de->d_name) + 1) * sizeof(char));
            strcat(strcat(filePath, path), de->d_name);
            fptr = fopen(filePath, "r");

            char *word = (char*)malloc(2 * sizeof(char));
            int bytes_allocated = 0;
            while((ch = fgetc(fptr)) != EOF)
            {
                if(ch == '\n' || ch == ' ')
                {
                    word[bytes_allocated] = '\0';
                    int pos = findHashPosition(word, strlen(word));
                    printf("word: %s, pos: %d\n", word, pos);

                    word = (char*)malloc(2 * sizeof(char));
                    bytes_allocated = 0;
                    continue;
                }
                word = (char*)realloc(word, bytes_allocated + 2);
                word[bytes_allocated++] = ch;
            }
            free(word);
            fclose(fptr);
        }
    }
    closedir(dr);
    return 0;
}
