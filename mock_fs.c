#include <stdio.h>
#include <stdint.h>
// #include <stdlib.h>

#define FILE_NAME_SIZE 128
#define MAXIMUM_FILES 256
#define MAXIMUM_CHILDREN 16
#define print_string printf
#define MAX_IN_MEMORY_FILE_CONTENT_LENGTH 128
#define MAX_INODE_CONTENT_SIZE 128

///////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////

char *strncopy(char *destination, char *source, size_t maximum) {
  int i;
  for (i = 0; source[i] != '\0' && i < maximum; i++) {
    destination[i] = source[i];
  }
  destination[i] = '\0';
  return destination;
}

uint32_t strlen(const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}

///////////////////////////////////////////////////////////////////////////
// File system
///////////////////////////////////////////////////////////////////////////

#define MAX_FILE_NAME 255
#define BLOCK_SIZE 512
#define MAX_INODES 128
#define MAX_FILESYSTEM_SIZE 65536
#define TOTAL_DIRECT_MEMORY_POINTERS 12

enum INodeType {
    INODE_FILE = 0,
    INODE_DIRECTORY = 1 << 0
} foo;

typedef uint32_t ino_t;

typedef struct dirent {
  ino_t d_ino; // inode number
  uint16_t d_reclen; // Total size of this entry (including all subfields)
  uint8_t d_namelen; // Length of string in d_name
  uint8_t d_type; // The file corresponding file type
  char d_name[MAX_FILE_NAME + 1]; // Plus one for null terminator
} DirectoryEntry;

typedef struct DIR {
  uint32_t fd; // Associated file descriptor
  uint32_t offset; // Offset within a directory index
  uint32_t length; // entry length
} OpenDirectory;

typedef struct inode {
  uint32_t type_and_permissions; // The file type and permissions
  uint32_t length; // The how many  of this
  uint64_t direct_block_pointer[TOTAL_DIRECT_MEMORY_POINTERS];
  // Note: We're skipping indirect memory pointers for now
} inode;

// Return an inode from the memory location
#define inode_lookup(memoryChunk, index) (\
    (inode *) ( \
      (memoryChunk) \
      + (sizeof(inode) * (index)) \
    ) \
  )
// Return a DirectoryEntry from a given block
#define directory_entry_lookup(memoryChunk, index) ( \
  (DirectoryEntry *)block_lookup(memoryChunk, index) \
)
// Return a raw block from the memory addresses directly after the inode list
#define block_lookup(memoryChunk, index) ( \
    (memoryChunk) \
    + (sizeof(inode) * MAX_INODES) \
    + (BLOCK_SIZE * (index)) \
  )

int inodeCount = 0;
int blockCount = 0;
char *memoryChunk = NULL;




