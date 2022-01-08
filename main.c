#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

#define HASH_TABLE_SIZE 1024 // The size of the hash table
#define DEBUG // Uncomment for debug printing

/*
 * The position list node. The position of a word
 * in a file (row and column) are stored in a list.
 * The list head is stored in the file node of the respective file
*/
typedef struct position_node {
    unsigned short row;
    unsigned short col;
    struct position_node *next;
} POSITION_LIST;

/*
 * The file list node. The filenames that every
 * word exists are stored in a list. The list's head
 * is stored in the tree node of the respective word
*/
typedef struct file_node {
    char *name;
    struct file_node *next;
    POSITION_LIST *position;
} FILE_LIST;

/*
 * The tree node. The words with the same hash
 * are stored in the tree in the
 * respective position in the hash table
*/
typedef struct word_node {
    char *word;
    struct word_node *left;
    struct word_node *right;
    FILE_LIST *file;
} WORD_TREE;

/*
 * A struct that temporarily stores all
 * the properties of a word that are needed to
 * construct the structures in the hash table.
 * This struct is also used for the search
 * process of a word in the structures
*/
typedef struct word_props {
    char *word;
    char *filename;
    unsigned short row, col;
} WORD_PROPS;

/*
 * if DEBUG is defined, print the structure
*/
#ifdef DEBUG
/* Displays the list nodes that store the position in a file of a word */
void displayPositionList(POSITION_LIST *head) {
    if(head != NULL) {
        printf("row: %d, col: %d | ", head->row, head->col);
        displayPositionList(head->next);
    }
}

/* Displays the list nodes that store the filenames that a word exists */
void displayFileList(FILE_LIST *head) {
    if(head != NULL) {
        printf("filename: %s, ", head->name);
        displayPositionList(head->position);
        displayFileList(head->next);
    }
}

/* Displays the tree nodes that store a word, and it's respective lists */
void displayTree(WORD_TREE *root) {
        if (root != NULL) {
            displayTree(root->left);
            printf("word: %s | ", root->word);
            displayFileList(root->file);
            putchar('\n');
            displayTree(root->right);
        }
    }
#endif

/* Hashing function, calculates the position of a word in the hash table; */
unsigned short hashingFunction(char *word, unsigned short word_length) {
    unsigned short sum = 0;
    unsigned short characterCount = 0;
    while (*word) {
        sum += pow(2, (word_length - characterCount)) * (int) *word;
        characterCount++;
        word++;
    }

    return sum % HASH_TABLE_SIZE;
}

/* Create new position list node */
void newPositionListNode(POSITION_LIST **node, WORD_PROPS *wordProps) {
    *node = (POSITION_LIST*)malloc(sizeof(POSITION_LIST));
    (*node)->row = (*wordProps).row;
    (*node)->col = (*wordProps).col;
    (*node)->next = NULL;
}

/* Traverse the position list and insert a node */
void traversePositionList(POSITION_LIST **listNode, WORD_PROPS *wordProps) {
    if(*listNode == NULL)
        newPositionListNode(listNode, wordProps);
    else
        traversePositionList(&(*listNode)->next, wordProps);
}

/* Create new file list node */
void newFileListNode(FILE_LIST **node, WORD_PROPS *wordProps) {
    *node = (FILE_LIST*)malloc(sizeof(FILE_LIST));
    (*node)->name = (char*)malloc(strlen((*wordProps).filename) * sizeof(char) + 1);
    strcpy((*node)->name, (*wordProps).filename);
    (*node)->next = NULL;
    (*node)->position = NULL;

    /* Create the position list of the file list */
    traversePositionList(&(*node)->position, wordProps);
}

/* Traverse the file list and insert a node */
void traverseFileList(FILE_LIST **listNode, WORD_PROPS *wordProps) {
    if(*listNode == NULL)
        newFileListNode(listNode, wordProps);
    else
        traverseFileList(&(*listNode)->next, wordProps);
}

