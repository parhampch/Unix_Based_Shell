#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <regex.h>
#include <string.h> 
#include <stdlib.h>

#define MAX_LEN 513

int split(char* input, char splitBy, char** output){
    int lenght = strlen(input);
    lenght = input[lenght - 1] == '\n' ? lenght - 1 : lenght;
    int counter = 0;
    int index = 0;
    int subIndex = 0;
    output[counter] = malloc(MAX_LEN * sizeof(char));
    while (input[index] == splitBy) index++;

    while (index < lenght){
      if (input[index] != splitBy){
        output[counter][subIndex] = input[index];
        subIndex++;
        index++;
      }
      else{
        subIndex = 0;
        counter++;
        while(input[index] == splitBy) index++;
        if (index != lenght) output[counter] = malloc(MAX_LEN * sizeof(char));
      }
    }
    return input[lenght - 1] == splitBy ? counter : counter + 1;
}

int wantToQuit(char** commands, int numberOfCommands){
  char* quit = "quit";
  int ans = 0;
  for (int i = 0; i < numberOfCommands; i++){
    int index = 0;
    int forBreaked = 0;
    while (commands[i][index] == ' ') index++;
    for (int j = 0; j < 4; j++){
      if (commands[i][index] != quit[j]){
        forBreaked = 1;
        break;
      }
      index++;
    }
    if(!forBreaked) ans = 1;
  }
  return ans;
}

pid_t runCommand(char* command){
  char** args = (char **) malloc(MAX_LEN * sizeof(char*));
  char* location = (char *) malloc(MAX_LEN * sizeof(char));
  split(command, ' ', args);
  pid_t pid = -1;
  if (!strcmp(args[0], "quit") || !strcmp(args[0], "")) return pid;
  else if (!strcmp(args[0], "cd")) {
    int result = chdir(args[1]);
    if (result == -1) 
      printf("An error occured during running the command\n");
  }
  else if (!strcmp(args[0], "pwd")) 
    printf("%s\n", getcwd(location, MAX_LEN));
  else {
    pid_t pid = fork();
    if (pid == 0){
      int result = execvp(args[0], args);
      if (result == -1) 
        printf("An error occured during running the command\n");
    }
  }
  return pid;
}

void runShell(FILE* mode){
  int quit = 0;
  while (!quit){
    char* inputLine = (char *) malloc(MAX_LEN * sizeof(char));
    if (fgets(inputLine, MAX_LEN, mode) == NULL) break;
    if (inputLine[strlen(inputLine) - 1] != '\n'){
      printf("The input line is too long!\n");
      while (inputLine[strlen(inputLine) - 1] != '\n')
        fgets(inputLine, MAX_LEN, mode);
      continue;
    }

    char** commands= (char **) malloc(MAX_LEN * sizeof(char*));
    int numberOfCommands = split(inputLine, ';', commands);
    quit = wantToQuit(commands, numberOfCommands);
    pid_t* allPIDs = (pid_t*) malloc(numberOfCommands * sizeof(pid_t));
    int* status  = (int*) malloc(numberOfCommands * sizeof(int));
    for (int i = 0; i < numberOfCommands; i++){
        pid_t pid = runCommand(commands[i]);
        allPIDs[i] = pid;
    }
    for (int i = 0; i < numberOfCommands; i++)
      waitpid(allPIDs[i], &status[i], WUNTRACED | WCONTINUED);

    free(commands);
    free(allPIDs);
    free(status);
    free(inputLine);
  }
}
 
int main(int argc, char **argv) {
  if (argc == 1) runShell(stdin);
  else{
    FILE* commandFile = fopen(argv[1], "r");
    if (commandFile == NULL){
      printf("Can not open the bash file!\n");
      return 1;
    }
    runShell(commandFile);
  }
  return 0;
}
