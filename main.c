#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

#define HASH_TABLE_SIZE 1024

typedef struct position_node {
    int row;
    int col;
    struct position_node *next;
} POSITION_LIST;

typedef struct file_node {
    char *name;
    struct word_node *next;
    POSITION_LIST *position;
} FILE_LIST;

typedef struct word_node {
    char *word;
    struct word_node *left;
    struct word_node *right;
    FILE_LIST *file;
} WORD_TREE;

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

// add the position of the word in the file that is coming from
void addWordsFilePosition(POSITION_LIST **filePosition, int row, int col) {
    // Add word's position in respective file
    *filePosition = (POSITION_LIST*)malloc(sizeof(POSITION_LIST));
    (**filePosition).row = row;
    (**filePosition).col = col;
}

// Add the file in the dictionary that the word is coming from
void addWordsFile(FILE_LIST **fileNode, char *fileName, int row, int col){
    // Add word's respective filename
    *fileNode = (FILE_LIST*)malloc(sizeof(FILE_LIST));
    (*fileNode)->name = (char*)malloc(strlen(fileName) * sizeof(char));
    strcpy((*fileNode)->name, fileName);

    // Add the word's position in the file from which it is coming from
    addWordsFilePosition(&((*fileNode)->position), row, col);
}

// Add the word in the dictionary
void addWord(WORD_TREE **wrdNode, char *word, int wordLength, char *fileName, int row, int col) {
    // Add word
    *wrdNode = (WORD_TREE*)malloc(sizeof(WORD_TREE));
    (*wrdNode)->word = (char*)malloc(wordLength * sizeof(char));
    strcpy((*wrdNode)->word, word);

    // Add the file from which the word is coming from
    addWordsFile(&(*wrdNode)->file, fileName, row, col);
}

void addInDictionary(WORD_TREE **wrdNode, char *word, int wordLength, char *fileName, int row, int col) {
    // Word does not exist in dictionary, add the word in the dictionary
    if (*wrdNode == NULL)
        addWord(wrdNode, word, wordLength, fileName, row, col);
    else {
        int wordCmp = strcmp((*wrdNode)->word, word);
        if (wordCmp > 0) {

        } else if (wordCmp < 0) {

        } else {

        }
    }
}

int main(void) {
    char ch, *path = "/home/fragkiadamis/CLionProjects/Assignment_2/files/";
    struct dirent *de;
    FILE *fptr;
    WORD_TREE *hashTable[HASH_TABLE_SIZE] = {NULL};

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

            // Declare variables to store row and column of the word in the file
            int row = 1, col = 0, colIndex = 0;
            int newWord = 1;

            // Create a pointer to store each word
            char *word = (char*)malloc(2 * sizeof(char));
            int bytes_allocated = 0;

            // While it's not the End Of File...
            // Isolate each word. Words are distinguished by either a space or a newline
            while((ch = fgetc(fptr)) != EOF) {
                if(ch == '\n' || ch == ' ') { // The word is over
                    word[bytes_allocated] = '\0'; // Add null terminator
                    int wordLength = strlen(word); // Calculate word's length
                    int pos = hashingFunction(word, wordLength); // Find the word's position in the hash table.
                    addInDictionary(&hashTable[pos], word, wordLength, de->d_name, row, col);
                    printf("hash: %d, word: %s, fileName: %s, row: %d col: %d\n", pos, hashTable[pos]->word, hashTable[pos]->file->name, hashTable[pos]->file->position->row, hashTable[pos]->file->position->col);

                    // Default the word and continue to next one.
                    free(word);
                    word = (char*)malloc(2 * sizeof(char));
                    bytes_allocated = 0;

                    newWord = 1;
                    if (ch == '\n') {
                        row++; // Increase row
                        col = colIndex = 0; // Default column
                    } else
                        colIndex++;

                    continue;
                }

                // Append character to word
                word = (char*)realloc(word, bytes_allocated + 2);
                word[bytes_allocated++] = ch;
                colIndex++;

                if (newWord) {
                    newWord = 0;
                    col = colIndex;
                }
            }
            free(word);
            fclose(fptr);
            break;
        }
    }
    closedir(dr);
    return 0;
}
