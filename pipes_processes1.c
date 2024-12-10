// C program to demonstrate use of fork() and 2-way pipe
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <string.h> 
#include <sys/wait.h> 

int main() 
{ 
    int fd1[2];  // First pipe for P1 -> P2 communication 
    int fd2[2];  // Second pipe for P2 -> P1 communication 
    char fixed_str1[] = "howard.edu"; 
    char fixed_str2[] = "gobison.org"; 
    char input_str[100]; 
    pid_t p; 

    if (pipe(fd1) == -1 || pipe(fd2) == -1) 
    { 
        fprintf(stderr, "Pipe Failed"); 
        return 1; 
    } 

    printf("Enter a string to concatenate: ");
    scanf("%s", input_str); 
    p = fork(); 

    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed"); 
        return 1; 
    } 

    // Parent process (P1)
    else if (p > 0) 
    { 
        close(fd1[0]);  // Close reading end of first pipe 
        close(fd2[1]);  // Close writing end of second pipe 

        // Write input string to P2
        write(fd1[1], input_str, strlen(input_str) + 1); 
        close(fd1[1]);  // Close writing end of first pipe 

        // Wait for P2 to send string back
        wait(NULL); 

        // Read the modified string from P2
        char concat_str2[100]; 
        read(fd2[0], concat_str2, 100); 
        close(fd2[0]); 

        // Concatenate "gobison.org"
        strcat(concat_str2, fixed_str2); 

        // Print the final concatenated string
        printf("Concatenated string: %s\n", concat_str2); 
    } 

    // Child process (P2)
    else
    { 
        close(fd1[1]);  // Close writing end of first pipe 
        close(fd2[0]);  // Close reading end of second pipe 

        // Read string from P1
        char concat_str1[100]; 
        read(fd1[0], concat_str1, 100); 
        close(fd1[0]);  // Close reading end of first pipe 

        // Concatenate "howard.edu"
        strcat(concat_str1, fixed_str1); 

        // Print concatenated string
        printf("Concatenated string: %s\n", concat_str1); 

        // Prompt for second input
        char input_str2[100]; 
        printf("Enter another string to concatenate: ");
        scanf("%s", input_str2); 

        // Send second string back to P1
        write(fd2[1], concat_str1, strlen(concat_str1) + 1); 
        close(fd2[1]);  // Close writing end of second pipe 

        exit(0); 
    } 
    return 0; 
}
