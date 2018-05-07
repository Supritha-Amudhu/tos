/*    
Student Name: Supritha Amudhu
Student ID: 917946619
Student Email: samudhu@mail.sfsu.edu
*/


#include <kernel.h>

#define COMMAND_MAN 0
#define COMMAND_CLEAR 1
#define COMMAND_SHELL 2
#define COMMAND_PONG 3
#define COMMAND_ECHO 4
#define COMMAND_PS 5
#define COMMAND_TOP 6
#define COMMAND_ABOUT 7

#define TOTAL_SHELL_COMMANDS 8
#define MAX_COMMAND_LENGTH 500

#define EMPTY_SPACE_STRING " "
#define EMPTY_SPACE_CHAR ' '
#define TERMINAL_SYMBOL ">>"
#define NEXT_LINE 13
#define TAB_SPACE 9
#define BACKSPACE 8


int window_id_counter = 0;

struct command_components{
	char* command;
	char* parameter;
};

static char* shell_commands[8][3] = {
							{"man", "Print a short description for a command                          or list all commands", "Print a short description for a command or list all commands"},
							{"clear", "Clear the current window", "Clear the current window"},
							{"shell", "Launch a new Shell", "Launch a new Shell"},
							{"pong", "Launch a Ping Pong game", "Launch a Ping Pong game"},
							{"echo", "Print a message on the Screen", "Print a message on the Screen"},
							{"ps", "Print a list of TOS processes", "Print a list of TOS processes"},
							{"top", "Print the Process table every second unt                         il interrupted", "Print the Process table every second until interrupted"},
							{"about", "Print your name", "Print your name"}
						};

int* window_id_array[MAX_PROCS];

/* This method will list all available commands in Shell by providing the window ID and the content to be displayed. */
void view_available_commands(int window_id){
	wm_print(window_id, "1. man <command> -       %s\n", shell_commands[0][1]);
	wm_print(window_id, "2. clear         -       %s\n", shell_commands[1][1]);
	wm_print(window_id, "3. shell         -       %s\n", shell_commands[2][1]);
	wm_print(window_id, "4. pong          -       %s\n", shell_commands[3][1]);
	wm_print(window_id, "5. echo          -       %s\n", shell_commands[4][1]);
	wm_print(window_id, "6. ps <-d>       -       %s\n", shell_commands[5][1]);
	wm_print(window_id, "7. top           -       %s\n", shell_commands[6][1]);
	wm_print(window_id, "8. about         -       %s\n", shell_commands[7][1]);
	wm_print(window_id, "\nPlease choose an option:\n\n");
}


/*  List of all helper processes */

struct command_components trim_white_spaces(int window_id, char* input, int input_length){
	char* output_command = malloc(MAX_COMMAND_LENGTH);
	char* command_parameter = malloc(MAX_COMMAND_LENGTH);
	int output_command_index = 0;
	int command_parameter_index = 0;
	int index = 0;
	int white_space_count = 0;
	

	/* Trim white space between words, find two words in the same command */
	for(int index = 0; index < input_length; index++){
		/* When output_command_index is not 0, the command parameter string has started, return the remaining string as parameter */
		if(input[index] != EMPTY_SPACE_CHAR && input[index-1] == EMPTY_SPACE_CHAR && output_command_index != 0){
			int inner_loop_index = index;
			while(input[inner_loop_index] != '\0'){
				command_parameter[command_parameter_index] = input[inner_loop_index];
				inner_loop_index++;
				command_parameter_index++;
			}		
			break;
		}
		/* Checks if the current character is a white space, if so, skips it. */
		else if(input[index] == EMPTY_SPACE_CHAR){
			white_space_count++;
			continue;
		}
		/* If neither of these cases, add the character to output_command. */
		else{
			output_command[output_command_index] = input[index];
			output_command_index++;
		}	
		/* Checks if the command contains only white spaces. */
		if(white_space_count >= input_length){
			struct command_components command_final = {"None", "None"};
			return command_final;
		}	
	}
	/* Assign command parameter as None if there are no parameters  */
	if(command_parameter_index == 0){
		command_parameter = "None";
	}
	struct command_components command_final = {output_command, command_parameter};
	return command_final; 
}

/* Trim trailing white spaces in a string. If the string has two words, return the input string. */
char* trim_trailing_white_spaces(char* input){
	int index = 0;
	int string_index = 0;
	char* trimmed_string = malloc(MAX_COMMAND_LENGTH);

	/* If there are two words detected, then return the entire input string as parameter. */
	while(input[index] != '\0'){
		if(input[index] != EMPTY_SPACE_CHAR && input[index-1] == EMPTY_SPACE_CHAR){
			return input;
		}
		/* Skip if it an empty space - Here it is trailing space. */
		if(input[index] == EMPTY_SPACE_CHAR){	
			index++;
			continue;
		}
		trimmed_string[string_index] = input[index];
		index++;
		string_index++;
	}
	return trimmed_string;
}

