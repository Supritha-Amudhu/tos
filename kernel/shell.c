#include <kernel.h>

#define COMMAND_ALL 8
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

struct available_commands{
	int key;
	char* command;
	char* description;
};

struct command_components{
	char* command;
	char* parameter;
};

struct available_commands command_man = {COMMAND_MAN+1, "man [command]", "Print a short description for a command or list all commands"};
struct available_commands command_clear = {COMMAND_CLEAR+1, "clear", "Clear the current window"};
struct available_commands command_shell = {COMMAND_SHELL+1, "shell", "Launch a new Shell"};
struct available_commands command_pong = {COMMAND_PONG+1, "pong", "Launch a Ping Pong game"};
struct available_commands command_echo = {COMMAND_ECHO+1, "echo <message>", "Print a message on the Screen"};
struct available_commands command_ps = {COMMAND_PS+1, "ps [-d]", "Print a list of TOS processes"};
struct available_commands command_top = {COMMAND_TOP+1, "top", "Print the Process table every second until interrupted"};
struct available_commands command_about = {COMMAND_ABOUT+1, "about", "Print your name"};



// static char* shell_commands[] = {
// 							command_man.command,
// 							command_clear.command,
// 							command_shell.command,
// 							command_pong.command,
// 							command_echo.command,
// 							command_ps.command,
// 							command_top.command,
// 							command_about.command
// 						};

static char* shell_commands[8][2] = {
							{"man", "Print a short description for a command or list all commands"},
							{"clear", "Clear the current window"},
							{"shell", "Launch a new Shell"},
							{"pong", "Launch a Ping Pong game"},
							{"echo", "Print a message on the Screen"},
							{"ps", "Print a list of TOS processes"},
							{"top", "Print the Process table every second until interrupted"},
							{"about", "Print your name"}
							// {"list", "Lists all available options"}
						};

int* window_id_array[MAX_PROCS];

//This method will list all available commands in Shell by providing the window ID and the content to be displayed.
void view_available_commands(int window_id){
	// wm_print(window_id, "These are the cool Shell operations that you can perform! Try them :)\n");
	wm_print(window_id, "%d. %s 			-		%s\n", command_man.key, command_man.command, command_man.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_clear.key, command_clear.command, command_clear.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_shell.key, command_shell.command, command_shell.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_pong.key, command_pong.command, command_pong.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_echo.key, command_echo.command, command_echo.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_ps.key, command_ps.command, command_ps.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_top.key, command_top.command, command_top.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_about.key, command_about.command, command_about.description);
	// wm_print(window_id, "%d. %s 			-		%s\n", command_list.key, command_list.command, command_list.description);
	wm_print(window_id, "Please choose an option:\n");
}


// List of all helper processes

struct command_components trim_white_spaces(int window_id, char* input, int input_length){
	struct command_components command_final;
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
			// wm_print(window_id, "Character at position %d is empty.\n", i);
			continue;
		}
		else{
			if(command_parameter_index != 0){
				// wm_print(window_id, "The first command parameter has started.\n");
				int j = i;
				while(input[j] != '\0'){
					command_parameter[command_parameter_index] = input[j];
					j++;
					command_parameter_index++;
				}		
				// wm_print(window_id, "Command parameter until now: %s\n", command_parameter);
				break;
			}
			else{
				output_command[output_command_index] = input[i];
				// wm_print(window_id, "output_command in each loop: %s\n", output_command);
				output_command_index++;
				// wm_print(window_id, "input[i] in each loop: %c\n", input[i]);
			}	
		}

		if(white_space_count >= input_length){
			wm_print(window_id, "Empty command entered.\n");
			return command_final;
		}	
	}
	struct command_components command_final1 = {output_command, command_parameter};
	// wm_print(window_id, "Output command: %s\n", output_command);
	// wm_print(window_id, "Command parameter: %s\n", command_parameter);
	return command_final1; 
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

