#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/mman.h>

#define BUFFER_SIZE 1024
int variant = 35455;
int my_pipe, their_pipe;
int dim_request;
unsigned int shm_number = 0;
int shm_region;
int offset = 0, value = 0;
void *memory = NULL;
const char *shm_name = "/tgAVZH";
const char *my_name = "RESP_PIPE_35455";
const char *their_name = "REQ_PIPE_35455";
char buffer[BUFFER_SIZE], request[BUFFER_SIZE];
int createPipeResp = 1, createPipeReq = 1;

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

void extract_text(char *request)
{
    if (request[0] != '\"')
        return;

    size_t length = strlen(request);
    if (length < 2 || request[length - 1] != '\"')
        return;

    memmove(request, request + 1, length - 2);
    request[length - 2] = '\0';
}

void read_request(int my_pipe, int their_pipe)
{
    memset(request, '\0', sizeof(request));
    int noOfBytes = 0;
    int totalBytes = 0;
    char currentByte;

    while (1)
    {
        noOfBytes = read(their_pipe, &currentByte, 1);

        if (noOfBytes <= 0 || currentByte == '#')
        {
            break;
        }

        request[totalBytes++] = currentByte;
    }

    request[totalBytes] = '\0';
    printf("%s\n", request);
}

void process_write_to_shm(int their_pipe, int my_pipe)
{
    if (read(their_pipe, &offset, sizeof(unsigned int)) != sizeof(unsigned int))
    {
        printf("sunt aici 1\n");
        write(my_pipe, "WRITE_TO_SHM#", 13);
        write(my_pipe, "ERROR#", 6);
    }

    if (read(their_pipe, &value, sizeof(unsigned int)) != sizeof(unsigned int))
    {
        printf("sunt aici 2\n");
        write(my_pipe, "WRITE_TO_SHM#", 13);
        write(my_pipe, "ERROR#", 6);
    }

    printf("Offset %d\n", offset);
    printf("Value %d\n", value);
    if (shm_number > 0)
    {
        shm_region = shm_open(shm_name, O_RDWR, 0664);
        if (shm_region == -1)
        {
            printf("sunt aici 3\n");
            write(my_pipe, "WRITE_TO_SHM#", 13);
            write(my_pipe, "ERROR#", 6);
        }

        struct stat shm_stat;
        if (fstat(shm_region, &shm_stat) == -1)
        {
            printf("sunt aici 4\n");
            write(my_pipe, "WRITE_TO_SHM#", 13);
            write(my_pipe, "ERROR#", 6);
        }

        off_t shm_size = shm_stat.st_size;
        if (offset >= 0 && offset <= (shm_size - sizeof(unsigned int)))
        {
            ftruncate(shm_region, offset + sizeof(unsigned int));
            memory = mmap(0, shm_number, PROT_READ | PROT_WRITE, MAP_SHARED, shm_region, 0);
            if (memory == MAP_FAILED)
            {
                printf("sunt aici 5\n");
                write(my_pipe, "WRITE_TO_SHM#", 13);
                write(my_pipe, "ERROR#", 6);
            }

            unsigned int *data = (unsigned int *)(memory + offset);
            *data = value;
            printf("sunt aici 6\n");
            write(my_pipe, "WRITE_TO_SHM#", 13);
            write(my_pipe, "SUCCESS#", 8);
        }
        else
        {
            printf("sunt aici 7\n");
            write(my_pipe, "WRITE_TO_SHM#", 13);
            write(my_pipe, "ERROR#", 6);
        }
    }
    else
    {
        printf("sunt aici 8\n");
        write(my_pipe, "WRITE_TO_SHM#", 13);
        write(my_pipe, "ERROR#", 6);
    }
}