/* Create a tree node */
void newTreeNode(WORD_TREE **node, WORD_PROPS *wordProps){
    *node = (WORD_TREE*)malloc(sizeof(WORD_TREE));
    (*node)->word = (char*)malloc(strlen((*wordProps).word) * sizeof(char) + 1);
    strcpy((*node)->word, (*wordProps).word);
    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->file = NULL;

    /* Create the file list of the tree node */
    traverseFileList(&(*node)->file, wordProps);
}

/* Traverse the tree to find the leaves */
void traverseTree(WORD_TREE **treeNode, WORD_PROPS *wordProps) {
    if(*treeNode == NULL) // Root of the tree is null
        newTreeNode(treeNode, wordProps);

    // Traverse the tree
    else if(strcmp((*treeNode)->word, (*wordProps).word) < 0) // The word alphabetically is first
        traverseTree(&(*treeNode)->left, wordProps);
    else if(strcmp((*treeNode)->word, (*wordProps).word) > 0) // The word alphabetically is second
        traverseTree(&(*treeNode)->right, wordProps);
    else // The word already exists in the tree, add the new list nodes (either file node and position node or just position node)
        traverseFileList(&(*treeNode)->file, wordProps);
}

/* Search tree and return node */
WORD_TREE *searchTree(WORD_TREE *root, char *searchString) {
    if(root == NULL)
        return NULL;

    if(strcmp(root->word, searchString) < 0)
        return searchTree(root->left, searchString);
    else if(strcmp(root->word, searchString) > 0)
        return searchTree(root->right, searchString);
    else if(strcmp(root->word, searchString) == 0)
        return root;
}

/* Display the filenames of the word and the positions inside them */
void displayWordData(WORD_TREE *treeNode) {
    while(treeNode->file != NULL) {
        // For each position of the word in file display the filename alongside the position.
        while(treeNode->file->position != NULL) {
            printf("%s (%d, %d)\n", treeNode->file->name, treeNode->file->position->row, treeNode->file->position->col);
            treeNode->file->position = treeNode->file->position->next;
        }
        treeNode->file = treeNode->file->next;
    }
}

int main(void) {
    char ch, *path = "/home/fragkiadamis/CLionProjects/Assignment_2/files/";
    unsigned short bytes_allocated = 0, pos = 0;
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

    puts("Creating the dictionary...");
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

            unsigned short newWord = 1;
            unsigned short colIndex = 0;

            WORD_PROPS wordProps;
            wordProps.word = (char*)malloc(2 * sizeof(char));
            wordProps.filename = (char*)malloc(strlen(de->d_name) * sizeof(char) + 1);
            strcpy(wordProps.filename, de->d_name);
            wordProps.row = 1;
            wordProps.col = 0;

            // While it's not the End Of File...
            // Isolate each word. Words are distinguished by either a space or a newline
            while((ch = fgetc(fptr)) != EOF) {
                if(ch == '\n' || ch == ' ') { // The word is over
                    wordProps.word[bytes_allocated] = '\0'; // Add null terminator
                    pos = hashingFunction(wordProps.word, strlen(wordProps.word)); // Find the word's position in the hash table.
                    traverseTree(&hashTable[pos], &wordProps);

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
                wordProps.word[bytes_allocated] = ch;
                bytes_allocated++;
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

#ifdef DEBUG
    for(unsigned short i = 0; i < HASH_TABLE_SIZE; i++) {
        if(hashTable[i] != NULL) {
            printf("Hash: %d\n", i);
            displayTree(hashTable[i]);
            puts("----------------------");
        }
    }
#endif

    char *searchString = (char*)malloc(2 * sizeof(char));
    bytes_allocated = 0;
    puts("Enter a word for search");
    while((ch = getchar()) != '\n') {
        searchString = (char*)realloc(searchString, bytes_allocated + 2);
        searchString[bytes_allocated] = ch;
        bytes_allocated++;
    }
    searchString[bytes_allocated] = '\0';
    pos = hashingFunction(searchString, strlen(searchString));

    if(hashTable[pos] == NULL)
        puts("The word does not exist in the dictionary");
    else {
        WORD_TREE *treeNode = searchTree(hashTable[pos], searchString);
        if(treeNode)
            displayWordData(treeNode);
        else
            puts("The word does not exist in the dictionary");
    }
    free(searchString);

    return 0;
}
