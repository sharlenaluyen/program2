#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

/**************************************************
* Type: Function
* Parameters: 
* Returns: 
* Purpose: 
* 
****************************************************/
struct room{
	char name[10];
	int TYPE;
	int connectionsarray[6]; //6 elements for max connections
	int num_connections;
	bool canconnect;
	int id;
};

bool IsGraphFull(struct room* roomArray);
int GetRandomRoom();
bool CanAddConnectionFrom(int x, struct room* roomArray);
bool ConnectionAlreadyExists(int x, int y, struct room* roomArray);
void ConnectRoom(int x, int y, struct room* roomArray);
bool IsSameRoom(int x, int y);
void AddRandomConnection();
void CreateRooms2 (struct room* roomArray);
int* InitializeRooms(int* takenRooms);
void createRoomFiles(struct room* roomArray);


char directory[64] = "luyens.rooms.";
int room_flag[9] = {0};
int total_connections = 0;

//all room options that we can have rooms from
const char *rooms[] = {"Crowther",
					   "Dungeon",
					   "PLUGH",
					   "PLOVER",
					   "twisty",
					   "XYZZY",
					   "Zork",
					   "DRAGON",
					   "FIRE",
					   "SHADOW"
					  }; 

/**************************************************
* Type: Function
* Parameters: N/A
* Returns: N/A
* Purpose: creates a directory - from slides. 
****************************************************/
void createDirectory(){
	struct stat st = {0};
	pid_t pid;

	/* get the process id */
	if ((pid = getpid()) < 0) {
		  perror("unable to get pid");
	}

	char pid_string[32];
	sprintf(pid_string, "%d", pid);
	strcat(directory, pid_string);

	if (stat(directory, &st) == -1) {
	    mkdir(directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}else{
		printf("Error creating directory.");
	}
}

/**************************************************
* Type: Function
* Parameters: struct room*, roomArray
* Returns: true if all rooms have 3-6 connections
* Purpose: Checks to see if every room has at least 
* 3 connections.
****************************************************/
// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull(struct room* roomArray){
	int i = 0;
	for(i = 0; i < 7; i++){
		if(roomArray[i].num_connections < 3){
			return false;
		}
	}
	return true;
}

/**************************************************
* Type: Function
* Parameters: N/A
* Returns: a random number between 0 and 6, representing a room
* Purpose: To return a random room within a roomArray 
****************************************************/
// Returns a random Room, does NOT validate if connection can be added
int GetRandomRoom(){	
	return rand() % 7;
}

/**************************************************
* Type: Function
* Parameters: int x, representing the current room
			  struct room* roomArray
* Returns: true if a specified room has less than 6 
* connections
* Purpose: Checks if a specific room can be added 
****************************************************/
// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(int x, struct room* roomArray){
	if(roomArray[x].num_connections < 6) return true;
	return false;
}

/**************************************************
* Type: Function
* Parameters: int x, first room
			  int y, second room
			  struct room* roomArray
* Returns: true if a connection from first room to
* second room exists. false if they are not already
* connected
* Purpose: Checks to see if a connection already exists 
****************************************************/
// Returns true if a connection from Room x to Room y already exists, false otherwise
bool ConnectionAlreadyExists(int x, int y, struct room* roomArray){
	int i = 0;
	for(i = 0; i < 6; i++){
		if(roomArray[x].connectionsarray[i] == roomArray[y].id) return true;
	}
	return false;
}

/**************************************************
* Type: Function
* Parameters: int x, first room
			  int y, second room
			  struct room* roomArray
* Returns: N/A
* Purpose: Connects first room to second room
* based off of id number from rooms struct 
****************************************************/
// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(int x, int y, struct room* roomArray){
	int x_num_connections = roomArray[x].num_connections;

	roomArray[x].connectionsarray[x_num_connections] = roomArray[y].id;
	roomArray[x].num_connections++;
}

/**************************************************
* Type: Function
* Parameters: int x, first room
			  int y, second room
* Returns: returns true if the two sepcified rooms 
* are the same
* Purpose: Checks if two rooms are the same 
****************************************************/
// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(int x, int y){
  return(x == y);
}

