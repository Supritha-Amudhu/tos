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

#define EMPTY_SPACE ' '
#define TERMINAL_SYMBOL ">>"
#define NEXT_LINE '\n'
#define TAB_SPACE '\t'

struct available_commands{
	int key;
	char* command;
	char* description;
};

struct available_commands command_man = {COMMAND_MAN+1, "man [command]", "Print a short description for a command or list all commands"};
struct available_commands command_clear = {COMMAND_CLEAR+1, "clear", "Clear the current window"};
struct available_commands command_shell = {COMMAND_SHELL+1, "shell", "Launch a new Shell"};
struct available_commands command_pong = {COMMAND_PONG+1, "pong", "Launch a Ping Pong game"};
struct available_commands command_echo = {COMMAND_ECHO+1, "echo <message>", "Print a message on the Screen"};
struct available_commands command_ps = {COMMAND_PS+1, "ps [-d]", "Print a list of TOS processes"};
struct available_commands command_top = {COMMAND_TOP+1, "top", "Print the Process table every second until interrupted"};
struct available_commands command_about = {COMMAND_ABOUT+1, "about", "Print your name"};
struct available_commands command_list = {COMMAND_ALL+1, "list all options", "Lists all available options"};



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
							{"about", "Print your name"},
							{"list", "Lists all available options"}
						};



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
	wm_print(window_id, "%d. %s 			-		%s\n", command_list.key, command_list.command, command_list.description);
	wm_print(window_id, "Please choose an option:\n");
}


// List of all helper processes

char* trim_white_spaces(int window_id, char* input, int input_length){
	// char* output;

	//Trim leading white space
	int i = 0;
	while(input[i] == EMPTY_SPACE && i < input_length){
		i++;
		wm_print(window_id, "Trim leading white space: %s\n", input);
	} 

	//If all the characters is empty space
	if(*input = 0){
		wm_print(window_id, "All characters empty space: %s\n", input);
		return input;
	}

	//Trim trailing white space
	// output = input;// + strlen(input) - 1;
	// int o = input_length;
	// while((output > input) && (output[o] == EMPTY_SPACE)){
	// 	output--;	
	// 	wm_print(window_id, "Trim trailing white space: %s\n", output);
	// } 
	// *(output+1) = 0;
	// wm_print(window_id, "Final trimmed output: %s\n", input);
	return input; 
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
	// char* command_trimmed = trim_white_spaces(window_id, command, command_length);
	for(int index = 0; index < 8; index++){
		char* command_string = shell_commands[index][0];
		if(str_compare(command, command_string) == 1){
			execute_shell_commands(window_id, index);
			return 0;
		}
	}
	execute_shell_commands(window_id, 55);
}


void shell_print_process_headings(int window_id){
	wm_print(window_id, "Comes here?\n");
	wm_print(window_id, "State 				Active	 Priority	Name\n");
	wm_print(window_id, "-------------------------------------------\n");
}

void shell_print_process_details(int window_id, PROCESS process){
	wm_print(window_id, "Prints details?\n");
	static const char* process_states = 
						{
							"READY			",
							"ZOMBIE			",
							"SEND_BLOCKED	",
							"REPLY_BLOCKED	",
							"RECEIVE_BLOCKED",
							"MESSAGE_BLOCKED",
							"INTR_BLOCKED	"
						};
	if(!process->used){
		wm_print(window_id, "PCB slot unused.\n");
		return;
	}
	wm_print(window_id, process_states[process->state]);
	//Check for active proc, if active print *, if not print an empty space and the rest of the details
	if(process == active_proc){
		wm_print(window_id, " *			");
	}
	else{
		wm_print(window_id, "			");
		wm_print(window_id, process->priority);
		wm_print(window_id, process->name);
	}
}



// Main shell commands

void call_man(int window_id, int command_id){
	char* command_name = '\0';
	if(command_name){
		wm_print(window_id, shell_commands[command_id][1]);
	}
	else{
		view_available_commands(window_id);
		wm_print(window_id, TERMINAL_SYMBOL);
	}
}


//Invokes the clear_wm() method which clears the window contents when a window ID is specified.
void clear_screen(int window_id){
	wm_clear(window_id);
}


void create_new_shell(){
	// create_process(shell_process, 5, 0, 'Shell Process Created');
}


//Invoke the method start_pong() which will start the Ping Pong game.
void launch_pong(){
	start_pong();
}


void print_echo_message(int window_id, char* message){
	wm_print(window_id, message);
}


//Calls the ps [-d] command to list all processes on TOS
void list_processes(int window_id){
	wm_print(window_id, "Initial method visited?\n");
	PCB* process = pcb;
	shell_print_process_headings(window_id);
	for(int index=0; index < MAX_PROCS; index++){
		if(!process->used){
			continue;
		}
		shell_print_process_details(window_id, process);	
	}
	
}


void print_processes_until_interrupt(){

}


void print_about(int window_id){
	wm_print(window_id, "Hey there! I am Supritha Amudhu, a passionate programmer and a bibliophile. :)\n");
}

void list_options(int window_id){
	view_available_commands(window_id);
}


void execute_shell_commands(int window_id, int command_id){
	switch(command_id){
		case COMMAND_MAN:
			call_man(window_id, command_id);
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
			print_echo_message(window_id, "Testing echo");
		break;

		case COMMAND_PS:
			list_processes(window_id);
		break;

		case COMMAND_TOP:
			print_processes_until_interrupt();
		break;

		case COMMAND_ABOUT:
			print_about(window_id);
		break;

		case COMMAND_ALL:
			list_options(window_id);
		break;

		default:
			wm_print(window_id, "Sorry, command not found. Please try another command.\n");
		break;
	};
	wm_print(window_id, TERMINAL_SYMBOL);
}




void shell_process(PROCESS process, PARAM param){
	int window_id = wm_create(5, 2, 60, 18);
	wm_print(window_id, TERMINAL_SYMBOL);
	view_available_commands(window_id);
	wm_print(window_id, TERMINAL_SYMBOL);
	char* command = malloc(MAX_COMMAND_LENGTH);
	int command_index = 0;

	while(1){
		char key = keyb_get_keystroke(window_id, TRUE);
		int command_id = key - '0';

		if(key == EMPTY_SPACE){
			wm_print(window_id, EMPTY_SPACE);
		}
		else if(key == NEXT_LINE || key == 13){
			wm_print(window_id, "\n");
			find_command(window_id, command_id, command, command_index);
			command = malloc(MAX_COMMAND_LENGTH);
			command_index = 0;
		}
		else if(key == TAB_SPACE){
			wm_print(window_id, "\t");
		}
		else{
			wm_print(window_id, "%c", key);
			command[command_index] = key;
			command_index++;
		}
	}
}


// Start the Shell process and resign kernel
void start_shell()
{
	create_process(shell_process, 5, 0, 'Shell Process Created');
	resign();
}
