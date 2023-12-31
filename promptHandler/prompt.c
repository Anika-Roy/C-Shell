#include "../headers.h"
#include "prompt.h"

/*
Specification1
On every line when waiting for user input, a shell prompt of the following form must appear along with it.
With proper error handling.
Format: <Username@SystemName:~>
Example: <JohnDoe@SYS:/home/johndoe/sem3>
Note: The directory from which shell is invoked becomes the home directory for the shell and 
represented with “~”. All paths inside this directory should be shown relative to it. 
Absolute path of a directory/file must be shown when outside the home directory.

*/
int tokeniser(struct TokenWithDelimiter tokens[], char input[]) {
    // After a lot of trial and error,help from ChatGPT, discussed with Ujjwal Shekhar(2021113009)
 
    // Tokenize with all whitespaces (space and tab) to get command and arguments

    char input_copy[4096];
    strcpy(input_copy, input);
    // printf("input string to tokeniser: %s, length of string: %ld\n", input_copy, strlen(input_copy));

    // tokenise input copy with ; and & to get commands
    char *command = strtok(input_copy, ";&");
    int command_count = 0;
    while (command != NULL) {
        // printf("%s\n", command);
        strcpy(tokens[command_count].token, command);
        command_count++;
        command = strtok(NULL, ";&");
    }

    // printf("before check command count: %d\n", command_count);
    // printf("last token: %s\n",tokens[command_count - 1].token);
    // check if the last token has only whitespaces
    // if yes, then decrement command_count
    int last_token_length = strlen(tokens[command_count - 1].token);
    // printf("last token char=%d\n",tokens[command_count - 1].token[0]);
    int last_token_is_whitespace = 1;
    for (int i = 0; i < last_token_length; i++) {
        if (!isspace(tokens[command_count - 1].token[i])) {
            last_token_is_whitespace = 0;
            break;
        }
    }
    if (last_token_is_whitespace) {
        command_count--;
    }

    // printf("after check command count: %d\n", command_count);

    // now using the original input string, get the corresponding delimiter in tokens[i].delimiter
    // compare the tokens and skip whitespaces to find the delimiter
    // if the token is the last token, then the delimiter is ;
    int i = 0;
    int j = 0;
    tokens[command_count - 1].delimiter = ';';
    while (input[i] != '\0' && j < command_count) {
        if (input[i] == ' ' || input[i] == '\t') {
            i++;
            continue;
        }

        if (input[i] == ';' || input[i] == '&') {
            tokens[j].delimiter = input[i];
            j++;
        }

        i++;
    }

    //print the tokens and their delimiters
    // printf("Tokens in tokeniser:\n");
    // for (int i = 0; i < command_count; i++) {
    //     printf("token: %s\n", tokens[i].token);
    //     printf("delimiter:%c\n", tokens[i].delimiter);
    // }

    return command_count;
}