int main(void) {
  memoryChunk = (char *) malloc(MAX_FILESYSTEM_SIZE);

  char dotName[] = ".";
  char dotDotName[] = "..";

  // Root starts at node 1
  ino_t rootINodeID = ++inodeCount;
  inode *root = inode_lookup(memoryChunk, rootINodeID);
  root->type_and_permissions = INODE_DIRECTORY;
  root->length = 0;
  for (int i = 0 ; i < TOTAL_DIRECT_MEMORY_POINTERS; i++) {
    root->direct_block_pointer[i] = NULL;
  }

  printf("Wat::: %d\n", sizeof(uint16_t));

    // Add .
    DirectoryEntry *rootDotEntry = (DirectoryEntry *) directory_entry_lookup(memoryChunk, blockCount++);
    strncopy(rootDotEntry->d_name, dotName, strlen(dotName));
    rootDotEntry->d_namelen = strlen(rootDotEntry->d_name);
    rootDotEntry->d_ino = rootINodeID;
    root->direct_block_pointer[root->length] = rootDotEntry;
    (root->length)++;

    // Add ..
    DirectoryEntry *rootDotDotEntry = (DirectoryEntry *) directory_entry_lookup(memoryChunk, blockCount++);
    strncopy(rootDotDotEntry->d_name, dotDotName, strlen(dotDotName));
    // asm("int3");
    rootDotDotEntry->d_namelen = strlen(rootDotDotEntry->d_name);
    rootDotDotEntry->d_ino = rootINodeID;
    root->direct_block_pointer[root->length] = rootDotDotEntry;
    (root->length)++;

    // Create inode for file
    ino_t helloWorldTxtINodeId = ++inodeCount;
    inode *helloWorldTxtINode = inode_lookup(memoryChunk, helloWorldTxtINodeId);
    helloWorldTxtINode->type_and_permissions = INODE_FILE;
    helloWorldTxtINode->length = 1;  // TODO: Confirm if length is blocks, or bytes
    for (int i = 0 ; i < TOTAL_DIRECT_MEMORY_POINTERS; i++) {
      helloWorldTxtINode->direct_block_pointer[i] = NULL;
    }

    // Create content for the helloworldTxtINode
    char *helloWorldTxtContent =  (char *) block_lookup(memoryChunk, blockCount++);
    char helloWorldTxtContentToCopy[] = "this is a file's content!!";
    strncopy(helloWorldTxtContent, helloWorldTxtContentToCopy, strlen(helloWorldTxtContentToCopy));
    helloWorldTxtINode->direct_block_pointer[0] = helloWorldTxtContent;

    char helloWorldTxtName[] = "helloWorld.txt";
    DirectoryEntry *helloWorldTxtEntry = (DirectoryEntry *) directory_entry_lookup(memoryChunk, blockCount++);
    strncopy(helloWorldTxtEntry->d_name, helloWorldTxtName, strlen(helloWorldTxtName));
    helloWorldTxtEntry->d_namelen = strlen(helloWorldTxtEntry->d_name);
    helloWorldTxtEntry->d_ino = helloWorldTxtINodeId;
    root->direct_block_pointer[root->length] = helloWorldTxtEntry;
    (root->length)++;



  printf("Root file type: %d\n", root->type_and_permissions);
  printf("Root length: %d\n", root->length);
  for (int i = 0, length = root->length ; i < length; i++) {
    DirectoryEntry *thisChild = root->direct_block_pointer[i];
    printf("Child %d: File name: '%s'\n", i, thisChild->d_name);
  }

  // DirectoryEntry *pls = (DirectoryEntry *) (root->base_block_pointer_1);
  // printf("Root first file name: %s\n", pls[0].d_name);

  // inode *helloWorldTxt = &((inode *) memory_chunk)[2];
  // char foo[] = "hello world";
  // // strncopy(helloWorldTxt->base_block_pointer_1, foo, strlen(foo));

  // printf("File type: %d\n", helloWorldTxt->type_and_permissions);
  // // printf("Content: %s\n", helloWorldTxt->base_block_pointer_1);

  // printf("%ld\n", sizeof(DirectoryEntry));
  printf("We done here\n");
}

// typedef struct DirectoryEntry {
//   char name[FILE_NAME_SIZE];
//   uint32_t inode;
// } DirectoryEntry;

// typedef struct FileSystemNode {
//     char name[FILE_NAME_SIZE];
//     uint32_t length;
//     uint32_t inode;
// } FileSystemNode;

// FileSystemNode *createFile(char name[], uint32_t length, uint32_t inode)
// {
//   FileSystemNode *node = malloc(sizeof(FileSystemNode));
//   strncopy(node->name, name, FILE_NAME_SIZE);
//   node->length = length;
//   node->inode = inode;
//   return node;
// }

// void printFileSystemNode(FileSystemNode *node) {
//   print_string("node(%d)->\n", node->inode);
//   print_string("  ->name=%s\n", node->name);
//   print_string("  ->inode=%d\n", node->inode);
//   print_string("  ->length=%d\n", node->length);
// }

// void printFileSystemTree(FileSystemNode *node) {

// }

// DirectoryEntry *readdir(FileSystemNode *node) {
//   if (strcmp(node->name, "dev")) {

//   }
// }
