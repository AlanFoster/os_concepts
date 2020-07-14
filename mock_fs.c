#include <stdio.h>
#include <stdint.h>
// #include <stdlib.h>

#undef NULL
#define NULL 0
#define FILE_NAME_SIZE 128
#define MAXIMUM_FILES 256
#define MAXIMUM_CHILDREN 16
#define print_string printf
#define MAX_IN_MEMORY_FILE_CONTENT_LENGTH 128
#define MAX_INODE_CONTENT_SIZE 128

///////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////

char *strncopy(char *destination, char *source, uint32_t maximum) {
  uint32_t i;
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

int strcmp(char *str1, char *str2) {
    int i = 0;
    for ( i = 0 ; str1[i] == str2[i]; i++) {
        if (str1[i] == '\0') {
            return 0;
        }
    }

    return str1[i] - str2[i];
}

void crashAndBurn(char *message) {
  print_string("%s", message);
  while (1);
}

///////////////////////////////////////////////////////////////////////////
// Ext2 inspired file system, which works entirely in memory
///////////////////////////////////////////////////////////////////////////

#define MAX_FILE_NAME 255
#define BLOCK_SIZE 512
#define MAX_INODES 128
#define MAX_FILESYSTEM_SIZE 65536
#define TOTAL_DIRECT_MEMORY_POINTERS 12
#define ROOT_INODE 2

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
  union {
    DirectoryEntry* direct_block_pointer_directories[TOTAL_DIRECT_MEMORY_POINTERS];
    char* direct_block_pointer_bytes[TOTAL_DIRECT_MEMORY_POINTERS];
  } ;
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

int inodeCount = 2;
int blockCount = 0;
char *memoryChunk = NULL;

ino_t getFreeINodeID() {
  // Simple bump for now.
  return ++inodeCount;
}

void createDirectoryEntry(ino_t parent, char *name, ino_t child) {
  inode *parentNode = inode_lookup(memoryChunk, parent);
  // if (isDirectory(parentNode)) {
    // Detect valid
  // }

  // Create the new directory entry
  DirectoryEntry *entry = (DirectoryEntry *) directory_entry_lookup(memoryChunk, blockCount++);

  uint32_t len = strlen(name);
  strncopy(entry->d_name, name, len);
  entry->d_namelen = len;
  entry->d_ino = child;

  // Store a reference to the newly created directory entry within the parent inode
  parentNode->direct_block_pointer_directories[parentNode->length] = entry;
  (parentNode->length)++;
}

ino_t createRootDirectory() {
  ino_t ino = ROOT_INODE;
  inode *node = inode_lookup(memoryChunk, ino);
  node->type_and_permissions = INODE_DIRECTORY;
  node->length = 0;
  for (int i = 0 ; i < TOTAL_DIRECT_MEMORY_POINTERS; i++) {
    node->direct_block_pointer_directories[i] = NULL;
  }
  return ino;
}

ino_t createINodeDirectory() {
  ino_t ino = getFreeINodeID();
  inode *node = inode_lookup(memoryChunk, ino);
  node->type_and_permissions = INODE_DIRECTORY;
  node->length = 0;
  for (int i = 0 ; i < TOTAL_DIRECT_MEMORY_POINTERS; i++) {
    node->direct_block_pointer_directories[i] = NULL;
  }
  return ino;
}

ino_t createINodeFile() {
  ino_t ino = getFreeINodeID();
  inode *node = inode_lookup(memoryChunk, ino);
  node->type_and_permissions = INODE_FILE;
  node->length = 1;  // TODO: Confirm if length is blocks, or bytes
  for (int i = 0 ; i < TOTAL_DIRECT_MEMORY_POINTERS; i++) {
    node->direct_block_pointer_bytes[i] = NULL;
  }

  return ino;
}

// Deletes all content and writes it out again for now
void writeContent(ino_t ino, char *buffer, int size) {
  inode *node = inode_lookup(memoryChunk, ino);
  for (int i = 0 ; i < TOTAL_DIRECT_MEMORY_POINTERS; i++) {
    node->direct_block_pointer_bytes[i] = NULL;
  }

  if (size > 512) {
    crashAndBurn("Current file system only supports at most 512b files");
  }

  char *block =  (char *) block_lookup(memoryChunk, blockCount++);
  strncopy(block, buffer, size);
  node->length = size;
  node->direct_block_pointer_bytes[0] = block;
}

// TODO: Confirm what happens if you try to read more data than what's available
void readContent(ino_t ino, char *buffer, uint32_t size) {
  inode *node = inode_lookup(memoryChunk, ino);

  int readAmount;
  if (node->length < size) {
    readAmount = node->length;
  } else {
    readAmount = size;
  }

  // TODO: find the right block correctly etc.
  strncopy(buffer, node->direct_block_pointer_bytes[0], readAmount);
}

ino_t mkfolder(char *name, ino_t parent) {
  ino_t ino = createINodeDirectory();
  createDirectoryEntry(ino, ".", ino);
  createDirectoryEntry(ino, "..", parent);
  createDirectoryEntry(parent, name, ino);
  return ino;
}

ino_t initRamdisk() {
  memoryChunk = (char *) malloc(MAX_FILESYSTEM_SIZE);

  ino_t root = createRootDirectory();
  createDirectoryEntry(root, ".", root);
  createDirectoryEntry(root, "..", root);

  // Create inode for file
  ino_t helloWorldTxt = createINodeFile();
  char content[] = "Content from hello world file!";
  writeContent(helloWorldTxt, content, strlen(content));

  // Create content for the helloworldTxtINode
  createDirectoryEntry(root, "helloWorld.txt", helloWorldTxt);

  ino_t subFolder = mkfolder("subFolder", root);
  // Create 5 arbitrary files within the subfolder with the same name and value
  for (int i = 0 ; i < 9; i++) {
    ino_t node = createINodeFile();
    char fileName[] = "0.txt";
    char fileContent[] = "Content from file number 0";
    fileName[0] = '0' + i;
    fileContent[strlen(content) - 1] = '0' + i;

    writeContent(node, fileContent, strlen(content));
    createDirectoryEntry(subFolder, fileName, node);
  }

  return root;
}

///////////////////////////////////////////////////////////////////////////
// Virtual file system
///////////////////////////////////////////////////////////////////////////

// TODO:
// typedef struct vfs_node {
// } VFSNode;

///////////////////////////////////////////////////////////////////////////
// Simple programs for viewing the file system
///////////////////////////////////////////////////////////////////////////

ino_t find(ino_t root, char *name) {
  inode *node = inode_lookup(memoryChunk, root);

  if (strcmp(name, "/") == 0) {
      return root;
  }

  for (int i = 0, length = node->length ; i < length; i++) {
    DirectoryEntry *entry = node->direct_block_pointer_directories[i];
    print_string("%s\n", entry->d_name);
    if (strcmp(entry->d_name, name) == 0) {
      return entry->d_ino;
    }
  }

  return NULL;
}

void ls(ino_t root, char *name) {
  ino_t ino = find(root, name);
  inode *node = inode_lookup(memoryChunk, ino);
  printf("type: %d\n", node->type_and_permissions);
  printf("length: %d\n", node->length);
  for (int i = 0, length = node->length ; i < length; i++) {
    DirectoryEntry *entry = node->direct_block_pointer_directories[i];
    printf("Child %d: File name: '%s'\n", i, entry->d_name);
  }
}

void cat(ino_t root, char *name) {
  ino_t ino = find(root, name);
  inode *node = inode_lookup(memoryChunk, ino);
  char content[BUFSIZ];
  readContent(ino, content, node->length);
  printf("The file content is: %s\n", content);
}

int main(void) {
  ino_t root = initRamdisk();

  print_string("> ls /\n");
  ls(root, "/");
  print_string("> cat helloWorld.txt\n");
  cat(root, "helloWorld.txt");
  print_string("> ls subFolder\n");
  ls(root, "subFolder");
}
