#include "../drivers/screen.h"
#include "../cpu/timer.h"
#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "./util.h"
#include "../drivers/keyboard.h"
#include "../drivers/ports.h"
#include "./mem.h"
#include "./paging.h"
#include "./fs.h"
#include "./kprint.h"

#define SYSTEM_CLOCK_HZ 100
#define RED "\e[31m"
#define GREEN "\e[32m"
#define YELLOW "\e[33m"
#define CYAN "\e[36m"
#define RESET "\e[0m"

void print_help() {
    print_string("  " GREEN "ticks" RESET " - print the total cpu ticks since boot\n");
    print_string("  " GREEN "count" RESET " - count in hex\n");
    print_string("  " GREEN "kmalloc" RESET " - test kmalloc\n");
    print_string("  " GREEN "divide_zero" RESET " - test page fault handling\n");
    print_string("  " GREEN "page_fault" RESET " - test page fault handling\n");
    print_string("  " GREEN "halt" RESET " - halt the machine\n");
    print_string("  " GREEN "help" RESET " - print the available instructions\n");
}

void test_kmalloc() {
    uint32_t allocated_memory = kmalloc(32, 1);
    print_string("Allocated memory: %x\n", allocated_memory);
}

void on_user_input(char *user_input) {
    kprintf("User input detected\n");

    if (strcmp(user_input, "ticks") == 0) {
        print_string("the total cpu ticks since boot is: " YELLOW "%d" RESET "\n", get_tick());
    } else if (strcmp(user_input, "halt") == 0) {
        print_string("halting...\n");
        asm volatile("hlt");
    } else if (strcmp(user_input, "help") == 0) {
        print_help();
    } else if (strcmp(user_input, "kmalloc") == 0) {
        test_kmalloc();
    } else if (strcmp(user_input, "count") == 0) {
        for (int i = 0 ; i <= 32; i++) {
            print_string("int: %d - hex: " YELLOW "%x" RESET "\n", i, i);
        }
    } else if (strcmp(user_input, "divide_zero") == 0) {
        print_string(RED "about to divide by zero fault\n" RESET);

        uint32_t explosion = 10 / 0;
    } else if (strcmp(user_input, "page_fault") == 0) {
        print_string(RED "about to page fault\n" RESET);

        uint32_t *ptr = (uint32_t*) 0xA0000000;
        uint32_t do_page_fault = *ptr;
    } else {
        print_string(RED "unknown command\n" RESET);
        print_help();
    }
    print_string("> ");
}

// typedef struct FileSystemNode {
//     char name[128];
// } FileSystemNode;

#define FILE_NAME_SIZE 128
#define MAXIMUM_FILES 256
#define MAXIMUM_CHILDREN 16
#define MAX_IN_MEMORY_FILE_CONTENT_LENGTH 128

// In Memory Filesystem

enum InMemoryFileType {
    IN_MEMORY_FILE = 0,
    IN_MEMORY_DIRECTORY = 1 << 0
};

struct InMemoryFileNode {
  char name[FILE_NAME_SIZE];
  uint32_t length; // In bytes
  char content[MAX_IN_MEMORY_FILE_CONTENT_LENGTH];
  enum InMemoryFileType type;
  uint32_t childrenCount;
  struct InMemoryFileNode *children;
};

struct InMemoryFileNode createInMemoryFile(
  char *name,
  char *content
) {
  struct InMemoryFileNode InMemoryFileNode;
  strncopy(InMemoryFileNode.name, name, FILE_NAME_SIZE);
  strncopy(InMemoryFileNode.content, content, MAX_IN_MEMORY_FILE_CONTENT_LENGTH);
  InMemoryFileNode.length = strlen(InMemoryFileNode.content);
  InMemoryFileNode.type = IN_MEMORY_FILE;
  InMemoryFileNode.childrenCount = 0;
  InMemoryFileNode.children = NULL;

  return InMemoryFileNode;
}

