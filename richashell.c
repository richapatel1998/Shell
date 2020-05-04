#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define BUFFER_LEN 1024
#define MAXIMUM_LEN 10000
char* prompt;

int main(int argc, char  **argv){//keeps tracks of how many things are in the array, the other one is the length of the string in the array
        //the name of program, name of shell thing, and the -p there you did less than 3.
        prompt = malloc(sizeof(char) * BUFFER_LEN);
        if (argc < 3) {//check if this is in the correct order, less than 3 because you have 3 things automatically. There is 
                strcpy(prompt, "308sh> ");
        } else {//make sure the -p exiists and is the correct length
                if(argc == 3 && (strcmp("-p",argv[1])== 0)) {    // this is a string comparator to see if -p and the element is in first
                        //get what the user gave you as a prompt and print it to the console
                        strcpy(prompt, argv[2]);
                }

        }
        printf("%s", prompt);
        while(1) {
                char commandline[MAXIMUM_LEN];

                fgets(commandline, MAXIMUM_LEN, stdin);
                commandline[strlen(commandline) -1] = '\0';
                //first argument is cd after have rest of argu

                char* token = strtok(commandline, " ");

                //char* i[BUFFER_LEN] = null;
                int x = 0;
               //the zero element is the first token
                //second element might not be there
                //cd abc, abc is the element second one
                //x==1 is the number of token which is 1, 

                char* arr[500];
                while(token != NULL) {
                        arr[x] = malloc(sizeof(char)*strlen(token)); //allocate memory however many characters are there
                        strcpy(arr[x], token);
//printf("--%s--  ",arr[x]);
                        x++;
                        token = strtok(NULL, " ");
                
                }
                arr[x] = NULL;

                //comparing the two because to make sure they are the same
                if( (x == 1) && strcmp("exit", arr[0])==0) { //when you have 
                        exit(0); //exit and return 0
                }

                //the shell should print process id
                else if ( (x == 1) && strcmp("pid", arr[0])==0) {
                        printf("%ld\n",(long)getpid());
                }

                //the shell should print the process id of its parent
                else if ( (x == 1) && strcmp("ppid", arr[0])==0) {
                        printf("%ld\n",(long)getppid());
                }

                //print the current working directory
                else if ( (x == 1) && strcmp("pwd", arr[0])==0) {
                        char cwdname[500];
                        char *tmp;
                        tmp = getcwd(cwdname, 500);
                        if (tmp != NULL) {

                                printf("%s\n", cwdname);
                        }
                }
                //cd <dir> change the working directory. with no arguments, change the user's home directory
                else if (strcmp("cd", arr[0]) == 0) {
                        if (x == 1) {
                                // go to home directory
                                chdir(getenv("HOME"));
                        }
                        else {
                                // go to directory specified by arr[1]
                                chdir(arr[1]);
                        }
                }
                // add else if for setenv


                //sets an environment variable (which is visible in all future processes)
                //if there is only one argument, clears the variable
                //   set <var> <value>
                else if ( strcmp("set", arr[0])==0) {

                       if(x == 1) {
                            printf("set : Incomplete command\n");
                        } else if (x ==2){
                            unsetenv(arr[1]);
                        } else {
                            setenv(arr[1], arr[2], 1);
                        }
                }

                else if ( strcmp("get", arr[0])==0) {

                       if (x == 1) {
                            printf("get : Incomplete command\n");
                        } else if (x == 2) {
                            printf("%s=%s\n", arr[1], getenv(arr[1]));
                        } else {
                            printf("get : too many arguments\n");
                        }
                }                //get <var> prints the current value of the environment variable

                else {

                        pid_t waitid, childid;
                        int status;
                        int runInBackground = 0;

			if ( (x > 1) && strcmp("&", arr[x-1])==0) {
				//printf("in backbound..\n");
				arr[x-1] = NULL;
				runInBackground = 1;
			}                    
                        childid = fork(); //if fork is equal to 0 then you are in the child process
                        if (childid == 0) { //the one calling the fork is parent process
                                // we are in child process
                                //printf("testing...\n");
                                printf("[%ld] %s\n",(long)getpid(), arr[0]); 

                                if(execvp(arr[0], arr) < 0) {
                                        perror(prompt);
                                        exit(EXIT_FAILURE);	
                                }
                        } else if (childid < 0) {
                                // some error
                                perror(prompt);
                        } else {
                        	if (!runInBackground) {
	                        	int exited = 0;
	                        	int killed = 0;
	                                do{
	                                        waitid = waitpid(childid, &status, WUNTRACED);
	                                        exited = WIFEXITED(status);
	                                        killed = WIFSIGNALED(status);
	                                        //printf("%d---test\n", status);

	                                }while(!exited && !killed);
	                                if (exited) {
	                                	printf("[%ld] : Exited %d\n", (long)childid, status);
	                                }
	                                if (killed) {
	                                	printf("[%ld] : Killed %d\n", (long)childid, status);
	                                }
                        	} 
                        }
                }

                pid_t backgroundWaitPid = -1;
                int backgroundStatus;

               	do {
               		backgroundWaitPid = waitpid(-1, &backgroundStatus, WNOHANG);
               		if (backgroundWaitPid > 0) {
               			printf("[%ld] : Terminated %d\n", (long)backgroundWaitPid, backgroundStatus);
               		}
               	} while ( backgroundWaitPid > 0);


                //print the promt for next command
                printf("%s", prompt);
        }
    
}