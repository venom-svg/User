#include <iostream>
#include <fstream>
#include <unistd.h>
#include <assert.h>
#include <string>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
    int fd[2], pid;
    
    // Check if file exists or not
    std::ifstream file("file1.txt");
    if (!file)
    {
        std::cerr << "File not found." << std::endl;
        return 1;
    }
    
    // Redirect stdin to file1.txt
    int filedes = open("file1.txt", O_RDONLY);
    if (filedes == -1)
    {
        perror("Error opening file");
        return 1;
    }
    dup2(filedes, fileno(stdin));
    close(filedes);
    
    // Create pipe
    if (pipe(fd) == -1)
    {
        perror("error in creating pipe");
        return 1;
    }

    pid = fork();
    if (pid == 0)
    {
        // Child process: Redirect stdout to the pipe and run 'sort'
        dup2(fd[1], fileno(stdout));
        close(fd[0]);
        close(fd[1]);
        execl("/usr/bin/sort", "sort" , "-n", (char *) 0);
    }
    else if (pid > 0)
    {
        // Parent process: Redirect stdin to the pipe and run 'uniq'
        dup2(fd[0], fileno(stdin));
        close(fd[1]);
        close(fd[0]);
        execl("/usr/bin/uniq", "uniq", (char *) 0);
    }
    else
    {
        perror("error in fork command");
        return 1;
    }
    return 0;
}
