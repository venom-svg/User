#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
using namespace std;

void show_menu() {
    cout << "\n1) Fork";
    cout << "\n2) Wait";
    cout << "\n3) Exec";
    cout << "\n4) cp (Copy file using system calls)";
    cout << "\n5) Grep (Search string in file using system calls)";
    cout << "\n6) Exit";
    cout << "\nEnter Choice: ";
}

void simulate_fork() {
    pid_t pid = fork();
    if (pid < 0) {
        cerr << "Fork failed!" << endl;
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        cout << "Child Process ID: " << getpid()
             << " | Parent ID: " << getppid() << endl;
        exit(EXIT_SUCCESS);
    } else {
        cout << "Parent Process ID: " << getpid()
             << " | Child ID: " << pid << endl;
    }
}

void simulate_wait() {
    pid_t pid = fork();
    if (pid < 0) {
        cerr << "Fork failed!" << endl;
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        cout << "Child Process ID: " << getpid()
             << " | Parent ID: " << getppid() << endl;
        sleep(2);  // Simulate some work
        exit(EXIT_SUCCESS);
    } else {
        cout << "Parent waiting for child..." << endl;
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            cout << "Parent Process ID: " << getpid()
                 << " | Child exited with status: " << WEXITSTATUS(status) << endl;
        }
    }
}

void simulate_exec() {
    pid_t pid = fork();
    if (pid < 0) {
        cerr << "Fork failed!" << endl;
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        cout << "Child executing `ls -l` command:" << endl;
        execlp("/bin/ls", "ls", "-l", NULL);
        // If exec fails:
        perror("execlp failed");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            cout << "Parent Process ID: " << getpid()
                 << " | Child finished `ls` with status: " << WEXITSTATUS(status) << endl;
        }
    }
}

void simulate_cp() {
    pid_t pid = fork();
    if (pid < 0) {
        cerr << "Fork failed!" << endl;
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process executes cp command
        cout << "Child executing `cp` command:" << endl;
        execlp("/bin/cp", "cp", "src.txt", "dest.txt", NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                cout << "File copied successfully from src.txt to dest.txt" << endl;
            } else {
                cout << "File copy failed!" << endl;
            }
        }
    }
}

void simulate_grep() {
    string searchStr;
    cout << "Enter the string to search: ";
    cin >> searchStr;

    pid_t pid = fork();
    if (pid < 0) {
        cerr << "Fork failed!" << endl;
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process executes grep command
        cout << "Child executing `grep` command:" << endl;
        execlp("/bin/grep", "grep", "-c", searchStr.c_str(), "src.txt", NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                cout << "Grep command executed successfully" << endl;
            } else if (WEXITSTATUS(status) == 1) {
                cout << "String not found in file" << endl;
            } else {
                cout << "Grep command failed!" << endl;
            }
        }
    }
}

int main() {
    int choice;
   
    while (true) {
        show_menu();
        cin >> choice;
       
        switch (choice) {
            case 1:
                simulate_fork();
                break;
            case 2:
                simulate_wait();
                break;
            case 3:
                simulate_exec();
                break;
            case 4:
                simulate_cp();
                break;
            case 5:
                simulate_grep();
                break;
            case 6:
                cout << "Exiting..." << endl;
                exit(EXIT_SUCCESS);
            default:
                cout << "Invalid choice. Try again." << endl;
        }
    }
   
    return 0;
}