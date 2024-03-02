#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdbool.h>

#define STATUS_BAD_NO_OF_ARGS 1
#define MAX_COMMAND 10
#define DIRECTORY_SIZE 100000
#define VAL_SIZE 1000000
#define PERMISSION 11

int directoryExists(char *dirPath)
{
    struct stat info;
    if (stat(dirPath, &info) != 0)
    {
        return 0;
    }
    if (S_ISDIR(info.st_mode))
        return 1;
    else
        return 0;
}

int fileExists(char *filePath)
{
    struct stat info;
    if (stat(filePath, &info) != 0)
    {
        return 0;
    }
    if (S_ISREG(info.st_mode))
        return 1;
    else
        return 0;
}

int checkInput(char *input, char *variant)
{
    if (sscanf(input, "%s", variant) < 1)
        return 2;
    else
        return 0;
}

void displayFiles(char *currDir)
{
    DIR *dir = opendir(currDir);

    if (dir == NULL)
    {
        perror("Could not open directory!");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        char path[4096];
        sprintf(path, "%s/%s", currDir, entry->d_name);
        if (strstr(path, "/.") == NULL && strstr(path, "/..") == NULL)
            printf("%s\n", path);
    }
    // free(entry);
    closedir(dir);
}

void displayFilesRecursively(const char *currDir)
{
    DIR *dir = opendir(currDir);

    if (dir == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        char path[4096];
        struct stat command;

        sprintf(path, "%s/%s", currDir, entry->d_name);

        if (lstat(path, &command) == -1)
        {
            perror("Failed to get file status");
            continue;
        }

        if (directoryExists(path))
        {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
            {
                continue;
            }
            printf("%s\n", path);
            displayFilesRecursively(path);
        }
        else
        {
            printf("%s\n", path);
        }
    }
    // free(entry);
    closedir(dir);
}

void sizeOfFiles(char *currDir, long value)
{
    DIR *dir = opendir(currDir);

    if (dir == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        char path[4096];
        struct stat command;

        sprintf(path, "%s/%s", currDir, entry->d_name);

        if (lstat(path, &command) == -1)
        {
            perror("Failed to get file status");
            continue;
        }

        if (S_ISREG(command.st_mode))
        {
            if (command.st_size < value)
                printf("%s\n", path);
        }
    }
    // free(entry);
    closedir(dir);
}

void sizeofFilesRecursive(char *currDir, long value)
{
    DIR *dir = opendir(currDir);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        char path[4096];
        struct stat command;

        sprintf(path, "%s/%s", currDir, entry->d_name);

        if (lstat(path, &command) == -1)
        {
            perror("Failed to get file status");
            continue;
        }

        if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
        {
            continue;
        }

        if (S_ISREG(command.st_mode))
        {
            if (command.st_size < value)
            {
                printf("%s\n", path);
            }
        }

        if (S_ISDIR(command.st_mode))
            sizeofFilesRecursive(path, value);
    }
    // free(entry);
    closedir(dir);
}

void permissionsList(char *currDir, char *permission)
{
    DIR *dir = opendir(currDir);

    if (dir == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        char path[4096];
        struct stat cm;

        sprintf(path, "%s/%s", currDir, entry->d_name);

        if (lstat(path, &cm) == -1)
        {
            perror("Failed to get file status");
            continue;
        }

        if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
        {
            continue;
        }

        char owner_perms[4];
        if (cm.st_mode & S_IRUSR)
            owner_perms[0] = 'r';
        else
            owner_perms[0] = '-';

        if (cm.st_mode & S_IWUSR)
            owner_perms[1] = 'w';
        else
            owner_perms[1] = '-';

        if (cm.st_mode & S_IXUSR)
            owner_perms[2] = 'x';
        else
            owner_perms[2] = '-';
        owner_perms[3] = '\0';
        char group_perms[4];

        if (cm.st_mode & S_IRGRP)
            group_perms[0] = 'r';
        else
            group_perms[0] = '-';
        if (cm.st_mode & S_IWGRP)
            group_perms[1] = 'w';
        else
            group_perms[1] = '-';
        if (cm.st_mode & S_IXGRP)
            group_perms[2] = 'x';
        else
            group_perms[2] = '-';
        group_perms[3] = '\0';
        char other_perms[4];

        if (cm.st_mode & S_IROTH)
            other_perms[0] = 'r';
        else
            other_perms[0] = '-';
        if (cm.st_mode & S_IWOTH)
            other_perms[1] = 'w';
        else
            other_perms[1] = '-';
        if (cm.st_mode & S_IXOTH)
            other_perms[2] = 'x';
        else
            other_perms[2] = '-';
        other_perms[3] = '\0';
        char perms[12];
        
        sprintf(perms, "%s%s%s", owner_perms, group_perms, other_perms);
        if (strcmp(perms, permission) == 0)
        {
            printf("%s\n", path);
        }
    }
    //free(entry);
    closedir(dir);
}

