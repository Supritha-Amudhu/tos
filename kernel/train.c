/*    
Student Name: Supritha Amudhu
Student ID: 917946619
Student Email: samudhu@mail.sfsu.edu
*/

#include <kernel.h>

#define TRAIN_WINDOW_WIDTH 50
#define TRAIN_WINDOW_HEIGHT 16
#define SLEEP_TICKS 3

#define CONFIGURATION_COUNT 4
#define SWITCH_COUNT 5

#define CLEAR_MEMORY_BUFFER "R\015"


// **************************
// run the train application
// **************************

/* Method that wll reverse the direction of the train */
void set_train_direction(int window_id, char* direction){
	sleep(SLEEP_TICKS);

	COM_Message direction_message;

	direction_message.output_buffer = direction;
	direction_message.input_buffer = NULL;
	direction_message.len_input_buffer = 0;

	send(com_port, &direction_message);
	sleep(SLEEP_TICKS);

	wm_print(window_id, "Changing direction of train to: %s\n", direction);
}


/* This method clears the output memory buffer */
void clear_memory_buffer(int window_id){
	COM_Message clear_buffer;

	clear_buffer.output_buffer = CLEAR_MEMORY_BUFFER;
	clear_buffer.input_buffer = NULL;
	clear_buffer.len_input_buffer = 0;

	sleep(SLEEP_TICKS);
	send(com_port, &clear_buffer);
}


/* This method toggles a switch between Red and Green based on the command being sent as active_switch */
void toggle_switch(int window_id, char* active_switch){
	sleep(SLEEP_TICKS);

	COM_Message toggle_switch_message;

	toggle_switch_message.output_buffer = active_switch;
	toggle_switch_message.input_buffer = NULL;
	toggle_switch_message.len_input_buffer = 0;

	send(com_port, &toggle_switch_message);
	sleep(SLEEP_TICKS);

	wm_print(window_id, "Toggling switch: %s\n", active_switch);
}

/* This method changes the speed of the train with and without the wagon */
void change_speed(int window_id, char* speed){
	sleep(SLEEP_TICKS);

	COM_Message speed_change_message;

	speed_change_message.output_buffer = speed;
	speed_change_message.input_buffer = NULL;
	speed_change_message.len_input_buffer = 0;

	send(com_port, &speed_change_message);
	sleep(SLEEP_TICKS);

	wm_print(window_id, "Changing speed of train to: %d\n", speed);
}

/* This metod probes a Contact and checks if there is either a train/wagon/Zamboni on that Contact */
int probe_contact(int window_id, char* contact, int timer){
	wm_print(window_id, "Probing contact: %s\n", contact);

	int index = 0;
	int probe_status = 0;

	while( index < timer){
		clear_memory_buffer(window_id);

		COM_Message probe_message;
		char buffer[3];

		probe_message.output_buffer = contact;
		probe_message.input_buffer = buffer;
		probe_message.len_input_buffer = 3;

		sleep(SLEEP_TICKS);
		send(com_port, &probe_message);

		probe_status = buffer[1] - '0';

		if(probe_status == 1){
			break;
		}
		index++;
	}
	wm_print(window_id, "\n");
	return probe_status;
	
}

/* The method confirms the type of configuration by checking out the positions of the train and wagon */
int confirm_train_wagon_positions(window_id){
	wm_print(window_id, "Confirming positions of train and wagon.\n");

	char* train_position[4][4] = {
		{ "C5\015", "C12\015" },
		{ "C11\015", "C2\015" },
		{ "C16\015", "C2\015" },
		{ "C16\015", "C8\015" }
	};

	int train_status = 0;
	int wagon_status = 0;

	for(int index = 0; index < CONFIGURATION_COUNT; index++){
			train_status = probe_contact(window_id, train_position[index][0], 2);
			wagon_status = probe_contact(window_id, train_position[index][1], 2);

			if(train_status == 0 && wagon_status == 0){
				continue;
			}

			if(train_status == 1 && wagon_status == 1){
				wm_print(window_id, "Train and Wagon are in position:\n");
				wm_print(window_id, "Configuration %d detected: \n", (index+1));
				return (index+1);
			}
		}
	wm_print(window_id, "Configuration not confirmed: 0\n");
	return 0;
}

/* The method that sets up all of the initial switches so that the Zamboni always travels in the outer loop. */
void initialize_train_tracks(int window_id){
	wm_print(window_id, "Setting up switches. \n");
	wm_print(window_id, "Setting up train tracks. \n");

	char* init_switch[] = {
		"M4G\015",
		"M5G\015",
		"M8G\015",
		"M9R\015",
		"M1G\015"
	};


	for(int i = 0; i < SWITCH_COUNT; i++){
		COM_Message message;

		message.output_buffer = init_switch[i];
		message.input_buffer = NULL;
		message.len_input_buffer = 0;

		send(com_port, &message);
		sleep(SLEEP_TICKS - 1);
		wm_print(window_id, "%s\n", message.output_buffer);
	}
}


