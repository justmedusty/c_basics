//
// Created by dustyn on 3/23/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define PATH_MAX 150

int main()
{
    //file descriptor
    int fd;

    //buffer of 1kib
    char buffer[1024];

    //using ssize_t since it returns a -1 on error which makes for easy handling
    ssize_t bytes_read;

    //path to home dir
    char home_path[PATH_MAX];

    //path to file
    char file_path[PATH_MAX];

    //Instantiting a pointer to the memory address storing the result of get home (your home dir)
    const char* home_env = getenv("HOME");

    //Make sure this succeeded
    if (home_env == NULL)
    {
        fprintf(stderr, "Could not get home environment");
        return EXIT_FAILURE;
    }

    //Copy the contents from the memory address pointed to by home_env pointer, to home path
    strncpy(home_path, home_env,PATH_MAX - 1);

    //put a null terminator at the end to signify this is the end, and not read further
    home_path[PATH_MAX - 1] = '\0';

    //safe print string which will appened our example.txt file to the end of the home path
    snprintf(file_path,PATH_MAX, "%s/example.txt", home_path);

    //Open the file with the path specified, with Write only flags
    fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Error writing file");
        return EXIT_FAILURE;
    }

    //write to the file
    write(fd, "This is a test\n", 15);
    write(fd, "12345\n", 6);

    //close the file
    close(fd);

    //open the file with read only flags
    fd = open(file_path,O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening file to read");
        return EXIT_FAILURE;
    }
    //because bytes_read is type ssize_t, we can tell if there was an error but checking for -1
    bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == -1)
    {
        perror("Error reading from from file");
        return EXIT_FAILURE;
    }

    write(STDOUT_FILENO, buffer, bytes_read);
    close(fd);

    return EXIT_SUCCESS;
}