struct InMemoryFileNode createInMemoryDirectory(
  char *name,
  int childrenCount,
  struct InMemoryFileNode *children
) {
  struct InMemoryFileNode InMemoryFileNode;
  strncopy(InMemoryFileNode.name, name, FILE_NAME_SIZE);
  strncopy(InMemoryFileNode.content, "", MAX_IN_MEMORY_FILE_CONTENT_LENGTH);
  InMemoryFileNode.length = 0;
  InMemoryFileNode.type = IN_MEMORY_DIRECTORY;
  InMemoryFileNode.childrenCount = childrenCount;
  InMemoryFileNode.children = children;

  return InMemoryFileNode;
}

void mockPrintTreeDescription(struct InMemoryFileNode node, int childCount, int childrenCount, int depth) {
  for (int i = 0 ; i < depth ; i++) {
    if (i % 2 == 0) {
      print_string("| ");
    }
  }

  if (childCount < childrenCount - 1) {
    print_string("|- ");
  } else {
    if (node.type == IN_MEMORY_DIRECTORY) {
      print_string("|- ");
    } else {
      print_string("^- ");
    }
  }

  print_string(
    "%s\n",
    node.name
  );
}

void mockPrintTreeRecursive(struct InMemoryFileNode node, int childCount, int childrenCount, int depth) {
  if (node.type == IN_MEMORY_FILE) {
    mockPrintTreeDescription(node, childCount, childrenCount, depth);
  } else {
    mockPrintTreeDescription(node, 0, 0, depth);

    for (uint32_t i = 0 ; i < node.childrenCount ; i++ ){
      mockPrintTreeRecursive(node.children[i], i, node.childrenCount, depth + 1);
    }
  }
}

void printInMemoryTree(struct InMemoryFileNode node) {
  int depth = 0;

  print_string(".\n");

  for (uint32_t i = 0 ; i < node.childrenCount ; i++ ){
    mockPrintTreeRecursive(
      node.children[i], i, node.childrenCount, depth
    );
  }
  print_string("\n");
}

void readInMemoryFile(struct InMemoryFileNode node, char *source) {
  strncopy(source, node.content, MAX_IN_MEMORY_FILE_CONTENT_LENGTH);
}

void cat(struct InMemoryFileNode node) {
  if (node.type == IN_MEMORY_DIRECTORY) {
    print_string("%s is a directory\n", node.name);
    return;
  }

  char contents[MAX_IN_MEMORY_FILE_CONTENT_LENGTH];
  readInMemoryFile(node, contents);
  print_string("%s\n", contents);
}

void filesystem_test() {
  struct InMemoryFileNode helloFolderChildren[4] = {
    createInMemoryFile(
      "a.txt",
      "the content of a"
    ),
    createInMemoryFile(
      "b.txt",
      "the content of b"
    ),
    createInMemoryFile(
      "c.txt",
      "the content of c"
    ),
    createInMemoryFile(
      "d.txt",
      "the content of d"
    ),
  };

  struct InMemoryFileNode helloFolder = createInMemoryDirectory(
    "helloFolder",
    4,
    helloFolderChildren
  );

  struct InMemoryFileNode helloWorldTxt = createInMemoryFile(
    "helloWorld.txt",
    "hello world, this is some content!"
  );

  struct InMemoryFileNode rootChildren[2] = {
    helloFolder,
    helloWorldTxt
  };

  struct InMemoryFileNode root = createInMemoryDirectory(
    "root",
    2,
    rootChildren
  );

  print_string("> tree\n");
  printInMemoryTree(root);

  print_string("> cat root\n");
  cat(root);

  print_string("> cat root/helloWorld.txt\n");
  cat(root.children[1]);

  print_string("> cat root/helloFolder/a.txt\n");
  cat(root.children[0].children[0]);
}

void kernel_main() {
    clear_screen();
    load_idt();
    isr_install();
    init_timer(SYSTEM_CLOCK_HZ);
    init_keyboard();
    init_paging();

    kprintf("Boot up successfully\n");

    print_string("type " CYAN "help" RESET " to see the available commands\n");
    filesystem_test();
    print_string("> ");
}