/* Code for running Configuration 1 with and without Zamboni  */
void configuration_1(int window_id, int zamboni){
	wm_print(window_id, "Configuration 1.\n");
	int status = 0;

	toggle_switch(window_id, "M3R\015");
	toggle_switch(window_id, "M6G\015");
	toggle_switch(window_id, "M7G\015");
	toggle_switch(window_id, "M2R\015");

	sleep(SLEEP_TICKS - 1);

	if(zamboni == 1){
		status = probe_contact(window_id, "C7\015", 60);
	}

	change_speed(window_id, "L20S5\015");

	if(zamboni == 1){
		status = probe_contact(window_id, "C10\015", 60);
	}

	change_speed(window_id, "L20S4\015");
	toggle_switch(window_id, "M5R\015");
	change_speed(window_id, "L20S5\015");

	status = probe_contact(window_id, "C9\015", 60);

	toggle_switch(window_id, "M5G\015");
	change_speed(window_id, "L20S4\015");

	status = probe_contact(window_id, "C12\015", 80);
	change_speed(window_id, "L20S3\015");
	status = probe_contact(window_id, "C12\015", 80);
	change_speed(window_id, "L20S2\015");
	status = probe_contact(window_id, "C12\015", 80);
	change_speed(window_id, "L20S0\015");

	toggle_switch(window_id, "M1R\015");
	toggle_switch(window_id, "M2G\015");

	set_train_direction(window_id, "L20D\015");
	change_speed(window_id, "L20S5\015");

	status = probe_contact(window_id, "C1\015", 80);

	toggle_switch(window_id, "M1G\015");

	status = probe_contact(window_id, "C6\015", 80);

	toggle_switch(window_id, "M4R\015");
	toggle_switch(window_id, "M3R\015");

	change_speed(window_id, "L20S2\015");
	change_speed(window_id, "L20S0\015");
	set_train_direction(window_id, "L20D\015");
	
	change_speed(window_id, "L20S4\015");

	status = probe_contact(window_id, "C5\015", 80);
	change_speed(window_id, "L20S3\015");
	change_speed(window_id, "L20S2\015");
	change_speed(window_id, "L20S0\015");
	wm_print(window_id, "Hurraaayyy! Configuration 1 is done!\n");
	become_zombie();
	
}

/* Code for running Configuration 2 with and without Zamboni  */
void configuration_2(int window_id, int zamboni){
	wm_print(window_id, "Configuration 2.\n");
	int status = 0;

	toggle_switch(window_id, "M1G\015");
	toggle_switch(window_id, "M9R\015");
	toggle_switch(window_id, "M8G\015");
	toggle_switch(window_id, "M5G\015");
	toggle_switch(window_id, "M4G\015");
	toggle_switch(window_id, "M2G\015");
	toggle_switch(window_id, "M7R\015");
	toggle_switch(window_id, "M3G\015");

	sleep(SLEEP_TICKS - 1);

	if(zamboni == 1){
		status = probe_contact(window_id, "C10\015", 60);
	}

	set_train_direction(window_id, "L20D\015");
	change_speed(window_id, "L20S5\015");

	status = probe_contact(window_id, "C13\015", 60);
	change_speed(window_id, "L20S2\015");
	change_speed(window_id, "L20S0\015");
	set_train_direction(window_id, "L20D\015");
	change_speed(window_id, "L20S5\015");

	if(zamboni == 1){
		status = probe_contact(window_id, "C4\015", 60);
	}
	toggle_switch(window_id, "M4R\015");

	status = probe_contact(window_id, "C1\015", 60);
	toggle_switch(window_id, "M4G\015");

	if(zamboni == 1){
		status = probe_contact(window_id, "C10\015", 60);
	}
	toggle_switch(window_id, "M8R\015");

	status = probe_contact(window_id, "C13\015", 60);
	change_speed(window_id, "L20S4\015");
	change_speed(window_id, "L20S3\015");

	status = probe_contact(window_id, "C11\015", 60);
	change_speed(window_id, "L20S2\015");
	change_speed(window_id, "L20S0\015");
	toggle_switch(window_id, "M8G\015");
	wm_print(window_id, "Hurraaayyy! Configuration 2 is done!\n");
	become_zombie();
}

/* Code for running Configuration 3 with and without Zamboni  */
void configuration_3(int window_id, int zamboni){
	wm_print(window_id, "Configuration 3.\n");
	int status = 0;

	toggle_switch(window_id, "M1G\015");
	toggle_switch(window_id, "M9G\015");
	toggle_switch(window_id, "M8G\015");
	toggle_switch(window_id, "M5G\015");
	toggle_switch(window_id, "M4G\015");
	toggle_switch(window_id, "M2G\015");
	toggle_switch(window_id, "M7R\015");
	toggle_switch(window_id, "M3G\015");

	sleep(SLEEP_TICKS - 1);

	if(zamboni == 1){
		status = probe_contact(window_id, "C13\015", 60);
	}

	change_speed(window_id, "L20S5\015");

	if(zamboni == 1){
		status = probe_contact(window_id, "C4\015", 60);
	}
	toggle_switch(window_id, "M4R\015");
	status = probe_contact(window_id, "C1\015", 60);
	toggle_switch(window_id, "M4G\015");

	status = probe_contact(window_id, "C15\015", 60);
	change_speed(window_id, "L20S4\015");

	status = probe_contact(window_id, "C14\015", 60);
	change_speed(window_id, "L20S2\015");
	change_speed(window_id, "L20S0\015");
	set_train_direction(window_id, "L20D\015");
	change_speed(window_id, "L20S4\015");

	status = probe_contact(window_id, "C16\015", 60);
	change_speed(window_id, "L20S3\015");
	change_speed(window_id, "L20S2\015");
	change_speed(window_id, "L20S0\015");
	wm_print(window_id, "Hurraaayyy! Configuration 3 is done!\n");
	become_zombie();
}