void permissionsRecursive(char *currDir, char *permission)
{
    DIR *dir = opendir(currDir);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        struct stat cm;
        char path[4096];
        // if (path == NULL)
        // {
        //     perror("Failed to allocate memory");
        //     free(entry);
        //     closedir(dir);
        //     return;
        // }
        sprintf(path, "%s/%s", currDir, entry->d_name);

        if (lstat(path, &cm) == -1)
        {
            perror("Failed to get file status");
            continue;
        }

        char owner_perms[4];
            if (cm.st_mode & S_IRUSR)
                owner_perms[0] = 'r';
            else
                owner_perms[0] = '-';

            if (cm.st_mode & S_IWUSR)
                owner_perms[1] = 'w';
            else
                owner_perms[1] = '-';

            if (cm.st_mode & S_IXUSR)
                owner_perms[2] = 'x';
            else
                owner_perms[2] = '-';
            owner_perms[3] = '\0';
            char group_perms[4];

            if (cm.st_mode & S_IRGRP)
                group_perms[0] = 'r';
            else
                group_perms[0] = '-';
            if (cm.st_mode & S_IWGRP)
                group_perms[1] = 'w';
            else
                group_perms[1] = '-';
            if (cm.st_mode & S_IXGRP)
                group_perms[2] = 'x';
            else
                group_perms[2] = '-';
            group_perms[3] = '\0';
            char other_perms[4];

            if (cm.st_mode & S_IROTH)
                other_perms[0] = 'r';
            else
                other_perms[0] = '-';
            if (cm.st_mode & S_IWOTH)
                other_perms[1] = 'w';
            else
                other_perms[1] = '-';
            if (cm.st_mode & S_IXOTH)
                other_perms[2] = 'x';
            else
                other_perms[2] = '-';
            other_perms[3] = '\0';
            char perms[12];
            sprintf(perms, "%s%s%s", owner_perms, group_perms, other_perms);

        if (S_ISREG(cm.st_mode))
        { 
            if (strcmp(perms, permission) == 0)
            {
                printf("%s\n", path);
            }
        }
        else
        {
            if (S_ISDIR(cm.st_mode))
            {
                if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                {
                    continue;
                }
                if (strcmp(perms, permission) == 0)
                    printf("%s\n", path);
                permissionsRecursive(path, permission);
            }
        }
    }
    //free(entry);
    closedir(dir);
}

void readParse(char *path)
{
    bool ok = true;
    int fd = open(path, O_RDONLY);
    lseek(fd, -3, SEEK_END);
    unsigned char header_size[2];
    read(fd, header_size, 2);
    header_size[2] = '\0';
    int hsize = 0;
    hsize += header_size[0];
    hsize += header_size[1] * 256;
    char magic;
    read(fd, &magic, 1);
    lseek(fd, -hsize, SEEK_END);
    unsigned char version;
    read(fd, &version, 1);
    unsigned char no_of_sections;
    read(fd, &no_of_sections, 1);
    for (int i = 0; i < no_of_sections; i++)
    {
        char name[6];
        char type[4];
        char offset[4];
        char size[4];

        read(fd, name, 6);
        read(fd, type, 4);
        read(fd, offset, 4);
        read(fd, size, 4);
        int ty = (int)type[0];
        if (ty != 15 && ty != 72 && ty != 17 && ty != 89)
        {
            ok = false;
            printf("ERROR\nwrong sect_types\n");
            return;
        }
    }
    if (magic != 'r')
    {
        ok = false;
        printf("ERROR\nwrong magic\n");
        return;
    }
    if (version < 71 || version > 180)
    {
        ok = false;
        printf("ERROR\nwrong version\n");
        return;
    }
    if (no_of_sections < 6 || no_of_sections > 19)
    {
        ok = false;
        printf("ERROR\nwrong sect_nr\n");
        return;
    }
    if (ok == true)
    {
        printf("SUCCESS\n");
        lseek(fd, -hsize + 2, SEEK_END);
        printf("version=%d\n", version);
        printf("nr_sections=%d\n", no_of_sections);
        char *name = malloc(6);
        char *type = malloc(4);
        char *offset = malloc(4);
        char *size = malloc(4);
        for (int i = 1; i <= no_of_sections; i++)
        {
            read(fd, name, 6);
            read(fd, type, 4);
            read(fd, offset, 4);
            read(fd, size, 4);
            int ty = (int)type[0];
            int sz = *((int *)size);
            printf("section%d: %s %d %d\n", i, name, ty, sz);
        }
        free(name);
        free(type);
        free(offset);
        free(size);
    }
}

