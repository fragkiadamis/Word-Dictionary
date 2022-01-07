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
    struct file_node *next;
    POSITION_LIST *position;
} FILE_LIST;

typedef struct word_node {
    char *word;
    struct word_node *left;
    struct word_node *right;
    FILE_LIST *file;
} WORD_TREE;

typedef struct word_props {
    char *word;
    char *filename;
    int row, col;
} WORD_PROPS;

void displayPositionList(POSITION_LIST *head) {
    if(head != NULL) {
        printf("row: %d, col: %d | ", head->row, head->col);
        displayPositionList(head->next);
    }
}

void displayFileList(FILE_LIST *head) {
    if(head != NULL) {
        printf("filename: %s, ", head->name);
        displayPositionList(head->position);
        displayFileList(head->next);
    }
}

void displayTree(WORD_TREE *root)
{
    if (root != NULL) {
        displayTree(root->left);
        printf("word: %s | ", root->word);
        displayFileList(root->file);
        putchar('\n');
        displayTree(root->right);
    }
}

// Hashing function, calculates the position of a word in the hash table;
int hashingFunction(char *word, int word_length) {
    int sum = 0;
    int characterCount = 0;
    while (*word) {
        sum += pow(2, (word_length - characterCount)) * (int) *word;
        characterCount++;
        word++;
    }

    return sum % HASH_TABLE_SIZE;
}

POSITION_LIST *newPositionListNode(WORD_PROPS wordProps) {
    POSITION_LIST *node = (POSITION_LIST*)malloc(sizeof(POSITION_LIST));
    node->row = wordProps.row;
    node->col = wordProps.col;
    node->next = NULL;
    printf("%s %s %d %d\n", wordProps.word, wordProps.filename, node->row, node->col);
    return node;
}

POSITION_LIST *insertPositionListNode(POSITION_LIST *head, WORD_PROPS wordProps) {
    if(head == NULL)
        return newPositionListNode(wordProps);
    insertPositionListNode(head->next, wordProps);
    return head;
}

FILE_LIST *newFileListNode(WORD_PROPS wordProps) {
    FILE_LIST *node = (FILE_LIST*)malloc(sizeof(FILE_LIST));
    node->name = (char*)malloc(strlen(wordProps.filename) * sizeof(char) + 1);
    strcpy(node->name, wordProps.filename);
    node->next = NULL;
    node->position = insertPositionListNode(node->position = NULL, wordProps);
    return node;
}

FILE_LIST *insertFileListNode(FILE_LIST *head, WORD_PROPS wordProps) {
    if(head == NULL)
        return newFileListNode(wordProps);
    insertFileListNode(head->next, wordProps);
    return head;
}

WORD_TREE *newTreeNode(WORD_PROPS wordProps){
    WORD_TREE *node = (WORD_TREE*)malloc(sizeof(WORD_TREE));
    node->word = (char*)malloc(strlen(wordProps.word) * sizeof(char) + 1);
    strcpy(node->word, wordProps.word);
    node->left = NULL;
    node->right = NULL;
    node->file = insertFileListNode(node->file = NULL, wordProps);
    return node;
}

WORD_TREE *insertTreeNode(WORD_TREE *root, WORD_PROPS wordProps) {
    if(root == NULL)
        return newTreeNode(wordProps);

    if(strcmp(root->word, wordProps.word) < 0)
        root->left = insertTreeNode(root->left, wordProps);
    else if(strcmp(root->word, wordProps.word) > 0)
        root->right = insertTreeNode(root->right, wordProps);
    else {
        root->file = insertFileListNode(root->file, wordProps);
        printf("%s %s %d %d\n", root->word, root->file->name, root->file->position->row, root->file->position->col);
    }

    return root;
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

            int newWord = 1;
            WORD_PROPS wordProps;
            wordProps.filename = de->d_name;
            wordProps.word = (char*)malloc(2 * sizeof(char));
            wordProps.filename = (char*)malloc(strlen(de->d_name) * sizeof(char) + 1);
            strcpy(wordProps.filename, de->d_name);
            wordProps.row = 1;
            wordProps.col = 0;
            int bytes_allocated = 0;
            int colIndex = 0;

            // While it's not the End Of File...
            // Isolate each word. Words are distinguished by either a space or a newline
            while((ch = fgetc(fptr)) != EOF) {
                if(ch == '\n' || ch == ' ') { // The word is over
                    wordProps.word[bytes_allocated] = '\0'; // Add null terminator
                    int pos = hashingFunction(wordProps.word, strlen(wordProps.word)); // Find the word's position in the hash table.
                    hashTable[pos] = insertTreeNode(hashTable[pos], wordProps);

                    // Default the word and continue to next one.
                    free(wordProps.word);
                    wordProps.word = (char*)malloc(2 * sizeof(char));
                    bytes_allocated = 0;

                    newWord = 1;
                    if (ch == '\n') {
                        wordProps.row++; // Increase row
                        wordProps.col = colIndex = 0; // Default column
                    } else
                        colIndex++;

                    continue;
                }

                // Append character to word
                wordProps.word = (char*)realloc(wordProps.word, bytes_allocated + 2);
                wordProps.word[bytes_allocated++] = ch;
                colIndex++;

                if (newWord) {
                    newWord = 0;
                    wordProps.col = colIndex;
                }
            }
            free(wordProps.word);
            fclose(fptr);
        }
    }
    closedir(dr);

//    for(int i = 0; i < HASH_TABLE_SIZE; i++) {
//        if(hashTable[i] != NULL) {
//            displayTree(hashTable[i]);
//            puts("----------------------");
//        }
//    }

    return 0;
}
