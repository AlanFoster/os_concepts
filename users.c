#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define CLR "\033[0m"

#define MAX_USERS 10
#define MAX_NAME_SIZE 30

#define CREATE_USER 1
#define LIST_USERS 2
#define QUIT 0

typedef enum {
  DB_SUCCESS,
  DB_FAIL,
} database_result;

struct user {
  char name[MAX_NAME_SIZE];
  int age;
};

struct user_database {
  int count;
  struct user users[MAX_USERS];
  char *path;
};

struct user_database *user_database(char *path) {
  struct user_database *user_database = (struct user_database*) calloc(sizeof(struct user_database), 1);
  user_database->path = path;
  return user_database;
}

void db_free(struct user_database *user_database) {
  free(user_database);
}

database_result db_load(struct user_database *user_database) {
  struct stat file_stat;
  if (stat(user_database->path, &file_stat) < 0) {
    return DB_SUCCESS;
  }

  FILE *file = fopen(user_database->path, "ab+");
  if (file == NULL) {
    return DB_FAIL;
  }

  user_database->count = 0;
  while (user_database->count < MAX_USERS) {
    struct user user;
    fread(&user, sizeof(struct user), 1, file);
    if (feof(file)) {
      break;
    }
    user_database->users[user_database->count] = user;
    user_database->count++;
  }

  fclose(file);

  return DB_SUCCESS;
}

database_result db_save(struct user_database *user_database) {
  FILE *file = fopen(user_database->path, "wb+");
  if (file == NULL) {
    return DB_FAIL;
  }

  ssize_t result = fwrite(user_database->users, sizeof(struct user), user_database->count, file);
  if (result < 0) {
    return DB_SUCCESS;
  }

  fclose(file);

  return DB_SUCCESS;
}

database_result db_save_user(struct user_database *db, struct user user) {
  if (db->count == MAX_USERS) {
    return DB_FAIL;
  }
  db->users[db->count] = user;
  db->count++;

  db_save(db);

  return DB_SUCCESS;
}

void print_menu() {
  printf("menu\n");
  printf("----\n");
  printf("1) create user\n");
  printf("2) list users\n");
  printf("0) quit\n");
  printf("\n");
}

int read_user_number(char *message) {
  char buffer[MAX_NAME_SIZE];
  char *pending;
  long int result;

  for (;;) {
    printf("%s%s%s", GREEN, message, CLR);

    fgets(buffer, sizeof(buffer), stdin);

    result = strtol(buffer, &pending, 10);

    if (errno == EINVAL) {
      printf(RED "Input was not a valid number\n" CLR);
      continue;
    }

    if (errno == ERANGE) {
      printf(RED "Input was out of range\n" CLR);
      continue;
    }

    if (&buffer[0] == pending) {
      printf(RED "Input was not a valid number\n" CLR);
      continue;
    }

    if (!(*pending == '\0' || *pending == '\n')) {
      printf(RED "Only numbers are supported\n" CLR);
      continue;
    }

    return result;
  }
}

int read_user_option() {
  return read_user_number(GREEN "Enter your option:\n" CLR);
}

void read_create_user(struct user *user) {
  char buffer[MAX_NAME_SIZE];

  printf("What is the first name?\n");
  fgets(buffer, sizeof(buffer), stdin);
  buffer[strcspn(buffer, "\n")] = 0;

  strncpy(user->name, buffer, MAX_NAME_SIZE);

  user->age = read_user_number("What is the age?\n");
}

void print_user(struct user user) {
  printf("  name: %s\n", user.name);
  printf("  age: %d\n", user.age);
  printf("\n");
}

void print_users(struct user *users, int count) {
  if (count == 0) {
    printf("there are no users in the system yet.\n");
  } else {
    printf("users\n");
    printf("-----\n");
    for (int i = 0 ; i < count ; i++ ) {
        print_user(users[i]);
    }
  }
}

int main(int _argc, char *_argv[]) {
  int option;
  struct user temp_user;
  memset(&temp_user, 0, sizeof(struct user));
  struct user_database *db = user_database("./user_db");
  if (db_load(db) != DB_SUCCESS) {
    printf(RED "failed loading database: %s\n" CLR, db->path);
    return 1;
  }

  do {
    print_menu();
    option = read_user_option();

    switch (option) {
      case QUIT:
        printf("Closing...\n");
        break;

      case CREATE_USER:
        read_create_user(&temp_user);
        if (db_save_user(db, temp_user) == DB_SUCCESS) {
          printf("Successfully saved user:\n");
          print_user(temp_user);
        } else {
          printf("Oops; there was a problem saving the user\n");
        }

        break;

      case LIST_USERS:
        print_users(db->users, db->count);
        break;

      default:
        printf(RED "option not valid, please try again.\n" CLR);
    }

  } while (option != QUIT);

  printf("Closed\n");
  db_free(db);

  return 0;
}