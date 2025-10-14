#include<stdio.h>
#include<unistd.h> //for using create system call / close / read / write
#include<fcntl.h>  //for using the flags 
#include<dirent.h> // DIR / entry
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<readline/readline.h>
#include<readline/history.h>
// #include<errno.h>

#define buf_size (4*1024*1024)

char buffer [buf_size];

int main(int argc, char *argv[])
{ 
    using_history();
    struct dirent *entry;
    char path[256];
    char again = 'Y';

    do{
    printf("Want to change DIR ??.... (Y/N)  ");  
    char change = getchar();
    if(change == 'Y' || change == 'y'){
        printf("Enter the directory you want to work in: ");
        scanf("%255s", path);

        // Change to the user-specified directory
        if (chdir(path) != 0) {
            perror("Failed to change directory!!");
            return 1;
        }else
            printf("Changed to directory: %s\n", path);
    }
    int choice;
    char filename[64] = {0};
    printf("what do you want to do \n");
    printf("#1. New file.\n");
    printf("##2. Open a file.\n");
    printf("###3. Append to a file.\n");
    printf("####4. Edit a file.\n");
    printf("#####5. Delete a file.\n");
    printf("\n####### Enter your choice : ");
    scanf("%d", &choice);
    while(getchar() != '\n');
    switch(choice){
        case 1:
        {
            DIR *dir;
            dir = opendir("."); // open current directory
            if(dir == NULL){
                perror("Can not open the folder !!\n");
                break;
            } 

            printf("\n############ Enter file name: ");
            scanf("%63s", filename);
            int exist = 0;
            while ((entry = readdir(dir)) != NULL) {
                if(strcmp(entry->d_name, filename) == 0){
                    printf("File already exists! \n");
                    exist = 1;
                    break;
                }
            }
            closedir(dir);//we should close the folder "directory" once we done with it following RAII principle.
           
            if(exist)
                break;

            int fd = creat(filename, 0644);
            if (fd == -1){
                perror("Error while creating the file: \n");
                break;
            }

            close(fd);
            // closedir(dir);
            printf("File %s created successfully :) \n", filename);
            break;
        }

        case 2:
        {
            printf("\n############ Enter file name : ");
            scanf("%63s", filename);
            int fd2 = open(filename , O_RDONLY);
            if (fd2 == -1){
                perror("error : ");
                break;
            }
            ssize_t n = read(fd2, buffer, buf_size);
            if(n<0){
                perror("error");
                close(fd2);
                break;
            }
            buffer[n] = '\0';
            printf("|%s:\n  %s ", filename, buffer);
            close(fd2);
            break;
        }
       
        case 3:
            printf("\n--> Enter file name : ");
            scanf("%63s", filename);
            while(getchar() != '\n');
            int fd3 = open(filename, O_WRONLY |O_CREAT |O_APPEND,0644);
            if (fd3 == -1){
                perror("ERROR, opening the file: ");
                break;
            }
            printf("\n ####### Enter content to be append : ");
            fgets(buffer, buf_size, stdin);
            write(fd3, buffer, strlen(buffer));
            close(fd3);
            printf("Content appended successfully :) !!\n");
            break;

        case 4:
        {
            char newcontent[buf_size] = {0};
            char *lines[256] = {NULL};  
            int line_count = 0;

            printf("\nEnter file name: ");
            scanf("%63s", filename);
            while (getchar() != '\n');

            int fd = open(filename, O_RDONLY);
            if (fd == -1) {
                perror("Could not open file for reading");
                break;
            }

            ssize_t bytes = read(fd, buffer, buf_size - 1);
            close(fd);

            if (bytes < 0) {
                perror("Error reading file");
                break;
            }
            buffer[bytes] = '\0';

            if (bytes == 0) {
                printf("File is empty. Starting fresh.\n");
            } else {
                char *copy = strdup(buffer); 
                char *line = strtok(copy, "\n");
                while (line != NULL && line_count < 256) {
                    lines[line_count] = strdup(line);
                    line_count++;
                    line = strtok(NULL, "\n");
                }
                free(copy);
            }

            printf("\n You can now edit the file line by line.\n");
            printf("Current content (%d lines):\n", line_count);
            for (int i = 0; i < line_count; i++) {
                printf("%d: %s\n", i + 1, lines[i]);
            }
            printf("(Type 'new' to add a line, 'done' to finish)\n\n");

            char *input;
            while (1) {
                input = readline("edit[line_num]> ");
                if (input == NULL) break;  

                if (strlen(input) == 0) {
                    free(input);
                    continue;
                }

                if (strcmp(input, "done") == 0) {
                    free(input);
                    break;
                }

                if (strcmp(input, "new") == 0) {
                    free(input);
                    char *new_line = readline("New line: ");
                    if (new_line) {
                        if (line_count < 256) {
                            lines[line_count] = strdup(new_line);
                            line_count++;
                        } else {
                            printf("Too many lines!\n");
                        }
                        free(new_line);
                    }
                    continue;
                }

                int num;
                if (sscanf(input, "%d", &num) == 1 && num >= 1 && num <= line_count) {
                    free(input);
                    char prompt[64];
                    snprintf(prompt, sizeof(prompt), "Edit line %d ('%s'): ", num, lines[num-1]);
                    char *edited = readline(prompt);
                    if (edited) {
                        if (strlen(edited) > 0) {
                            free(lines[num - 1]);
                            lines[num - 1] = strdup(edited);
                        }
                        free(edited);
                    }
                } else {
                    printf("Invalid input. Use: <line_num>, 'new', or 'done'\n");
                    free(input);
                }
            }

            newcontent[0] = '\0';
            for (int i = 0; i < line_count; i++) {
                strcat(newcontent, lines[i]);
                if (i < line_count - 1) strcat(newcontent, "\n");
            }
            if (line_count > 0 && newcontent[strlen(newcontent)-1] != '\n')
                strcat(newcontent, "\n");

            fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("Error saving file");
            } else {
                write(fd, newcontent, strlen(newcontent));
                printf(" File saved with %d lines.\n", line_count);
                close(fd);
            }

            for (int i = 0; i < line_count; i++) {
                free(lines[i]);
            }

            break;
        }
        case 5:
        {
            printf("\n############ Enter file name : ");
            scanf("%63s", filename);
            if (unlink(filename) == 0) {
                printf("File has been deleted successfully :) \n");
            } else {
                perror("Failed to delete the file :( !! \n");
            }
            break;
        default:
            printf("Invalid choice");
            break;
        }
    }
   printf("\n Want another operation (Y/N)  ");
   scanf(" %c", &again);
  }while(again == 'Y' || again == 'y');

    printf("\n\t\t\tI am sure you enjoy this, (if anyone had tried it already)");
    return 0; 
}