/* Compare two strings and return if they are equal or not. */
int str_compare(char* first_string, char* second_string){
	int index = 0;
	while(first_string[index] == second_string[index] && first_string[index] != '\0'){
		index++;
	}
	if(first_string[index] != second_string[index]){
		return 0;
	}
	else{
		return 1;
	}
}


/* Find the value for the typed command from the shell_commands array. */
int find_command(int window_id, char* command, int command_length){
	struct command_components command_final = trim_white_spaces(window_id, command, command_length);
	for(int index = 0; index < TOTAL_SHELL_COMMANDS; index++){
		char* command_from_list = shell_commands[index][0];
		if(str_compare(command_final.command, command_from_list) == 1){
			execute_shell_commands(window_id, index, command_final.parameter);
			return 0;
		}
	}
	execute_shell_commands(window_id, 55);
}


/* Remove single and double quotes from echo command's parameter. */
char* remove_quotes_echo(char* input){
	int index = 0;
	char* echo_output = malloc(MAX_COMMAND_LENGTH);
	int echo_output_index = 0;
	while(input[index] != '\0'){
		if(input[index] == '"' || input[index] == '\''){
			index++;
			continue;
		}
		echo_output[echo_output_index] = input[index];
		index++;
		echo_output_index++;
	}
	return echo_output;
}


/* Prints headings for the process table, for both ps and ps -d */
void shell_print_process_headings(int window_id, int print_all_details){
	if(print_all_details){
		wm_print(window_id, "State              Active  Priority       Name\n");
		wm_print(window_id, "-----------------------------------------------------\n");
	}
	else{
		wm_print(window_id, "Name\n");
		wm_print(window_id, "----\n");
	}
}


/* Prints process table details */
void shell_print_process_details(int window_id, PROCESS process, int print_all_details){
	static const char* process_states[] = 
						{
							"READY           ",
							"ZOMBIE          ",
							"SEND_BLOCKED    ",
							"REPLY_BLOCKED   ",
							"RECEIVE_BLOCKED ",
							"MESSAGE_BLOCKED ",
							"INTR_BLOCKED    "
						};
	if(!process->used){
		wm_print(window_id, "PCB slot unused.");
		return;
	}
	if(print_all_details){
		wm_print(window_id, "%s  ", process_states[process->state]);
		/* Check for active proc, if active, print *, if not print an empty space and the rest of the details. */
		if(process == active_proc){
			wm_print(window_id, "   *         ");
		}
		else{
			wm_print(window_id, "             ");
		}
		wm_print(window_id, "   %d    ",process->priority);
		wm_print(window_id, "   %s    \n",process->name);
	}
	else{
		wm_print(window_id, "%s\n",process->name);
	}
}


/* Main shell commands list */

/* Prints a description of the command that is given as a parameter. If no command is given,
	an entire list of available commands is displayed */
void call_man(int window_id, char* command_parameter){
	char* command_name = '\0';
	int command_match_found = 0;
	/* Checks if there is no command parameter. */
	if(command_parameter != "None"){
		for(int i = 0; i < TOTAL_SHELL_COMMANDS; i++){
			if(str_compare(trim_trailing_white_spaces(command_parameter), shell_commands[i][0])){
				wm_print(window_id, "%s\n", shell_commands[i][2]);
				command_match_found = 1;
			}
		}
		if(!command_match_found){
			wm_print(window_id, "Sorry, command not found. Please try another command.\n");
			return;
		}
	}
	else{
		view_available_commands(window_id);
	}
}

/* Clears the screen. Invokes the clear_wm() method which clears the window contents when 
	a window ID is specified. */
void clear_screen(int window_id){
	wm_clear(window_id);
}


/* Creates a new shell process */
void create_new_shell(){
	start_shell();
}


/* Invoke the method start_pong() which will start the Ping Pong game. */
void launch_pong(){
	start_pong();
}


/* Prints the text in the parameter onto the current window. If no text, method prints an empty line. */
void print_echo_message(int window_id, char* message){
	if(str_compare(message, "None")){
		wm_print(window_id, "\n");
	}
	else{
		wm_print(window_id, "%s\n", remove_quotes_echo(message));
	}
}


/* Lists names of all the processes in the process table using the command ps. If used with ps -d, all processes' 
	details are printed as well. */
void list_processes(int window_id, char* command_parameter){
	PCB *process = pcb;
	int print_all_details = 0;

	/* Checks if the command parameter is -d */
	if(str_compare(trim_trailing_white_spaces(command_parameter), "-d")){
		print_all_details = 1;
	}
	else if(str_compare(command_parameter, "None")){
		print_all_details = 0;
	}
	else{
		wm_print(window_id, "Sorry, command not found. Please try another command.\n");
		return;
	}

	shell_print_process_headings(window_id, print_all_details);
    for (int i = 0; i < MAX_PROCS; i++, process++) {
        if (!process->used)
            continue;
        shell_print_process_details(window_id, process, print_all_details);
    }
    
}


