#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include <sys/stat.h>
#define MAX_ARGS 64
const char *mypath[4] = {
"./",
"/usr/bin/",
"/bin/",
NULL
};
#define MAX_ARG_LEN 16

#define MAXCOM 1000 // max number of letters to be supported 
#define MAXLIST 100 // max number of commands to be supported 

#define clear() printf("\033[H\033[J") 
void init_shell() 
{ 
    clear(); 
    printf("\n\n\n\n******************"
        "************************"); 
    printf("\n\n\n\t****MY SHELL****"); 
    printf("\n\n\t-USE AT YOUR OWN RISK-"); 
    printf("\n\n\n\n*******************"
        "***********************"); 
    char cont;
    printf("\n Enter y to continue or n to exit: ");
    scanf ("%c", &cont);
    if(cont=='n'){
    exit(0);
    }
    //printf("\nYour Answer is:  %c", cont); 
    //printf("\n"); 
    sleep(1); 
    clear(); 
} 

void printPath_prompt() 
{ 
    char cwd[1024]; 
    getcwd(cwd, sizeof(cwd)); 
    printf("\n %s: ", cwd); 
} 
void readCommand(char buffer[])
{
char temp;
	scanf("%c",&temp);
  scanf("%[^\n]",buffer);
}

void parseCmd(char * cmd,char *processed[MAX_ARG_LEN]) 
{ 
    int i; 
     
    for (i = 0; i < MAX_ARGS; i++) { 
        processed[i]=strsep(&cmd,"\t"); 
  		 
        if (processed[i]==NULL) 
            break; 
        if (strlen(processed[i]) == 0) 
            i--; 
    } 
} 
int checkIfOwnCmd(char *processed[MAX_ARG_LEN])
{
	int no_cmds = 2; 
    char *ownList[no_cmds]; 
    int flag=0;
  
    ownList[0] = "exit"; 
    ownList[1] = "cd"; 
    if(strcmp(processed[0],ownList[0])==0){
     printf("\nYou are going to exit Bye.\n"); 
        exit(0); 
    }
  	else if(strcmp(processed[0],ownList[1])==0){
     
     if(processed[1]==NULL){
     	 printf("\n Please enter the directory you want to go..\n");
     } 
     printf("\nYou are going to change the directory\n");
       chdir(processed[1]); 
      // printPath_prompt();
       return 1; 
    }
    
    return 0;


}
void executeNormal(char** processed) 
{ 
struct stat stats;
int flag=0;
	char* pathEnvVar = getenv("PATH");
	printf("Unable %s\n",pathEnvVar);
	char temp[sizeof(mypath[2])];
	strcpy(temp,mypath[2]);
	printf("Unable %s\n",temp);
	if(memchr(processed[0],'/',sizeof(processed[0]))==NULL){
	
	strcat(temp,processed[0]);
	strcpy(processed[0],temp);
	printf("Unable %s\n",processed[0]);
	if (stat(temp, &stats) == 0)
    {
        flag=1;
    }
    else
    {
        printf("Unable to get file properties.\n");
        printf("Please check whether  file exists.\n");
    }
	
	}
	
    pid_t pid = fork();  
  
    if (pid == -1) { 
        perror("\nFailed forking "); 
        return; 
    } else if (pid == 0) { 
    //child
    	if(flag==1){
    	if (execvp(processed[0], processed) < 0) { 
            printf("\nCould not execute command11.."); 
        } 
        exit(0);
    	}
        else if (execvp(processed[0], processed) < 0) { 
            printf("\nCould not execute command.."); 
        } 
        exit(0); 
    } else { 
        // waiting for child to terminate 
        wait(NULL);  
        return; 
    } 
} 
int parsePipe(char* str, char**strpiped) 
{ 
     int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, "|"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL) 
        return 0; // returns zero if no pipe is found. 
    else { 
        return 1; 
    } 
  
    
} 
void execArgsPiped(char** parsed, char** parsedpipe) 
{ 
    // 0 is read end, 1 is write end 
    int pipefd[2];  
    pid_t p1, p2; 
  
    if (pipe(pipefd) < 0) { 
        printf("\nPipe could not be initialized"); 
        return; 
    } 
    p1 = fork(); 
    if (p1 < 0) { 
        printf("\nCould not fork"); 
        return; 
    } 
  
    if (p1 == 0) { 
        // Child 1 executing.. 
        // It only needs to write at the write end 
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]); 
  
        if (execvp(parsed[0], parsed) < 0) { 
            printf("\nCould not execute command 1.."); 
            exit(0); 
        } 
    } else { 
        // Parent executing 
        p2 = fork(); 
  
        if (p2 < 0) { 
            printf("\nCould not fork"); 
            return; 
        } 
  
        // Child 2 executing.. 
        // It only needs to read at the read end 
        if (p2 == 0) { 
            close(pipefd[1]); 
            dup2(pipefd[0], STDIN_FILENO); 
            close(pipefd[0]); 
            if (execvp(parsedpipe[0], parsedpipe) < 0) { 
                printf("\nCould not execute command 2.."); 
                exit(0); 
            } 
        } else { 
            // parent executing, waiting for two children 
            wait(NULL); 
            wait(NULL); 
        } 
    } 
} 

int main(){
	 init_shell(); 
	char* strpiped[2];
	 char* parsedArgsPiped[MAX_ARG_LEN]; 

	 char cmd[2000];
	 int check;
	 //char *found;
	  int piped=0;
	 char *processed[MAX_ARG_LEN];
	 while(1){
	  printPath_prompt();
	 readCommand(cmd); 		   	
	 int i=0;
	 piped = parsePipe(cmd, strpiped); 
	 if (piped) { 
        parseCmd(strpiped[1], processed); 
        parseCmd(strpiped[2], parsedArgsPiped); 
  
    } else { 
  
         parseCmd(cmd,processed); 
    } 
	
	    //printf("\n %s\t%s\t%s\t the string issss ", processed[0],processed[1],processed[2]); 
	    
	  check=checkIfOwnCmd(processed);
	  if(check==0 && piped==0){
	  	executeNormal(processed);
	  }
	  if(piped){
	   execArgsPiped(processed, parsedArgsPiped);
	  }
	 
	  
	 }
	return 0;
}