/* Code for running Configuration 4 with and without Zamboni  */
void configuration_4(int window_id, int zamboni){
	wm_print(window_id, "Configuration 4.\n");
	int status = 0;

	toggle_switch(window_id, "M1G\015");
	toggle_switch(window_id, "M9G\015");
	toggle_switch(window_id, "M8G\015");
	toggle_switch(window_id, "M5G\015");
	toggle_switch(window_id, "M4G\015");
	toggle_switch(window_id, "M2G\015");
	toggle_switch(window_id, "M7R\015");
	toggle_switch(window_id, "M3G\015");
	toggle_switch(window_id, "M6R\015");

	sleep(SLEEP_TICKS - 1);

	if(zamboni == 1){
		status = probe_contact(window_id, "C10\015", 60);
	}

	change_speed(window_id, "L20S5\015");

	if(zamboni == 1){
		status = probe_contact(window_id, "C7\015", 60);
	}

	toggle_switch(window_id, "M5R\015");

	status = probe_contact(window_id, "C10\015", 60);
	change_speed(window_id, "L20S4\015");

	status = probe_contact(window_id, "C7\015", 60);
	change_speed(window_id, "L20S2\015");
	change_speed(window_id, "L20S0\015");
	set_train_direction(window_id, "L20D\015");
	change_speed(window_id, "L20S3\015");
	change_speed(window_id, "L20S5\015");

	status = probe_contact(window_id, "C7\015", (SLEEP_TICKS - 1));
	change_speed(window_id, "L20S4\015");

	status = probe_contact(window_id, "C8\015", (SLEEP_TICKS - 1));
	change_speed(window_id, "L20S3\015");
	status = probe_contact(window_id, "C8\015", (SLEEP_TICKS - 1));
	change_speed(window_id, "L20S2\015");
	status = probe_contact(window_id, "C8\015", (SLEEP_TICKS - 1));
	change_speed(window_id, "L20S1\015");
	change_speed(window_id, "L20S0\015");

	set_train_direction(window_id, "L20D\015");

	if(zamboni == 1){
		status = probe_contact(window_id, "C6\015", 60);
	}
	change_speed(window_id, "L20S5\015");

	if(zamboni == 1){
		status = probe_contact(window_id, "C14\015", 60);
	}
	status = probe_contact(window_id, "C15\015", 60);
	change_speed(window_id, "L20S4\015");
	

	status = probe_contact(window_id, "C14\015", 60);
	change_speed(window_id, "L20S3\015");
	change_speed(window_id, "L20S2\015");
	change_speed(window_id, "L20S1\015");
	change_speed(window_id, "L20S0\015");

	set_train_direction(window_id, "L20D\015");
	change_speed(window_id, "L20S5\015");
	change_speed(window_id, "L20S4\015");

	status = probe_contact(window_id, "C16\015", 60);
	change_speed(window_id, "L20S3\015");
	change_speed(window_id, "L20S2\015");
	change_speed(window_id, "L20S1\015");
	change_speed(window_id, "L20S0\015");
	wm_print(window_id, "Hurraaayyy! Configuration 4 is done!\n");
	become_zombie();
}

/* The train process that checks for the position of the train, decides on the configuration type without
 or without Zamboni. */
void train_process(PROCESS self, PARAM param)
{
	int window_id = wm_create(6, 4, TRAIN_WINDOW_WIDTH, TRAIN_WINDOW_HEIGHT);
	wm_print(window_id, "init_train called.\n");
	
	initialize_train_tracks(window_id);

	int configuration = confirm_train_wagon_positions(window_id);

	wm_print(window_id, "Checking for Zamboni.\n");
	int zamboni = probe_contact(window_id, "C4\015", 40);
	if(zamboni){
		wm_print(window_id, "RUN! Zamboni detected!\n");
	}
	else{
		wm_print(window_id, "The coast is clear! No Zamboni!\n");
	}
	switch(configuration){

		case 1:
			configuration_1(window_id, zamboni);
		break;

		case 2:
			configuration_2(window_id, zamboni);
		break;

		case 3:
			configuration_3(window_id, zamboni);
		break;

		case 4:
			configuration_4(window_id, zamboni);
		break;

		default:
			wm_print(window_id, "Invalid configuration. Please try another.\n");
	}

	while(1){
		resign();
	}
	
}


/* The init function that is called from the Shell script on typing 'Train'. */
void init_train()
{
	PORT train_port = create_process(train_process, 5, 0, "Model Train Process");
	resign();
}