/**************************************************
* Type: Function
* Parameters: struct room* roomArray
* Returns: N/A
* Purpose: Adds a random connection to the roomArray 
****************************************************/
// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection(struct room* roomArray){
	int A = 0;
	int B = 0;
	while(true){
		A = GetRandomRoom();

		if (CanAddConnectionFrom(A, roomArray) == true)
		  break;
	}

	do{
		B = GetRandomRoom();
	}
	while(CanAddConnectionFrom(B, roomArray) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B, roomArray) == true);

	ConnectRoom(A, B, roomArray);  // TODO: Add this connection to the real variables, 
	ConnectRoom(B, A, roomArray);  //  because this A and B will be destroyed when this function terminates
}

/**************************************************
* Type: Function
* Parameters: int takenRooms[7]
* Returns: array of 7 of the rooms that are taken
* Purpose: initializes room connections. Populates
* the array with numbers 0 through 9, representing
* the 10 rooms. 
****************************************************/
int* InitializeRooms(int takenRooms[7]){
	int count = 0;
	bool inthere = false;
	int ranNum = 0;
	int i = 0;
	while(count < 7){
		ranNum = rand()%10;
		inthere = false;
		for(i = 0; i < 7; i++){
			if(takenRooms[i] == ranNum){inthere = true;}
		}
		if(!inthere){
			//Add to takenRooms
			takenRooms[count] = ranNum;
			count++;
		}
	}
	return takenRooms;
}

/**************************************************
* Type: Function
* Parameters: struct room* roomArray
* Returns: N/A
* Purpose: Creates the connections between the rooms!
*  
****************************************************/
// Create all connections in graph
void CreateRooms2 (struct room* roomArray){
	//Initialize roomArray
	int takenRooms[7] = {-1,-1,-1,-1,-1,-1,-1};
	int* takenRooms2 = InitializeRooms(takenRooms);
	
	int i = 0; int j = 0;
	for(i = 0 ; i < 7; i++){
		memset(roomArray[i].name, '\0', 10); //sets name to NULL first to clear it out
		strcpy(roomArray[i].name, rooms[takenRooms[i]]); //puts the name in based off of the chosen rooms
		roomArray[i].TYPE = 0; //sets all types to mid rooms first
		for(j = 0; j < 6; j++){ //resets connectionsarray to empty
			roomArray[i].connectionsarray[j] = -1;
		}
		roomArray[i].num_connections = 0; 
		roomArray[i].canconnect = 1;
		roomArray[i].id = i; //sets the ID of the rooms to their spots of the roomArray

	}
	
	int count = 0;
	while (IsGraphFull(roomArray) == false){ //fills the rooms!
	  	AddRandomConnection(roomArray); //continuously adds random connections until all rooms are full and filled
	}
}

/**************************************************
* Type: Function
* Parameters: struct room* roomArray
* Returns: N/A
* Purpose: 
* 
****************************************************/
void createRoomFiles(struct room* roomArray){
	//initializes the start and end room 
	//ensure there's only one of each and they are different
	int start = rand()%7;
    int end = rand()%7;
    while(end==start) end = rand()%7;

    roomArray[start].TYPE = 1;
    roomArray[end].TYPE = 2;

    //writes room files properly
	int i = 0; int j = 0; int flag = 0;
	for(i = 0; i < 7; i++){
		FILE *fp;
		char file_name[32];
		strcpy(file_name, directory);
		strcat(file_name, "/");
		strcat(file_name, roomArray[i].name);
		strcat(file_name, "_room");

		//writes room name
		fp = fopen(file_name,"w");
		fprintf(fp,"ROOM NAME: %s\n", roomArray[i].name);
		
		//writes the connections
		//compares flags to value within the connectionsarray in roomArray
		for(j = 0; j<6; j++){
			flag = roomArray[i].connectionsarray[j];
			if(-1 < flag){
				fprintf(fp,"CONNECTION %d: %s\n", j, roomArray[flag].name);	
			}
		}
		//writes room types
		switch(roomArray[i].TYPE){
			case 0:	fprintf(fp,"ROOM TYPE: MID_ROOM\n");	
				break;
			case 1:	fprintf(fp,"ROOM TYPE: START_ROOM\n");	
				break;
			case 2:	fprintf(fp,"ROOM TYPE: END_ROOM\n");	
				break;
		}
		fclose(fp);
	}
}

void main(){
	srand (time(0)); //seed for rand() to get a random number everytime the program runs
	struct room* roomArray = (struct room*)malloc(sizeof(struct room)*7); //create memory for roomArrays
	createDirectory();
	CreateRooms2(roomArray);
	createRoomFiles(roomArray);
	free(roomArray); //free the memory
	return 0;
}