int find_command(int window_id, int command_id, char* command, int command_length){
	struct command_components command_final = trim_white_spaces(window_id, command, command_length);
	for(int index = 0; index < 8; index++){
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


void shell_print_process_headings(int window_id){
	wm_print(window_id, "State                Active  Priority       Name\n");
	wm_print(window_id, "-----------------------------------------------------\n");
}

void shell_print_process_details(int window_id, PROCESS process){
	static const char* process_states[] = 
						{
							"READY          ",
							"ZOMBIE         ",
							"SEND_BLOCKED   ",
							"REPLY_BLOCKED  ",
							"RECEIVE_BLOCKED",
							"MESSAGE_BLOCKED",
							"INTR_BLOCKED   "
						};
	if(!process->used){
		wm_print(window_id, "PCB slot unused.\n");
		return;
	}
	wm_print(window_id, "%s  ", process_states[process->state]);
	//Check for active proc, if active print *, if not print an empty space and the rest of the details
	if(process == active_proc){
		wm_print(window_id, " *           ");
	}
	else{
		wm_print(window_id, "             ");
		wm_print(window_id, "  %d    ",process->priority);
		wm_print(window_id, "   %s    ",process->name);
		wm_print(window_id, "\n");
	}
}


// Main shell commands

void call_man(int window_id, int command_id, char* command_parameter){
	char* command_name = '\0';
	if(command_name){
		wm_print(window_id, shell_commands[command_id][1]);
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
	// create_process(shell_process, 5, 0, 'Shell Process Created');
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
	PCB* process = pcb;
	shell_print_process_headings(window_id);
	for(int index=0; index < MAX_PROCS; index++){
		if(!process->used){
			continue;
		}
		shell_print_process_details(window_id, process);	
	}
	
}


void print_processes_until_keyboard_interrupt(int window_id){
	int keyboard_interrupt = 1;
	while(keyboard_interrupt){
		list_processes(window_id, "");
		if(keyb_get_keystroke(window_id, TRUE)){
			keyboard_interrupt = 0;
		}
	}
	wm_print(window_id, "There was a keyboard interrupt detected.\n");
	view_available_commands(window_id);
}


void print_about(int window_id){
	wm_print(window_id, "Hey there! I am Supritha Amudhu, a passionate programmer and a bibliophile. :)\n");
}

void list_options(int window_id){
	view_available_commands(window_id);
}


void execute_shell_commands(int window_id, int command_id, char* command_parameter){
	switch(command_id){
		case COMMAND_MAN:
			call_man(window_id, command_id, command_parameter);
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
	wm_print(window_id, TERMINAL_SYMBOL);
}




void shell_process(PROCESS process, PARAM param){
	int window_id = wm_create(1, 1, 60, 18);
	window_id_array[window_id_counter] = window_id;
	window_id_counter++;
	wm_print(window_id, "Current window ID: %d\n", window_id);
	wm_print(window_id, "***************************************\n");
	view_available_commands(window_id);
	wm_print(window_id, TERMINAL_SYMBOL);
	char* command = malloc(MAX_COMMAND_LENGTH);
	int command_index = 0;
	// int backspace_count = 0;

	while(1){
		char key = keyb_get_keystroke(window_id, TRUE);
		// wm_print(window_id, "Key: %c", key);
		int command_id = key - '0';
		switch(key){
			case BACKSPACE:
				if(command_index == 0){
					// wm_print(window_id, "Count zero?\n");
					continue;
				}
				else{
					// wm_print(window_id, "Count is not zero?\n");
					wm_print(window_id, "\b");
					// backspace_count--;
					command_index--;
					command[command_index] = '\0';
					// wm_print(window_id, "Reducing Backspace Count: %d\n", backspace_count);
					continue;
				}
			break;

			case EMPTY_SPACE_CHAR:
				command[command_index] = key;
				command_index++;
				// backspace_count++;
				wm_print(window_id, EMPTY_SPACE_STRING);
			break;

			case NEXT_LINE:
				wm_print(window_id, "\n");
				wm_print(window_id, "The command: %s\n", command);
				find_command(window_id, command_id, command, command_index);
				command = malloc(MAX_COMMAND_LENGTH);
				command_index = 0;
				// backspace_count = 0;
			break;

			case TAB_SPACE:
				command[command_index] = key;
				command_index++;
				// backspace_count++;
				wm_print(window_id, "\t");
			break;

			default:
				wm_print(window_id, "%c", key);
				command[command_index] = key;
				command_index++;
				// backspace_count++;
			break;
		};
		// if(key == BACKSPACE){
		// 	// wm_print(window_id, "Does it come here?\n");
		// 	if(command_index == 0){
		// 		// wm_print(window_id, "Count zero?\n");
		// 		continue;
		// 	}
		// 	else{
		// 		// wm_print(window_id, "Count is not zero?\n");
		// 		wm_print(window_id, "\b");
		// 		// backspace_count--;
		// 		command_index--;
		// 		command[command_index] = '\0';
		// 		// wm_print(window_id, "Reducing Backspace Count: %d\n", backspace_count);
		// 		continue;
		// 	}
		// }
		// if(key == EMPTY_SPACE_CHAR){
		// 	command[command_index] = key;
		// 	command_index++;
		// 	// backspace_count++;
		// 	wm_print(window_id, EMPTY_SPACE_STRING);
		// }
		// else if(key == NEXT_LINE){
		// 	wm_print(window_id, "\n");
		// 	wm_print(window_id, "The command: %s\n", command);
		// 	find_command(window_id, command_id, command, command_index);
		// 	command = malloc(MAX_COMMAND_LENGTH);
		// 	command_index = 0;
		// 	// backspace_count = 0;
		// }
		// else if(key == TAB_SPACE){
		// 	command[command_index] = key;
		// 	command_index++;
		// 	// backspace_count++;
		// 	wm_print(window_id, "\t");
		// }
		// else{
		// 	wm_print(window_id, "%c", key);
		// 	command[command_index] = key;
		// 	command_index++;
		// 	// backspace_count++;
		// }
		// wm_print(window_id, " Backspace Count: %d\n", backspace_count);
	}
}


// Start the Shell process and resign kernel
void start_shell()
{
	create_process(shell_process, 5, 0, 'Shell Process Created');
	resign();
}