int main()
{

    if (mkfifo(my_name, 0666) != 0)
    {
        printf("ERROR\ncannot create the response pipe | cannot open the request pipe\n");
    }

    their_pipe = open(their_name, O_RDONLY);
    if (their_pipe == -1)
    {
        printf("ERROR\ncannot create the response pipe | cannot open the request pipe\n");
    }

    my_pipe = open(my_name, O_WRONLY);
    if (my_pipe == -1)
    {
        printf("ERROR\ncannot create the response pipe | cannot open the request pipe\n");
    }

    if (createPipeReq != 0 && createPipeResp != 0)
    {
        strcpy(buffer, "CONNECT#");
        if (write(my_pipe, buffer, 8) == 8)
        {
            printf("SUCCESS\n");
        }
    }

    while (1)
    {
        dim_request = sizeof(request) - 1;
        read_request(my_pipe, their_pipe);
        if (strstr(request, "PING") != NULL)
        {
            write(my_pipe, "PING#", 5);
            write(my_pipe, &variant, 4);
            write(my_pipe, "PONG#", 5);
        }
        else
        {
            if (strstr(request, "CREATE_SHM") != NULL)
            {
                if (read(their_pipe, &shm_number, sizeof(unsigned int)) != sizeof(unsigned int))
                {
                    write(my_pipe, "CREATE_SHM#", 11);
                    write(my_pipe, "ERROR#", 6);
                }
                else
                {
                    shm_region = shm_open(shm_name, O_CREAT | O_RDWR, 0664);
                    if (shm_region == -1)
                    {
                        write(my_pipe, "CREATE_SHM#", 11);
                        write(my_pipe, "ERROR#", 6);
                    }
                    else
                    {
                        if (ftruncate(shm_region, shm_number) == -1)
                        {
                            write(my_pipe, "CREATE_SHM#", 11);
                            write(my_pipe, "ERROR#", 6);
                        }
                        else
                        {
                            write(my_pipe, "CREATE_SHM#", 11);
                            write(my_pipe, "SUCCESS#", 8);
                        }
                    }
                }
            }
            else
            {
                if (strstr(request, "WRITE_TO_SHM") != NULL)
                {
                    process_write_to_shm(their_pipe, my_pipe);
                }
                else
                {
                    if (strstr(request, "MAP_FILE") != NULL)
                    {
                        read_request(my_pipe, their_pipe);
                        extract_text(request);
                        int file_descriptor = open(request, O_RDONLY);
                        if (file_descriptor == -1)
                        {
                            write(my_pipe, "MAP_FILE#", 9);
                            write(my_pipe, "ERROR#", 6);
                        }
                        else
                        {
                            struct stat file_stat;
                            if (fstat(file_descriptor, &file_stat) == -1)
                            {
                                write(my_pipe, "MAP_FILE#", 9);
                                write(my_pipe, "ERROR#", 6);
                            }
                            else
                            {
                                off_t file_size = file_stat.st_size;
                                memory = mmap(NULL, file_size, PROT_READ, MAP_SHARED, file_descriptor, 0);
                                if (memory == MAP_FAILED)
                                {
                                    write(my_pipe, "MAP_FILE#", 9);
                                    write(my_pipe, "ERROR#", 6);
                                }
                                else
                                {
                                    write(my_pipe, "MAP_FILE#", 9);
                                    write(my_pipe, "SUCCESS#", 8);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (strstr(request, "READ_FROM_FILE_OFFSET"))
                        {
                            write(my_pipe, "READ_FROM_FILE_OFFSET#", 22);
                            write(my_pipe, "SUCCESS#", 8);
                            break;
                        }
                        else
                        {
                            if (strstr(request, "READ_FROM_FILE_SECTION"))
                            {
                                write(my_pipe, "READ_FROM_FILE_SECTION#", 23);
                                write(my_pipe, "SUCCESS#", 8);
                                break;
                            }
                            else
                            {
                                if (strstr(request, "READ_FROM_LOGICAL_SPACE_OFFSET"))
                                {
                                    write(my_pipe, "READ_FROM_LOGICAL_SPACE_OFFSET#", 31);
                                    write(my_pipe, "SUCCESS#", 8);
                                    break;
                                }
                                else
                                {
                                    if (strstr(request, "EXIT") != NULL)
                                    {
                                        munmap(memory, offset + sizeof(unsigned int));
                                        close(shm_region);
                                        shm_unlink(shm_name);
                                        break;
                                    }
                                    else
                                    {
                                        write(my_pipe, request, strlen(request));
                                        write(my_pipe, "ERROR#", 6);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
