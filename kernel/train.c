/*    
Student Name: Supritha Amudhu
Student ID: 917946619
Student Email: samudhu@mail.sfsu.edu
*/

#include <kernel.h>

#define TRAIN_WINDOW_WIDTH 50
#define TRAIN_WINDOW_HEIGHT 16


// **************************
// run the train application
// **************************

void train_process(PROCESS self, PARAM param)
{
	int window_id = create_window(6, 4, TRAIN_WINDOW_WIDTH, TRAIN_WINDOW_HEIGHT);
	wm_print(window_id, "Setting up switches. \n");
	wm_print(window_id, "Setting up train tracks. \n");

	while(1){
		resign();
	}
	
}


void init_train()
{
	PORT train_port = create_process(train_process, 5, 0, "Model Train Process");
	resign();
}
