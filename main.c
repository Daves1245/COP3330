#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#define MAX_K 100
#define MAX_PATH_LENGTH 501
#define MAX_LINE_LENGTH 300
#define N 500

#define ERR(msg) { \
    fprintf(stderr, (msg));\
    exit(EXIT_FAILURE); \
}

// TODO add colors to error prints

char *bin_name;

char visited[MAX_PATH_LENGTH][N];
int visited_i;

struct q_entry {
    char path[MAX_PATH_LENGTH];
    struct q_entry *prev, *next;
} *queue = NULL;

void enqueue(const char *path) {
    struct q_entry *entry = malloc(sizeof(*entry));
    if (!entry) {
        ERR("Out of memory!");
    }

    memset(entry, 0, sizeof(*entry));
    strcpy(entry->path, path);
    entry->next = queue;
    /*
    entry->prev = queue->prev;
    queue->prev->next = entry;
    queue->prev = entry;
    */
    queue = entry;
}

struct q_entry *dequeue(void) {
    struct q_entry *ret = queue;
    queue = queue->next;
    return ret;
}

void destroy(struct q_entry *entry) {
    memset(entry, 0, sizeof(*entry));
    free(entry);
}

void usage(void) {
    printf("usage: %s [file/directory] (file/directory)\n", bin_name);
}

void convert_package_name(char *dest, const char *source) {
    for (int i = 0; i < strlen(source); i++) {
        if (source[i] == '/') {
            dest[i] = '.';
        } else {
            dest[i] = source[i];
        }
    }
}

void verify(const char *filename) {
    FILE *fp;
    char package_name[MAX_LINE_LENGTH];
    char converted_package_name[MAX_LINE_LENGTH];

    memset(package_name, 0, sizeof package_name);
    memset(converted_package_name, 0, sizeof converted_package_name);
    if (!(fp = fopen(filename, "r+"))) {
        perror("fopen");
        ERR("DEBUG: fopen returned null!");
    }

    if (fscanf(fp, "package %s;", package_name) != 1) {
        printf("`%s` Does not have a package declaration!\n", filename);
    }


    /*
     * TODO this should be given the root_path that bfs was started with,
     * that way we can properly determine the real package path
    convert_package_name(converted_package_name, filename);
    printf("`%s` vs `%s`\n", package_name, converted_package_name);
    if (strcmp(package_name, converted_package_name) != 0) {
        printf("`%s`'s declared package name does not match its file hierarchy!\n");
    }
    */
    fclose(fp);
}

// TODO move add to visited here
// TODO: visit/verify everything, but only filter out java through
// verify? what's the cleanest solution here?
void visit(const char *filename) {
    if (!matches_extension(filename, ".java")) {
        return;
    }
    // printf("Visited %s!\n", filename);
    verify(filename);
}

int was_visited(const char *key) {
    for (int i = 0; i < visited_i; i++) {
        if (strcmp(visited[i], key) == 0) {
            return 1;
        }
    }
    return 0;
}

int matches_extension(const char *filename, const char *ext) {
    return strcmp(filename + strlen(filename) - strlen(ext), ext) == 0;
}

void bfs(const char *root_path) {
    char path[MAX_PATH_LENGTH];
    memset(path, 0, sizeof path);
    enqueue(root_path);
    // for emphasis that the passed argument is the _root_ path
    // (as in the outmost package)
    // at least until there are optimizations for checked-visited paths
    while (queue) {
        /*
        printf("Queue is: ");
        for (struct q_entry *iterator = queue; iterator;
                iterator = iterator->next) {
            printf("%s ", iterator->path);
        }
        printf("\n");
        */
        struct q_entry *root = dequeue();
        // TODO don't assume this works!
        realpath(root->path, path);
        DIR *cwd;
        struct dirent *dir;
        // printf("Entering `%s`\n", path);
        if (!(cwd = opendir(path))) {
            perror("opendir");
            ERR("Could not open directory!");
        }
        while (dir = readdir(cwd)) {
            char entry_path[MAX_PATH_LENGTH];
            if (was_visited(dir->d_name) ||
                    strcmp(dir->d_name, ".") == 0 ||
                    strcmp(dir->d_name, "..") == 0 ||
                    strcmp(dir->d_name, ".git") == 0) {
                continue;
            }
            memset(entry_path, 0, sizeof entry_path);
            sprintf(entry_path, "%s/%s", path, dir->d_name);

            if (dir->d_type == DT_DIR) {
                // printf("Enqueueing %s\n", entry_path);
                enqueue(entry_path);
                strcpy(visited[visited_i++], entry_path);
            } else if (dir->d_type == DT_REG) {
                visit(entry_path);
                strcpy(visited[visited_i++], entry_path);
            } else {
                printf("Unsupported filetype: %s\n", entry_path);
            }
        }
        closedir(cwd);
        destroy(root);
    }
}

int main(int argc, char **argv) {
    int fd;
    bin_name = argv[0];
    if (argc < 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    // TODO add visited array for optimizations
    for (int i = 1; i < argc; i++) {
        bfs(argv[i]);
    }
    exit(EXIT_SUCCESS);
}