/* Returns a count of processes from the process table. */
int process_count(){
	PCB *process = pcb;
	int process_count = 0;
	for (int i = 0; i < MAX_PROCS; i++, process++) {
        if (!process->used)
            continue;
        process_count++;
    }
    return process_count;
}


/* Prints all process details until a keyboard interrupt occurs. */
void print_processes_until_keyboard_interrupt(int window_id){
	int keyboard_interrupt = 1;
	while(keyboard_interrupt){
		list_processes(window_id, "-d");
		for(int i = 0; i < (MAX_PROCS - process_count()); i++){
			wm_print(window_id, "\n");
		}
		if(keyb_get_keystroke(window_id, FALSE)){
			keyboard_interrupt = 0;
		}
	}
	wm_print(window_id, "Keyboard interrupt detected.\n");
}


/* Prints an about me text onto the current window. */
void print_about(int window_id){
	wm_print(window_id, "Hey there! I am Supritha Amudhu, a passionate programmer and a bibliophile. :)\n");
}


/* Method that is called after the mapping of shell commands to the array shell_commands is done.
	This method has a set of switch cases that will call the respective methods for each available command. */
void execute_shell_commands(int window_id, int command_index, char* command_parameter){
	switch(command_index){

		/* Switch case for 'man or man <parameter>' */
		case COMMAND_MAN:
			call_man(window_id, command_parameter);
		break;

		/* Switch case for 'clear' */
		case COMMAND_CLEAR:
			if(command_parameter != "None"){
				wm_print(window_id, "Sorry, command not found. Please try another command.\n");
			}
			else{
				clear_screen(window_id);
			}
		break;

		/* Switch case for 'shell' */
		case COMMAND_SHELL:
			if(command_parameter != "None"){
				wm_print(window_id, "Sorry, command not found. Please try another command.\n");
			}
			else{
				create_new_shell();
			}
		break;

		/* Switch case for 'pong' */
		case COMMAND_PONG:
			if(command_parameter != "None"){
				wm_print(window_id, "Sorry, command not found. Please try another command.\n");
			}
			else{
				launch_pong();
			}
		break;

		/* Switch case for 'echo <message>' */
		case COMMAND_ECHO:
			print_echo_message(window_id, command_parameter);
		break;

		/* Switch case for 'ps' and 'ps -d' */
		case COMMAND_PS:
			list_processes(window_id, command_parameter);
		break;

		/* Switch case for 'top' */
		case COMMAND_TOP:
			if(command_parameter != "None"){
				wm_print(window_id, "Sorry, command not found. Please try another command.\n");
			}
			else{
				print_processes_until_keyboard_interrupt(window_id);
			}
		break;

		/* Switch case for 'about' */
		case COMMAND_ABOUT:
			if(command_parameter != "None"){
				wm_print(window_id, "Sorry, command not found. Please try another command.\n");
			}
			else{
				print_about(window_id);
			}
		break;

		/* Default case if there is no command mapping. */
		default:
			wm_print(window_id, "Sorry, command not found. Please try another command.\n");
		break;
	};

	/* Prints out a terminal symbol. */
	wm_print(window_id, "\n%s", TERMINAL_SYMBOL);
}



/* The method that gets executed when a shell process is created and executed. */
void shell_process(PROCESS process, PARAM param){
	int window_id = wm_create(1, 1, 65, 22);
	window_id_array[window_id_counter] = window_id;
	window_id_counter++;
	view_available_commands(window_id);
	wm_print(window_id, "\n%s", TERMINAL_SYMBOL);
	char* command = malloc(MAX_COMMAND_LENGTH);
	int command_index = 0;

	while(1){
		char key = keyb_get_keystroke(window_id, TRUE);
		switch(key){
			/* If the user types a backspace. */
			case BACKSPACE:
				if(command_index == 0){
					continue;
				}
				else{
					wm_print(window_id, "\b");
					command_index--;
					command[command_index] = '\0';
					continue;
				}
			break;

			/* If the user prints a empty space. */
			case EMPTY_SPACE_CHAR:
				command[command_index] = key;
				command_index++;
				wm_print(window_id, EMPTY_SPACE_STRING);
			break;

			/* If the user presses Enter. */
			case NEXT_LINE:
				wm_print(window_id, "\n");
				find_command(window_id, command, command_index);
				command = malloc(MAX_COMMAND_LENGTH);
				command_index = 0;
			break;

			/* If the user enters a horizontal tab space. */
			case TAB_SPACE:
				command[command_index] = key;
				command_index++;
				wm_print(window_id, "Does it come here?\t");
			break;

			/* Every character typed by the user queued up as in the command string. */
			default:
				wm_print(window_id, "%c", key);
				command[command_index] = key;
				command_index++;
			break;
		};
	}
}


/* Start the Shell process and resign Boot process. */
void start_shell()
{
	create_process(shell_process, 5, 0, "Shell Process");
	resign();
}