int handle_pastevents_execute_and_tokenise(char* input,char events[][MAX_EVENT_LENGTH],int event_count,struct TokenWithDelimiter tokens[],char original_command[]){
    
    // Handling PASTEVENTS EXECUTE: declare array of structs to store tokens and their delimiters 
    struct TokenWithDelimiter tokens_pastevents[MAX_TOKENS];
    char temp_input[4096];
    strcpy(temp_input,input);

    int i=tokeniser(tokens_pastevents,temp_input);

    // print tokens in tokens_pastevents
    // printf("tokens in tokens_pastevents:\n");
    // for(int j=0;j<i;j++){
    //     printf("token: %s\n",tokens_pastevents[j].token);
    //     printf("delimiter: %c\n",tokens_pastevents[j].delimiter);
    // }

    // remove the newline character from the last token if present
    if(tokens_pastevents[i-1].token[strlen(tokens_pastevents[i-1].token)-1]=='\n')
        tokens_pastevents[i-1].token[strlen(tokens_pastevents[i-1].token)-1]='\0';

    // handle pastevents execute command by iterating through the tokens
    for(int j=0;j<i;j++){
        // copy it to another string
        char temp[MAX_EVENT_LENGTH];
        strcpy(temp,tokens_pastevents[j].token);

        // tokenise it with all whitespaces to get the command and arguments
        char *args[MAX_ARGS];
        int arg_count = 0;

        args[arg_count] = strtok(temp, " \t\n");
        while (args[arg_count] != NULL && arg_count < MAX_ARGS) {
            arg_count++;
            args[arg_count] = strtok(NULL, " \t\n");
        }

        // if the command is pastevents and the second argument is execute, then replace the command with the command at index <index>
        if(strcmp(args[0],"pastevents")==0 && arg_count>1 && strcmp(args[1],"execute")==0){
            // args[2]="<index>"
            int index=atoi(args[2]);
            if(index>event_count){
                printf("Index out of bounds\n");
                continue;
            }
            //handle delimiters while copying
            // set the delimiter as the last letter of events[index]-1
            // remove the last letter from tokens_pastevents[j].token
            tokens_pastevents[j].delimiter = events[index-1][strlen(events[index-1])-1];
            strcpy(tokens_pastevents[j].token,events[index-1]);
            tokens_pastevents[j].token[strlen(tokens_pastevents[j].token)-1]='\0';

            // if(events[index-1])
            // strcpy(tokens_pastevents[j].token,events[index-1]); //[TODO: Handle delimiter clashes]

        }

        // else{
        //     // concatenate the tokenised arguemnets to get the minimal token
        //     char minimal_token[MAX_EVENT_LENGTH];
        //     minimal_token[0]='\0';
        //     for(int k=0;k<arg_count;k++){
        //         strcat(minimal_token,args[k]);
        //         if(k<arg_count-1)
        //             strcat(minimal_token," ");
        //     }
        //     // printf("minimal token: %s\n",minimal_token);

        //     // copy the minimal token to the tokenised token
        //     strcpy(tokens_pastevents[j].token,minimal_token);
        //     // printf("token: %s\n",tokens_pastevents[j].token);
        //     // printf("delimiter: %c\n",tokens_pastevents[j].delimiter);
        // }
    }

    // Concatenate all tokens(and their arguments) to get the original command(along with delimiter)
    
    for(int j=0;j<i;j++){
        char delimiter = tokens_pastevents[j].delimiter;

        strcat(original_command, tokens_pastevents[j].token);
        // printf("tokens length: %ld\n",strlen( tokens_pastevents[j].token));
        // if(j<i-1)
        //     strcat(original_command, &delimiter); [TODO: I had added this to debug, but forgot why, not its causing problems with sending things to background]
        char tmpstr[2];
        tmpstr[0] = delimiter;
        tmpstr[1] = 0;
        strcat(original_command, tmpstr);
    }

    // // print the original command
    // printf("original command: %s, length of original command: %ld\n",original_command,strlen(original_command));

    // printf("prompt handler: %d\n",i);
    i=tokeniser(tokens,original_command);

    // print the tokens and their delimiters
    // for (int j = 0; j < i; j++) {
    //     printf("token: %s\n", tokens[j].token);
    //     printf("delimiter:%c\n", tokens[j].delimiter);
    // }
    // printf("prompt handler: %d\n",i);
    return i;
}

void prompt(char *store_calling_directory) {
    // Reference used: ChatGPT

    char hostname[1024];
    char cwd[1024];

    // Get the username (automatically imported by Copilot)
    struct passwd *p = getpwuid(getuid());
    char *username = p->pw_name;

    
    // Get the system name (hostname)
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        exit(EXIT_FAILURE);
    }

    // Get the current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    // if (strstr(cwd, store_calling_directory) != NULL) {
    //         char *relative_path = strstr(cwd, store_calling_directory);
    //         // printf("%s\n", relative_path);
    //         printf("<%s@%s:~%s> ", username, hostname, relative_path + strlen(store_calling_directory));
    //     }
    //     else{
    //         printf("<%s@%s:%s> ", username, hostname, cwd);
    //     }
    if (strstr(cwd, store_calling_directory) != NULL) {
        char *relative_path = strstr(cwd, store_calling_directory);

        // Customize the colors for different parts of the prompt
        printf("\x1B[1;36m<\x1B[0m"); // Green color for username and hostname
        printf("\x1B[1;34m%s\x1B[0m", username); // Blue color for username
        printf("\x1B[1;36m@\x1B[0m");
        printf("\x1B[1;35m%s\x1B[0m", hostname); // Magenta color for hostname
        printf("\x1B[1;36m:~%s>\x1B[0m ", relative_path + strlen(store_calling_directory)); // Green color for the path
    } else {
        printf("\x1B[1;36m<\x1B[0m");
        printf("\x1B[1;34m%s\x1B[0m", username);
        printf("\x1B[1;36m@\x1B[0m");
        printf("\x1B[1;35m%s\x1B[0m", hostname);
        printf("\x1B[1;36m:%s>\x1B[0m ", cwd);
    }

}
    

