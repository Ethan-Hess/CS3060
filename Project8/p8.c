#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

long long total_size = 0;

void list_directory(const char *path, int indent_level)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip entries starting with '.'
        if (entry->d_name[0] == '.')
        {
            continue;
        }

        // Build full path using asprintf
        char *full_path = NULL;
        if (asprintf(&full_path, "%s/%s", path, entry->d_name) == -1)
        {
            perror("asprintf");
            continue;
        }

        // Get file stats
        struct stat sb;
        if (lstat(full_path, &sb) == -1)
        {
            perror("lstat");
            free(full_path);
            continue;
        }

        // Print indentation
        for (int i = 0; i < indent_level * 2; i++)
        {
            printf(" ");
        }

        // Check if it's a directory
        if (S_ISDIR(sb.st_mode))
        {
            printf("dir %s\n", full_path);
            list_directory(full_path, indent_level + 1);
        }
        // Check if it's a regular file
        else if (S_ISREG(sb.st_mode))
        {
            printf("%lld:%s\n", (long long)sb.st_size, entry->d_name);
            total_size += sb.st_size;
        }
        // Ignore everything else (symlinks, sockets, devices, etc.)

        free(full_path);
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    const char *target_path = ".";

    if (argc > 2)
    {
        fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
        return 1;
    }

    if (argc == 2)
    {
        target_path = argv[1];
    }

    list_directory(target_path, 0);
    printf("Total file space used:%lld\n", total_size);

    return 0;
}
