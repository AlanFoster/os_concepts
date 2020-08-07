#include <stdio.h>
#include <stdint.h>
// #include <stdlib.h>
#include <stdbool.h>

#undef NULL
#define NULL 0
#define FILE_NAME_SIZE 128
#define MAXIMUM_FILES 256
#define MAXIMUM_CHILDREN 16
#define print_string printf
#define MAX_IN_MEMORY_FILE_CONTENT_LENGTH 128
#define MAX_INODE_CONTENT_SIZE 128

#define rw_offset uint32_t

///////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////

char *strncopy(char *destination, char *source, size_t maximum) {
  size_t i;
  for (i = 0; source[i] != '\0' && i < maximum; i++) {
    destination[i] = source[i];
  }
  destination[i] = '\0';
  return destination;
}

// Placeholder for copying data from kernel space into user space
unsigned long copy_to_user(void *destination, const void * source, size_t num ) {
  memcpy(destination, source, num);
}

// Placeholder for copying data from userspace into kernel space
unsigned long copy_from_user(void *destination, const void * source, size_t num ) {
  memcpy(destination, source, num);
}

void *memcpy(void * destination, const void * source, size_t num) {
  unsigned char *dst = (unsigned char *) destination;
  unsigned char *src = (unsigned char *) source;
  for (size_t i = 0 ; i < num ; i ++) {
    dst[i] = src[i];
  }
}

size_t strlen(const char *str) {
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

struct directory_entry {
  ino_t d_ino; // inode number
  uint16_t d_reclen; // Total size of this entry (including all subfields)
  uint8_t d_namelen; // Length of string in d_name
  uint8_t d_type; // The file corresponding file type
  char d_name[MAX_FILE_NAME + 1]; // Plus one for null terminator
};

struct open_directory {
  uint32_t fd; // Associated file descriptor
  uint32_t offset; // Offset within a directory index
  uint32_t length; // entry length
};

struct inode {
  uint32_t type_and_permissions; // The file type and permissions
  uint32_t length; // The how many  of this
  union {
    struct directory_entry* direct_block_pointer_directories[TOTAL_DIRECT_MEMORY_POINTERS];
    char* direct_block_pointer_bytes[TOTAL_DIRECT_MEMORY_POINTERS];
  } ;
  // Note: We're skipping indirect memory pointers for now
};

// Return an inode from the memory location
static inline struct inode *inode_lookup(size_t *memoryChunk, uint32_t index) {
  return (struct inode *) (
    (memoryChunk)
    + (sizeof(struct inode) * (index))
  );
}
// Return a directory_entry from a given block
#define directory_entry_lookup(memoryChunk, index) ( \
  (struct directory_entry *) block_lookup(memoryChunk, index) \
)
// Return a raw block from the memory addresses directly after the inode list
#define block_lookup(memoryChunk, index) ( \
    (memoryChunk) \
    + (sizeof(struct inode) * MAX_INODES) \
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
  struct inode *parentNode = inode_lookup(memoryChunk, parent);
  // if (isDirectory(parentNode)) {
    // Detect valid
  // }

  // Create the new directory entry
  struct directory_entry *entry = (struct directory_entry *) directory_entry_lookup(memoryChunk, blockCount++);

  strncopy(entry->d_name, name, MAX_FILE_NAME);
  entry->d_namelen = entry->d_namelen;
  entry->d_ino = child;

  // Store a reference to the newly created directory entry within the parent inode
  parentNode->direct_block_pointer_directories[parentNode->length] = entry;
  (parentNode->length)++;
}

ino_t createRootDirectory() {
  ino_t ino = ROOT_INODE;
  struct inode *node = inode_lookup(memoryChunk, ino);
  node->type_and_permissions = INODE_DIRECTORY;
  node->length = 0;
  for (int i = 0 ; i < TOTAL_DIRECT_MEMORY_POINTERS; i++) {
    node->direct_block_pointer_directories[i] = NULL;
  }
  return ino;
}

ino_t createINodeDirectory() {
  ino_t ino = getFreeINodeID();
  struct inode *node = inode_lookup(memoryChunk, ino);
  node->type_and_permissions = INODE_DIRECTORY;
  node->length = 0;
  for (int i = 0 ; i < TOTAL_DIRECT_MEMORY_POINTERS; i++) {
    node->direct_block_pointer_directories[i] = NULL;
  }
  return ino;
}

