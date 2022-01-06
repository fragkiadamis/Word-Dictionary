#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

#define HASH_TABLE_SIZE 1024

// Hashing function, calculates the position of a word in the hash table;
int hashingFunction(char *word, int word_length) {
    int sum = 0;
    int characterCount = 0;
    while (*word)
    {
        sum += pow(2, (word_length - characterCount)) * (int) *word;
        characterCount++;
        word++;
    }

    return sum % HASH_TABLE_SIZE;
}

int main(void) {
    char ch, *path = "/home/fragkiadamis/CLionProjects/Assignment_2/files/";
    struct dirent *de;
    FILE *fptr;
    char *hash[HASH_TABLE_SIZE];

    // Open directory, terminate if the process fails.
    DIR *dr = opendir(path);
    if (dr == NULL)
    {
        printf("Could not open current directory" );
        exit(EXIT_FAILURE);
    }

    // While there are directory entries...
    while ((de = readdir(dr)) != NULL)
    {
        // Filter out current and parent directory entries
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;
        else
        {
            // Open file entry.
            char *filePath = (char*)malloc((strlen(path) + strlen(de->d_name) + 1) * sizeof(char));
            strcat(strcat(filePath, path), de->d_name);
            fptr = fopen(filePath, "r");

            // Create a pointer to store each word
            char *word = (char*)malloc(2 * sizeof(char));
            int bytes_allocated = 0;

            // While it's not the End Of File...
            // Isolate each word. Words are distinguished by either a space or a newline
            while((ch = fgetc(fptr)) != EOF)
            {
                if(ch == '\n' || ch == ' ') // The word is over
                {
                    word[bytes_allocated] = '\0'; // Add null terminator
                    int pos = hashingFunction(word, strlen(word)); // Find the word's position in the hash table.
                    printf("word: %s, pos: %d\n", word, pos);

                    // Default the word and continue to next one.
                    word = (char*)malloc(2 * sizeof(char));
                    bytes_allocated = 0;
                    continue;
                }
                // Append character to word
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