void reverse(char *str)
{
    int n = strlen(str);
    for (int i = 0; i < n / 2; i++)
    {
        char aux;
        aux = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = aux;
    }
}

void extractSections(char *path, int sect, int line)
{
    int fd = open(path, O_RDONLY);
    struct stat st;
    if (lstat(path, &st) == -1)
        printf("ERROR\ninvalid file\n");
    lseek(fd, -3, SEEK_END);
    unsigned char header_size[2];
    read(fd, header_size, 2);
    header_size[2] = '\0';
    int hsize = 0;
    hsize += header_size[0];
    hsize += header_size[1] * 256;
    lseek(fd, -hsize + 1, SEEK_END);
    unsigned char no_of_sections;
    read(fd, &no_of_sections, 1);
    if (sect > no_of_sections)
        printf("ERROR\ninvalid section\n");
    for (int i = 1; i <= no_of_sections; i++)
    {
        char name[6];
        char type[4];
        char offset[4];
        char size[4];

        read(fd, name, 6);
        read(fd, type, 4);
        read(fd, offset, 4);
        read(fd, size, 4);

        int off_set = *((int *)offset);
        int sz = *((int *)size);

        if (i == sect)
        {
            char *buffer = (char *)malloc(sz + 1);
            lseek(fd, off_set, SEEK_SET);
            read(fd, buffer, sz);
            buffer[sz] = '\0';
            int line_no = 0;
            char *end = buffer + sz - 1;
            char *start = buffer + sz - 1;
            while (start >= buffer)
            {
                if (*start == '\n')
                {
                    line_no++;
                    if (line_no == line)
                    {
                        start++;
                        break;
                    }
                    end = start - 1;
                }
                start--;
            }

            if (start < buffer)
            {
                start = buffer;
            }

            int len = end - start + 1;
            
                char *line_str = (char *)malloc(len + 1);
                strncpy(line_str, start, len);
                reverse(line_str);
                line_str[len] = '\0';

                printf("SUCCESS\n");
                printf("%s\n", line_str);
                free(line_str);
            if (line_no > line)
            {
                printf("ERROR\ninvalid line\n");
            }
            free(buffer);
            close(fd);
            return;
        }
    }
}

int readParseFindALL(char *path)
{
    bool ok = true;
    int fd = open(path, O_RDONLY);
    lseek(fd, -3, SEEK_END);
    unsigned char header_size[2];
    read(fd, header_size, 2);
    header_size[2] = '\0';
    int hsize = 0;
    hsize += header_size[0];
    hsize += header_size[1] * 256;
    char magic;
    read(fd, &magic, 1);
    lseek(fd, -hsize, SEEK_END);
    unsigned char version;
    read(fd, &version, 1);
    unsigned char no_of_sections;
    read(fd, &no_of_sections, 1);
    for (int i = 0; i < no_of_sections; i++)
    {
        char name[6];
        char type[4];
        char offset[4];
        char size[4];

        read(fd, name, 6);
        read(fd, type, 4);
        read(fd, offset, 4);
        read(fd, size, 4);
        int ty = (int)type[0];
        if (ty != 15 && ty != 72 && ty != 17 && ty != 89)
        {
            ok = false;
            return -1;
        }
    }
    if (magic != 'r')
    {
        ok = false;
        return -1;
    }
    if (version < 71 || version > 180)
    {
        ok = false;
        return -1;
    }
    if (no_of_sections < 6 || no_of_sections > 19)
    {
        ok = false;
        return -1;
    }
    if (ok == true)
    {
        lseek(fd, -hsize + 2, SEEK_END);
        char *name = malloc(6);
        char *type = malloc(4);
        char *offset = malloc(4);
        char *size = malloc(4);
        for (int i = 1; i <= no_of_sections; i++)
        {
            read(fd, name, 6);
            read(fd, type, 4);
            read(fd, offset, 4);
            read(fd, size, 4);
            int sz = *((int *)size);
            if (sz > 1022)
            {
                free(name);
                free(type);
                free(offset);
                free(size);
                return -1;
            }
        }
        free(name);
        free(type);
        free(offset);
        free(size);
    }
    return 0;
}

