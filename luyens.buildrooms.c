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



const char *rooms[] = {"CROWTHER",
					   "DUNGEON",
					   "PLUGH",
					   "PLOWER",
					   "twisty",
					   "XYZZY",
					   "ZORK",
					   "DRAGON",
					   "FIRE",
					   "SHADOW"
					  }; 


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

int getRandomRoom(){
	while(1){
		int r = rand()%10;
		if(room_flag[r]==0){
			room_flag[r]=1;
			return r;
		}
	}
}


// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull(struct room* roomArray)  
{
	int i = 0;
	for(i = 0; i < 7; i++){
		if(roomArray[i].num_connections < 3){
			return false;
		}
	}
	return true;
}

// Returns a random Room, does NOT validate if connection can be added
int GetRandomRoom()
{	
	return rand() % 7;
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(int x, struct room* roomArray) 
{
	if(roomArray[x].num_connections < 6) return true;
	return false;
}

// Returns true if a connection from Room x to Room y already exists, false otherwise
bool ConnectionAlreadyExists(int x, int y, struct room* roomArray)
{
	int i = 0;
	for(i = 0; i < 6; i++){
		if(roomArray[x].connectionsarray[i] == roomArray[y].id) return true;
	}
	return false;
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(int x, int y, struct room* roomArray) 
{
	int x_num_connections = roomArray[x].num_connections;

	roomArray[x].connectionsarray[x_num_connections] = roomArray[y].id;
	roomArray[x].num_connections++;
}

// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(int x, int y) 
{
  return(x == y);
}


// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection(struct room* roomArray)  
{
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

// Create all connections in graph
void CreateRooms2 (struct room* roomArray){
	//Initialize roomArray
	int takenRooms[7] = {-1,-1,-1,-1,-1,-1,-1};
	int* takenRooms2 = InitializeRooms(takenRooms);
	
	int i = 0; int j = 0;
	for(i = 0 ; i < 7; i++){
		memset(roomArray[i].name, '\0', 10);
		strcpy(roomArray[i].name, rooms[takenRooms[i]]);
		//printf("%s\n", roomArray[i].name);
		roomArray[i].TYPE = 0;
		for(j = 0; j < 6; j++){
			roomArray[i].connectionsarray[j] = -1;
		}
		roomArray[i].num_connections = 0;
		roomArray[i].canconnect = 1;
		roomArray[i].id = i;

	}
	
	int count = 0;
	while (IsGraphFull(roomArray) == false)
	{
	  	AddRandomConnection(roomArray);
	}
}

void createRoomFiles(struct room* roomArray)
{
	//initializes the start and end room so there's only one of each and they are different
	int start = rand()%7;
    int end = rand()%7;
    while(end==start) end = rand()%7;

    roomArray[start].TYPE = 1;
    roomArray[end].TYPE = 2;

	//printf("START: %s\n", roomArray[start].name);
	//printf("END: %s\n", roomArray[end].name);

	int i = 0; int j = 0; int flag = 0;
	for(i = 0; i<7; i++){
		FILE *fp;
		char file_name[32];
		strcpy(file_name, directory);
		strcat(file_name, "/");
		strcat(file_name, roomArray[i].name);
		strcat(file_name, "_room");


		fp = fopen(file_name,"w");
		fprintf(fp,"ROOM NAME: %s\n", roomArray[i].name);
		//printf("crf ROOM NAME: %s\n", roomArray[i].name);
		
		for(j = 0; j<6; j++){

			flag = roomArray[i].connectionsarray[j];
			//printf("Room Name: %s Flag: %d\n\n", roomArray[i].name, flag);
			//printf("Flag: %d\n", flag);
			if(-1 < flag){
				fprintf(fp,"CONNECTION %d: %s\n", j, roomArray[flag].name);	
				//printf("CONNECTION %d: %s\n", flag, roomArray[flag].name);
			}
		}

		switch(roomArray[i].TYPE){
			case 0:	fprintf(fp,"ROOM TYPE: MID_ROOM\n");	
				break;
			case 1:	fprintf(fp,"ROOM TYPE: START_ROOM\n");	
				break;
			case 2:	fprintf(fp,"ROOM TYPE: END_ROOM\n");	
				break;
		}
		//printf("Num Connections: %d\n",  roomArray[i].num_connections);
		//printf("TYPE: %d\n", roomArray[i].TYPE);


		fclose(fp);
	}
}

void main(){
	srand (time(0)); //seed for rand() to get a random number everytime the program runs
	struct room* roomArray = (struct room*)malloc(sizeof(struct room)*7);
	createDirectory();
	CreateRooms2(roomArray);
	createRoomFiles(roomArray);
	free(roomArray);
	return 0;
}