ino_t createINodeFile() {
  ino_t ino = getFreeINodeID();
  struct inode *node = inode_lookup(memoryChunk, ino);
  node->type_and_permissions = INODE_FILE;
  node->length = 1;  // TODO: Confirm if length is blocks, or bytes
  for (int i = 0 ; i < TOTAL_DIRECT_MEMORY_POINTERS; i++) {
    node->direct_block_pointer_bytes[i] = NULL;
  }

  return ino;
}

// Deletes all content and writes it out again for now
void writeContent(ino_t ino, char *buffer, int size) {
  struct inode *node = inode_lookup(memoryChunk, ino);
  for (int i = 0 ; i < TOTAL_DIRECT_MEMORY_POINTERS; i++) {
    node->direct_block_pointer_bytes[i] = NULL;
  }

  if (size > 512) {
    crashAndBurn("Current file system only supports at most 512b files");
  }

  char *block =  (char *) block_lookup(memoryChunk, blockCount++);
  copy_from_user(block, buffer, BLOCK_SIZE);
  node->length = size;
  node->direct_block_pointer_bytes[0] = block;
}

ssize_t readContent(ino_t ino, char *buffer, uint32_t size, rw_offset offset) {
  struct inode *node = inode_lookup(memoryChunk, ino);

  int readAmount;
  if (node->length < size) {
    readAmount = node->length;
  } else {
    readAmount = size;
  }

  // TODO: find the right block correctly etc.
  copy_to_user(buffer, ((node->direct_block_pointer_bytes[0]) + offset), readAmount);
  return readAmount;
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

// In Memory operations

ino_t find(ino_t root, char *name) {
  struct inode *node = inode_lookup(memoryChunk, root);

  if (strcmp(name, "/") == 0) {
      return root;
  }

  for (int i = 0, length = node->length ; i < length; i++) {
    struct directory_entry *entry = node->direct_block_pointer_directories[i];
    print_string("%s\n", entry->d_name);
    if (strcmp(entry->d_name, name) == 0) {
      return entry->d_ino;
    }
  }

  return NULL;
}

void in_memory_ls(ino_t root, char *name) {
  ino_t ino = find(root, name);
  struct inode *node = inode_lookup(memoryChunk, ino);
  printf("type: %d\n", node->type_and_permissions);
  printf("length: %d\n", node->length);
  for (int i = 0, length = node->length ; i < length; i++) {
    struct directory_entry *entry = node->direct_block_pointer_directories[i];
    printf("Child %d: File name: '%s'\n", i, entry->d_name);
  }
}

void in_memory_cat(ino_t root, char *name) {
  ino_t ino = find(root, name);
  struct inode *node = inode_lookup(memoryChunk, ino);
  char content[BUFSIZ];
  readContent(ino, content, node->length, 0);
  printf("The file content is: %s\n", content);
}

void touch(ino_t root, char *name) {
  ino_t ino = find(root, name);

  if (ino != NULL) {
    // TODO: Update the time modified
    return;
  }

  // Create inode for file
  ino_t node = createINodeFile();
  createDirectoryEntry(root, name, node);
}

///////////////////////////////////////////////////////////////////////////
// Virtual file system
///////////////////////////////////////////////////////////////////////////

typedef enum {
  O_RDONLY = 0,
  O_WRONLY = 1,
  O_CREATE = 2,
  O_TRUNC = 4,
} open_flag;

typedef enum {
  SUCCESS = 0,
  ERROR_NOT_IMPLEMENTED = 1
} result_code;

#define MAX_FILE_DESCRIPTORS 14

typedef size_t vfs_index;

struct vfs_inode {
  vfs_index index;
  uint32_t block_count;

  struct vfs_super_block *super_block;

  struct vfs_inode_operations *operations;
  struct vfs_file_operations *file_operations;
};

struct vfs_super_block {
  struct vfs_super_block_operations *operations;
  struct vfs_directory_entry *root;

  uint32_t blocksize;

  uint32_t current_nodes;
  uint32_t current_blocks;

  uint32_t max_nodes;
  uint32_t max_blocks;
};

struct vfs_super_block_operations {
  struct vfs_node *(*alloc_node)(struct vfs_super_block*);
};

struct vfs_directory_entry {
  char name[MAX_FILE_NAME];
  struct vfs_inode *inode;
  struct vfs_directory_entry *parent;
};

struct vfs_inode_operations {
  struct vfs_directory_entry *(*mkdir)(struct vfs_inode*, struct vfs_directory_entry*);
  // For some reason this will return null, or a negative error code
  struct vfs_directory_entry *(*lookup)(struct vfs_inode*, struct directory_entry*);
};

struct vfs_file {
  struct vfs_file_operations *file_operations;
  // The current file's vfs_inode
  struct vfs_inode *inode;

  // The O_XXX flags that were assigned when `open` was called
  open_flag open_flags;

  // The current associated vfs_directory_entry
  struct vfs_directory_entry *directory_entry;

  // The maximum number of bytes which can be read at a time
  uint32_t max_read_size;

  rw_offset current_offset;
};

struct vfs_file_operations {
  ssize_t (*read)(struct vfs_file*, char *buffer, size_t size, rw_offset offset);
  ssize_t (*write)(struct vfs_file*, char *buffer, size_t size, rw_offset offset);
  result_code (*open)(struct inode *, struct vfs_file *);
};
///////////////////////////////////////////////////////////////////////////
// User space values
///////////////////////////////////////////////////////////////////////////


// TODO: mode_t flags - https://jameshfisher.com/2017/02/24/what-is-mode_t/
// TODO: http://tldp.org/LDP/lki/lki-3.html
// typedef unsigned short mode_t;

#define MAX_OPENED_FILES 32
typedef uint32_t file_descriptor_index;

struct file_descriptor_table {
  uint32_t current_size;
  uint32_t maxsize;
  struct vfs_file *files;
};

///////////////////////////////////////////////////////////////////////////
// Simple programs for viewing the file system
///////////////////////////////////////////////////////////////////////////

struct file_system {
  struct vfs_directory_entry *root;
  struct vfs_directory *current_working_directory;
};

struct environment {
  struct file_system *file_system;
  struct file_descriptor_table *file_descriptor_table;
};

static struct environment *environment;

// void vfs_opendir(VFSNode node) {

// }

// void fs_ls(VFSNode node) {
  // if (node->)
// }

// VFSNode *vfs_alloc_node(VFSsuper_block *super_block) {
//   if (super_block->operations->alloc_node != NULL) {
//       return super_block->operations->alloc_node(super_block);
//   }
//   return NULL;
// }


static struct vfs_directory_entry *in_memory_mkdir(struct vfs_inode* node, struct vfs_directory_entry *entry) {
  struct inode *parentNode = inode_lookup(memoryChunk, node->index);
  // if (isDirectory(parentNode)) {
    // Detect valid
  // }

  // Create the new directory entry
  struct directory_entry *memory_entry = (struct directory_entry *) directory_entry_lookup(memoryChunk, blockCount++);

  size_t len = strlen(entry->name);
  strncopy(memory_entry->d_name, entry->name, len);
  memory_entry->d_namelen = len;
  memory_entry->d_ino = entry;

  // Store a reference to the newly created directory entry within the parent inode
  parentNode->direct_block_pointer_directories[parentNode->length] = memory_entry;
  (parentNode->length)++;
}

static ssize_t *in_memory_read(struct vfs_file* file, char *buf, ssize_t bytes_to_read, rw_offset offset) {
  return readContent(file->inode->index, buf, bytes_to_read, offset);
}

// Forward reference
struct vfs_directory_entry *in_memory_lookup(struct vfs_inode *parent, struct vfs_directory_entry *entry);
static struct vfs_inode_operations inode_operations = {
  .mkdir = in_memory_mkdir,
  .lookup = in_memory_lookup
};

static struct vfs_file_operations file_operations = {
  .read = in_memory_read
};

struct vfs_inode *in_memory_alloc_node(struct vfs_super_block *super_block) {
  struct vfs_inode *vfs_inode = malloc(sizeof(struct vfs_inode));
  memset(vfs_inode, 0, sizeof(struct vfs_inode));
  vfs_inode->super_block = super_block;
  vfs_inode->operations = &inode_operations;
  vfs_inode->file_operations = &file_operations;

  return vfs_inode;
};

struct vfs_directory_entry *in_memory_lookup(struct vfs_inode *parent, struct vfs_directory_entry *entry) {
  struct inode *node = inode_lookup(memoryChunk, parent->index);

  for (int i = 0, length = node->length ; i < length; i++) {
    struct directory_entry *current_entry = node->direct_block_pointer_directories[i];
    if (strcmp(current_entry->d_name, entry->name) == 0) {

      struct vfs_inode *vfs_node = in_memory_alloc_node(parent->super_block);
      vfs_node->index = current_entry->d_ino;

      entry->inode = vfs_node;
      entry->parent = parent;
      strncopy(entry->name, current_entry->d_name, MAX_FILE_NAME);

      return entry;
    }
  }

  return NULL;
}

static struct vfs_super_block_operations super_block_operations = {
  .alloc_node = in_memory_alloc_node
};

struct vfs_super_block* init_super_block(ino_t root_ino) {
  struct vfs_super_block *super_block = (struct vfs_super_block *) malloc(sizeof(struct vfs_super_block));
  memset(super_block,0, sizeof(struct vfs_super_block));
  super_block->blocksize = BLOCK_SIZE;
  super_block->current_blocks = 0;
  super_block->current_nodes = 0;
  super_block->max_nodes = MAX_INODES;
  super_block->max_blocks = (MAX_FILESYSTEM_SIZE - (MAX_INODES * BLOCK_SIZE)) / BLOCK_SIZE;
  super_block->operations = &super_block_operations;

  struct inode *node = inode_lookup(memoryChunk, root_ino);

  struct vfs_inode *root_inode = in_memory_alloc_node(super_block);
  root_inode->index = ROOT_INODE;
  root_inode->block_count = node->length;
  root_inode->operations = &inode_operations;

  struct vfs_directory_entry *root_directory_entry = (struct vfs_directory_entry*) malloc(sizeof(struct vfs_directory_entry));
  root_directory_entry->inode = root_inode;
  strncopy(root_directory_entry->name, "/", FILE_NAME_SIZE);
  root_directory_entry->parent = root_directory_entry;

  super_block->root = root_directory_entry;

  return super_block;
}

// Examples:
// /foo = foo
// /foo/bar = foo
// ../foo = foo
// . = invalid
// / = invalid
// foo.txt = txt.
// foo/bar = bar - not supported
char *next_path_segment(char *path) {
  bool hasSeenSlash = false;
  int length = 0;
  char *startPointer = path;
  char *endPointer = path;

  while (*endPointer) {
    if (hasSeenSlash) {
      if (*endPointer == '/' || *endPointer == '\0') {
        endPointer--;
        length--;
        break;
      }
      endPointer++;
    } else {
      if (*startPointer == '/') {
        hasSeenSlash = true;
      }
      startPointer++;
      endPointer++;
    }

    length++;
  }

  if (!hasSeenSlash) {
    return path;
  }

  char *result = (char *) malloc(length);
  strncopy(result, startPointer, length);
  result[length + 1] = '\0';
  return result;
}

// /foo = /
// foo/bar = foo
// foo = .
char *parent_path(char *path) {
  bool hasSeenSlash = false;
  char *startPointer = path;
  char *parentPointer = path + 1;
  char *endPointer = path + 1;

  while (*endPointer) {
    if (*endPointer == '/') {
      hasSeenSlash = true;
      parentPointer = endPointer;
    }
    endPointer++;
  }

  if (!hasSeenSlash && *path == '/') {
    return "/";
  }
  if (!hasSeenSlash) {
    return ".";
  }

  int length = parentPointer - startPointer;
  char *result = (char *) malloc(parentPointer - startPointer);
  strncopy(result, startPointer, length);
  result[length + 1] = '\0';
  return result;
}

char *last_path_segment(char *path) {
  char *startPointer = path;
  char *endPointer = path;

  while (*endPointer) {
    if (*startPointer == '/') {
      startPointer++;
      continue;
    }

    endPointer++;
    if (*endPointer == '/') {
      startPointer = endPointer;
      continue;
    }
  }

  return startPointer;
}

char *remaining_path_segments(char *path) {
  char *startPointer = path;

  for (;;) {
    if (*startPointer == '/') {
      startPointer++;
      break;
    }
    if (*startPointer == '\0') {
      break;
    }

    startPointer++;
  }

  return startPointer;
}

struct vfs_directory_entry *alloc_vfs_directory_entry(struct vfs_directory_entry *parent, char *name) {
  struct vfs_directory_entry *entry = (struct vfs_directory_entry*) malloc(sizeof(struct vfs_directory_entry));
  strncopy(entry->name, name, FILENAME_MAX);
  return entry;
}

struct vfs_directory_entry *vfs_lookup(char *path) {
  struct vfs_directory_entry *parent;

  char *remaining_path;
  if (*path == '/') {
    parent = environment->file_system->root;
    // Skip the root `/` as we've resolved it now
    remaining_path = remaining_path_segments(path);
  } else {
    parent = environment->file_system->current_working_directory;
    remaining_path = path;
  }

  for(;;) {
    char *next_segment = next_path_segment(remaining_path);
    struct vfs_directory_entry *child = alloc_vfs_directory_entry(parent, next_segment);
    parent = parent->inode->operations->lookup(parent->inode, child);
    if (!parent) {
      return parent;
    }

    remaining_path = remaining_path_segments(remaining_path);
    if (!*remaining_path) {
      return parent;
    }
  }

  return parent;
}

result_code vfs_mkdir(struct vfs_inode *inode, struct vfs_directory_entry *directory_entry) {
  if (inode->operations->mkdir != NULL) {
    vfs_index index = inode->operations->mkdir(inode, directory_entry);
    return SUCCESS;
  }
  return ERROR_NOT_IMPLEMENTED;
}

result_code mkdir(char *path) {
  char *parentPath = parent_path(path);
  struct vfs_directory_entry *parent = vfs_lookup(parentPath);
  // if (directory_entry == NULL) {
  // }

  char *folderName = last_path_segment(path);
  struct vfs_directory_entry *child = (struct vfs_directory_entry*) malloc(sizeof(struct vfs_directory_entry));
  strncopy(child->name, folderName, FILE_NAME_SIZE);

  parent->inode->operations->mkdir(parent->inode, child);
  return SUCCESS;
}

struct vfs_file *alloc_empty_file() {
  // TODO: Properly allocate files, reusing them if possible etc.
  struct vfs_file *file = (struct vfs_file*) malloc(sizeof(struct vfs_file));
  memset(file, 0, sizeof(struct vfs_file));

  return file;
}

struct vfs_file *open_file(const char *filename, open_flag open_flags) {
  if (!(open_flags & O_CREATE)) {
    struct vfs_directory_entry *entry = vfs_lookup(filename);

    // TODO: Properly allocate files
    struct vfs_file *file = alloc_empty_file();
    file->inode = entry->inode;
    file->directory_entry = entry;
    file->current_offset = 0;
    file->file_operations = entry->inode->file_operations;
    file->open_flags = open_flags;

    if (entry->inode->file_operations->open != NULL) {
      // TODO: Correctly detect error code
      entry->inode->file_operations->open(entry->inode, file);
    }

    return file;
  };

  if (open_flags & O_CREATE) {
    char *folder_name = last_path_segment(filename);
    struct vfs_directory_entry *entry = vfs_lookup(filename);

    entry->inode->file_operations->open


  }
};

// TODO: Support truncation / create flags etc.
file_descriptor_index open(char *path, open_flag open_flag) {
  struct vfs_file *file = open_file(path, open_flag);
  // TODO: Properly allocate files
  struct file_descriptor_table *file_descriptor_table = environment->file_descriptor_table;
  file_descriptor_index index = (file_descriptor_table->current_size);
  file_descriptor_table->current_size += 1;
  file_descriptor_table->files[index].current_offset = file->current_offset;
  file_descriptor_table->files[index].max_read_size = file->max_read_size;
  file_descriptor_table->files[index].directory_entry = file->directory_entry;
  file_descriptor_table->files[index].inode = file->inode;
  file_descriptor_table->files[index].file_operations = file->file_operations;

  return index;
}

ssize_t read(file_descriptor_index index, char *buffer, size_t bytes_to_read) {
  // TODO: In the future this shouldn't be a finite array.
  struct vfs_file *file = &environment->file_descriptor_table->files[index];
  ssize_t amount_read = file->file_operations->read(file, buffer, bytes_to_read, file->current_offset);
  file->current_offset += amount_read;
  return amount_read;
}

void close(file_descriptor_index fd) {
  // TODO
}

void ls(char *path) {
  // file_descriptor_index fd = open(path);
}

void cat(char *path) {
  file_descriptor_index fd = open(path, O_RDONLY);
  // TODO: Max this more efficient and don't read one character at a time
  char current = '\0';
  char second_last = '\0';

  do {
    second_last = current;
    read(fd, &current, 1);
    if (current != '\0') {
      print_string("%c", current);
    }
  } while(current != '\0');

  if (second_last != '\n') {
    print_string("\n");
  }

  close(fd);
}

void mkfile(char *path) {
  int fd = open(path, O_CREATE);
  close(fd);
}

int main(void) {
  // TODO: In the future this would read from ext etc.
  ino_t root = initRamdisk();
  struct vfs_super_block *super_block = init_super_block(root);

  char tests[][20] = {
    // "/",
    // "/foo",
    // "/foo/bar",
    // "/foo/bar/baz.txt",
    // "foo/bar/baz.txt",
    // "bar/baz.txt",
    // "foo",
    // "/helloWorld.txt",
    "helloWorld.txt",
    // "/"
  };

  for (int i = 0 ; i < sizeof(tests) / sizeof(tests[0]); i++) {
    printf("%s\n", tests[i]);
    printf("-------------\n");
    printf("next = %s\n", next_path_segment(tests[i]));
    printf("last = %s\n", last_path_segment(tests[i]));
    printf("parent = %s\n", parent_path(tests[i]));
    printf("remaining = %s\n", remaining_path_segments(tests[i]));
    printf("\n\n");
  }

  char path[] = "/foo/bar";
  printf("%s\n", path);
  printf("next segment: %s\n", (path));
  printf("last segment: %s\n", last_path_segment(path));

  // Create the global file system and assign the init ramdisk
  environment = (struct environment*) malloc(sizeof(struct environment));
  memset(environment, 0, sizeof(struct environment));

  struct file_system *file_system = (struct file_system*) malloc(sizeof(file_system));
  file_system->root = super_block->root;
  file_system->current_working_directory = super_block->root;
  environment->file_system = file_system;

  struct file_descriptor_table *file_descriptor_table = (struct file_descriptor_table*) malloc(sizeof(file_descriptor_table));
  file_descriptor_table->current_size = 0;
  file_descriptor_table->maxsize = MAX_OPENED_FILES;
  file_descriptor_table->files = (struct file*) malloc(sizeof(struct vfs_file) * MAX_OPENED_FILES);
  environment->file_descriptor_table = file_descriptor_table;

  printf("Getting started!\n");
  printf("Total stuff for now %d\n", super_block->current_blocks);

  super_block->operations->alloc_node(super_block);
  printf("After --- %d\n", super_block->current_blocks);

  super_block->operations->alloc_node(super_block);

  // mkdir("/foo");

  // VFSNode root =
  // vfs_ls(root);

  // print_string("> ls /\n");
  // ls(root, "/");
  // print_string("> cat helloWorld.txt\n");
  // cat(root, "helloWorld.txt");
  // print_string("> ls subFolder\n");
  // ls(root, "subFolder");
  // print_string("> touch newFile.txt\n");
  // touch(root, "newFile.txt");
  print_string("> ls\n");
  ls("/subFolder");

  // file_descriptor_index helloWorldTxtFd = open("/helloWorld.txt");
  // printf("Successfully opened the file with %d\n", helloWorldTxtFd);

  // printf("The content of file descriptor %d was %s\n", helloWorldTxtFd, buf);

  // print_string("> cat /helloWorld.txt\n");
  // cat("/helloWorld.txt");

  // print_string("cp /helloWorld.txt /newFile.txt");
  // cp("/helloWorld.txt", "/newFile.txt");
  // cat("/newFile.txt");

  print_string("mkfile /touchedFile.txt");
  mkfile("/touchedFile.txt");
}