void findAll(const char *currDir)
{
    DIR *dir = opendir(currDir);

    if (dir == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        char path[4096];
        struct stat command;

        snprintf(path, sizeof(path), "%s/%s", currDir, entry->d_name);

        if (lstat(path, &command) == -1)
        {
            perror("Failed to get file status");
            continue;
        }

        if (directoryExists(path))
        {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
            {
                continue;
            }
            findAll(path);
        }
        else
        {
            if (readParseFindALL(path) == 0)
                printf("%s\n", path);
        }
    }
    // free(entry);
    closedir(dir);
}

int main(int argc, char **argv)
{

    char *command;
    command = (char *)malloc(MAX_COMMAND + 1);

    strcpy(command, argv[1]);
    if (strcmp(command, "variant") == 0 && !checkInput(argv[1], command))
        printf("35455\n");
    if (strcmp(command, "list") == 0)
    {
        if (argc == 3)
        {
            if (directoryExists(argv[2] + 5))
            {
                printf("SUCCESS\n");
                char *directory;
                directory = (char *)malloc(DIRECTORY_SIZE + 1);
                strcpy(directory, argv[2] + 5);
                displayFiles(directory);
                free(directory);
            }
            else
                printf("ERROR\ninvalid directory path\n");
        }
        if (argc == 4)
        {
            if (strcmp(argv[2], "recursive") == 0)
            {
                if (directoryExists(argv[3] + 5))
                {
                    char *directory;
                    directory = (char *)malloc(DIRECTORY_SIZE + 1);
                    strcpy(directory, argv[3] + 5);
                    printf("SUCCESS\n");
                    displayFilesRecursively(directory);
                    free(directory);
                }
                else
                    printf("ERROR\ninvalid directory path\n");
            }
            if (strstr(argv[2], "size_smaller") != NULL)
            {
                char *val;
                val = (char *)malloc(VAL_SIZE + 1);
                long value = atol(strcpy(val, argv[2] + 13));
                if (directoryExists(argv[3] + 5))
                {
                    char *directory;
                    directory = (char *)malloc(DIRECTORY_SIZE + 1);
                    strcpy(directory, argv[3] + 5);
                    printf("SUCCESS\n");
                    sizeOfFiles(directory, value);
                    free(directory);
                }
                else
                    printf("ERROR\ninvalid directory path\n");
                free(val);
            }
            if (strstr(argv[2], "permissions") != NULL)
            {
                char *permission;
                permission = (char *)malloc(PERMISSION + 1);
                strcpy(permission, argv[2] + 12);
                if (directoryExists(argv[3] + 5))
                {
                    char *directory;
                    directory = (char *)malloc(DIRECTORY_SIZE + 1);
                    strcpy(directory, argv[3] + 5);
                    printf("SUCCESS\n");
                    permissionsList(directory, permission);
                    free(directory);
                }
                else
                    printf("ERROR\ninvalid directory path\n");
                free(permission);
            }
        }
        if (argc == 5)
        {
            if (strcmp(argv[1], "list") == 0)
            {
                if (strcmp(argv[2], "recursive") == 0)
                {
                    if (strstr(argv[3], "size_smaller") != NULL)
                    {
                        char *val;
                        val = (char *)malloc(VAL_SIZE + 1);
                        long value = atol(strcpy(val, argv[3] + 13));
                        if (directoryExists(argv[4] + 5))
                        {
                            char *directory;
                            directory = (char *)malloc(DIRECTORY_SIZE + 1);
                            strcpy(directory, argv[4] + 5);
                            printf("SUCCESS\n");
                            sizeofFilesRecursive(directory, value);
                            free(directory);
                        }
                        else
                            printf("ERROR\ninvalid directory path\n");
                        free(val);
                    }
                    if (strstr(argv[3], "permissions") != NULL)
                    {
                        char *permission = (char *)malloc(PERMISSION + 1);
                        strcpy(permission, argv[3] + 12);
                        if (directoryExists(argv[4] + 5))
                        {
                            char *directory = (char *)malloc(DIRECTORY_SIZE + 1);
                            strcpy(directory, argv[4] + 5);
                            printf("SUCCESS\n");
                            permissionsRecursive(directory, permission);
                            free(directory);
                        }
                        else
                            printf("ERROR\ninvalid directory path\n");
                        free(permission);
                    }
                }
                if (strcmp(argv[3], "recursive") == 0)
                {
                    if (strstr(argv[2], "size_smaller") != NULL)
                    {
                        char *val;
                        val = (char *)malloc(VAL_SIZE + 1);
                        long value = atol(strcpy(val, argv[2] + 13));
                        if (directoryExists(argv[4] + 5))
                        {
                            char *directory;
                            directory = (char *)malloc(DIRECTORY_SIZE + 1);
                            strcpy(directory, argv[4] + 5);
                            printf("SUCCESS\n");
                            sizeofFilesRecursive(directory, value);
                            free(directory);
                        }
                        else
                            printf("ERROR\ninvalid directory path\n");
                        free(val);
                    }
                    if (strstr(argv[2], "permissions") != NULL)
                    {
                        char *permission = (char *)malloc(PERMISSION + 1);
                        strcpy(permission, argv[2] + 12);
                        if (directoryExists(argv[4] + 5))
                        {
                            char *directory = (char *)malloc(DIRECTORY_SIZE + 1);
                            strcpy(directory, argv[4] + 5);
                            printf("SUCCESS\n");
                            permissionsRecursive(directory, permission);
                            free(directory);
                        }
                        else
                            printf("ERROR\ninvalid directory path\n");
                        free(permission);
                    }
                }
                if (strcmp(argv[4], "recursive") == 0)
                {
                    if (strstr(argv[2], "size_smaller") != NULL)
                    {
                        char *val;
                        val = (char *)malloc(VAL_SIZE + 1);
                        long value = atol(strcpy(val, argv[2] + 13));
                        if (directoryExists(argv[3] + 5))
                        {
                            char *directory;
                            directory = (char *)malloc(DIRECTORY_SIZE + 1);
                            strcpy(directory, argv[3] + 5);
                            printf("SUCCESS\n");
                            sizeofFilesRecursive(directory, value);
                            free(directory);
                        }
                        else
                            printf("ERROR\ninvalid directory path\n");
                        free(val);
                    }
                    if (strstr(argv[2], "permissions") != NULL)
                    {
                        char *permission = (char *)malloc(PERMISSION + 1);
                        strcpy(permission, argv[2] + 12);
                        if (directoryExists(argv[3] + 5))
                        {
                            char *directory = (char *)malloc(DIRECTORY_SIZE + 1);
                            strcpy(directory, argv[3] + 5);
                            printf("SUCCESS\n");
                            permissionsRecursive(directory, permission);
                            free(directory);
                        }
                        else
                            printf("ERROR\ninvalid directory path\n");
                        free(permission);
                    }
                }
            }
        }
    }
    if (strcmp(command, "parse") == 0)
    {
        if (fileExists(argv[2] + 5))
        {
            readParse(argv[2] + 5);
        }
    }
    if (strcmp(command, "findall") == 0)
    {
        if (directoryExists(argv[2] + 5))
        {
            printf("SUCCESS\n");
            findAll(argv[2] + 5);
        }
        else
            printf("ERROR\ninvalid directory path\n");
    }
    if (strcmp(command, "extract") == 0)
    {
        if (fileExists(argv[2] + 5))
        {
            extractSections(argv[2] + 5, atoi(argv[3] + 8), atoi(argv[4] + 5));
        }
        else
            printf("ERROR\ninvalid file\n");
    }
    free(command);
    return 0;
}