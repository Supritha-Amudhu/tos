#include <kernel.h>

#define COMMAND_ALL 0
#define COMMAND_MAN 1
#define COMMAND_CLEAR 2
#define COMMAND_SHELL 3
#define COMMAND_PONG 4
#define COMMAND_ECHO 5
#define COMMAND_PS 6
#define COMMAND_TOP 7
#define COMMAND_ABOUT 8

#define TOTAL_SHELL_COMMANDS 8

#define EMPTY_SPACE " "
#define TERMINAL_SYMBOL ">"
#define NEXT_LINE "\n"
#define TAB_SPACE "\t"

static char* shell_commands[] = {
									"man [command]",
									"clear",
									"shell",
									"pong",
									"echo <message>",
									"ps [-d]",
									"top",
									"about"
								};

struct available_commands{
	int key;
	char* command;
	char* description;
};

struct available_commands command_man = {COMMAND_MAN, "man[command]", "Print a short description for a command or list all commands"};
struct available_commands command_clear = {COMMAND_CLEAR, "clear", "Clear the current window"};
struct available_commands command_shell = {COMMAND_SHELL, "shell", "Launch a new Shell"};
struct available_commands command_pong = {COMMAND_PONG, "pong", "Launch a Ping Pong game"};
struct available_commands command_echo = {COMMAND_ECHO, "echo<message>", "Print a message on the Screen"};
struct available_commands command_ps = {COMMAND_PS, "ps [-d]", "Print a list of TOS processes"};
struct available_commands command_top = {COMMAND_TOP, "top", "Print the Process table every second until interrupted"};
struct available_commands command_about = {COMMAND_ABOUT, "about", "Print your name"};

//This method will list all available commands in Shell by providing the window ID and the content to be displayed.
void view_available_commands(int window_id){
	wm_print(window_id, "These are the cool Shell operations that you can perform! Try them :)\n");
	wm_print(window_id, "%d. %s 			-		%s\n", command_man.key, command_man.command, command_man.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_clear.key, command_clear.command, command_clear.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_shell.key, command_shell.command, command_shell.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_pong.key, command_pong.command, command_pong.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_echo.key, command_echo.command, command_echo.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_ps.key, command_ps.command, command_ps.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_top.key, command_top.command, command_top.description);
	wm_print(window_id, "%d. %s 			-		%s\n", command_about.key, command_about.command, command_about.description);
	wm_print(window_id, "Please choose an option:\n");
}



void call_man(){

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

}


void print_processes_until_interrupt(){

}


void print_about(int window_id){
	wm_print(window_id, "Hey there! I am Supritha Amudhu, a passionate programmer and a bibliophile. :)");
}


void execute_shell_commands(int window_id, int command){
	switch(command){
		case COMMAND_MAN:
			call_man();
		break;

		case COMMAND_CLEAR:
			clear_screen(5);
		break;

		case COMMAND_SHELL:
			create_new_shell();
		break;

		case COMMAND_PONG:
			launch_pong();
		break;

		case COMMAND_ECHO:
			print_echo_message(5, "Testing echo");
		break;

		case COMMAND_PS:
			list_processes(5);
		break;

		case COMMAND_TOP:
			print_processes_until_interrupt();
		break;

		case COMMAND_ABOUT:
			print_about(5);
		break;

		default:
			wm_print(window_id, "Sorry, command not found. Please try another command.\n");
		break;
	};
}

void shell_process(PROCESS process, PARAM param){
	int window_id = wm_create(5, 2, 60, 18);
	wm_print(window_id, TERMINAL_SYMBOL);
	view_available_commands(window_id);
	while(1){
		char key = keyb_get_keystroke(window_id, TRUE);
		wm_print(window_id, ("The option that you chose is: %c\n", key));
		if(key == EMPTY_SPACE){
			wm_print(window_id, EMPTY_SPACE);
		}
		else if(key == NEXT_LINE){
			wm_print(window_id, "\n");
		}
		else if(key == TAB_SPACE){
			wm_print(window_id, "\t");
		}
		else{
			int command = key - '0';
			wm_print(window_id, command);
			// execute_shell_commands();
		}
	}
}


// Start the Shell process and resign kernel
void start_shell()
{
	create_process(shell_process, 5, 0, 'Shell Process Created');
	resign();
}
