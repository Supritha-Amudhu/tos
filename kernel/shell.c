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
#define TAB_SPACE 17
#define BACKSPACE 8


int window_id_counter = 0;

struct command_components{
	char* command;
	char* parameter;
};

static char* shell_commands[8][3] = {
							{"man", "Print a short description for a command                          or list all commands"},
							{"clear", "Clear the current window"},
							{"shell", "Launch a new Shell"},
							{"pong", "Launch a Ping Pong game"},
							{"echo", "Print a message on the Screen"},
							{"ps", "Print a list of TOS processes"},
							{"top", "Print the Process table every second unt                         il interrupted"},
							{"about", "Print your name"}
						};

int* window_id_array[MAX_PROCS];

//This method will list all available commands in Shell by providing the window ID and the content to be displayed.
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


// List of all helper processes

struct command_components trim_white_spaces(int window_id, char* input, int input_length){
	char* output_command = malloc(MAX_COMMAND_LENGTH);
	char* command_parameter = malloc(MAX_COMMAND_LENGTH);
	int output_command_index = 0;
	int command_parameter_index = 0;

	int i = 0;
	int white_space_count = 0;
	

	//Trim white space between words, find two words in the same command
	for(i = 0; i < input_length; i++){
		if(input[i] != EMPTY_SPACE_CHAR && input[i-1] == EMPTY_SPACE_CHAR && output_command_index != 0){
			command_parameter[command_parameter_index] = input[i];
			command_parameter_index++;
		}
		else if(input[i] == EMPTY_SPACE_CHAR){
			white_space_count++;
			continue;
		}
		else{
			if(command_parameter_index != 0){
				int j = i;
				while(input[j] != '\0'){
					command_parameter[command_parameter_index] = input[j];
					j++;
					command_parameter_index++;
				}		
				break;
			}
			else{
				output_command[output_command_index] = input[i];
				output_command_index++;
			}	
		}

		if(white_space_count >= input_length){
			wm_print(window_id, "Empty command entered.\n");
			struct command_components command_final = {"None", "None"};
			return command_final;
		}	
	}
	if(command_parameter_index == 0){
		command_parameter = "None";
	}
	struct command_components command_final = {output_command, command_parameter};
	return command_final; 
}

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
		//Check for active proc, if active print *, if not print an empty space and the rest of the details
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


// Main shell commands

void call_man(int window_id, char* command_parameter){
	char* command_name = '\0';
	int command_match_found = 0;
	if(command_parameter != "None"){
		for(int i = 0; i < TOTAL_SHELL_COMMANDS; i++){
			if(str_compare(command_parameter, shell_commands[i][0])){
				wm_print(window_id, "%s\n", shell_commands[i][1]);
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

//Invokes the clear_wm() method which clears the window contents when a window ID is specified.
void clear_screen(int window_id){
	wm_clear(window_id);
}


void create_new_shell(){
	start_shell();
}


//Invoke the method start_pong() which will start the Ping Pong game.
void launch_pong(){
	start_pong();
}


void print_echo_message(int window_id, char* message){
	wm_print(window_id, "%s\n", remove_quotes_echo(message));
}


//Calls the ps [-d] command to list all processes on TOS
void list_processes(int window_id, char* command_parameter){
	PCB *process = pcb;
	int print_all_details = 0;

	if(str_compare(command_parameter, "-d")){
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


void print_processes_until_keyboard_interrupt(int window_id){
	int keyboard_interrupt = 1;
	while(keyboard_interrupt){
		list_processes(window_id, "-d");
		if(keyb_get_keystroke(window_id, TRUE)){
			keyboard_interrupt = 0;
		}
	}
	wm_print(window_id, "There was a keyboard interrupt detected.\n");
	// view_available_commands(window_id);
}


void print_about(int window_id){
	wm_print(window_id, "Hey there! I am Supritha Amudhu, a passionate programmer and a bibliophile. :)\n");
}


void execute_shell_commands(int window_id, int command_index, char* command_parameter){
	switch(command_index){
		case COMMAND_MAN:
			call_man(window_id, command_parameter);
		break;

		case COMMAND_CLEAR:
			clear_screen(window_id);
		break;

		case COMMAND_SHELL:
			create_new_shell();
		break;

		case COMMAND_PONG:
			launch_pong();
		break;

		case COMMAND_ECHO:
			print_echo_message(window_id, command_parameter);
		break;

		case COMMAND_PS:
			list_processes(window_id, command_parameter);
		break;

		case COMMAND_TOP:
			print_processes_until_keyboard_interrupt(window_id);
		break;

		case COMMAND_ABOUT:
			print_about(window_id);
		break;

		default:
			wm_print(window_id, "Sorry, command not found. Please try another command.\n");
		break;
	};
	wm_print(window_id, "\n%s", TERMINAL_SYMBOL);
}




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

			case EMPTY_SPACE_CHAR:
				command[command_index] = key;
				command_index++;
				wm_print(window_id, EMPTY_SPACE_STRING);
			break;

			case NEXT_LINE:
				wm_print(window_id, "\n");
				wm_print(window_id, "The command: %s\n", command);
				find_command(window_id, command, command_index);
				command = malloc(MAX_COMMAND_LENGTH);
				command_index = 0;
			break;

			case TAB_SPACE:
				command[command_index] = key;
				command_index++;
				wm_print(window_id, "\t");
			break;

			default:
				wm_print(window_id, "%c", key);
				command[command_index] = key;
				command_index++;
			break;
		};
	}
}


// Start the Shell process and resign kernel
void start_shell()
{
	create_process(shell_process, 5, 0, "Shell Process");
	resign();
}
