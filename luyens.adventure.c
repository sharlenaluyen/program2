#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>

/**************************************************
* Type: Function
* Parameters: 
* Returns: 
* Purpose: 
* 
****************************************************/

char* dName[100]; //serves as directory name
int START = 0;
int END = 0;

struct room{
    char name[10];
    int TYPE;
    int connectionsarray[6]; //6 elements for max connections
    int num_connections;
    bool canconnect;
    int id; //id is used for placement in roomArray in adventures
};

/**************************************************
* Type: Function
* Parameters: roomArray
* Returns: N/A
* Purpose: Manipulates roomArray struct to be empty 
* and ready to be filled. connectionsarray initialized 
* to -1 to signify that there is NOT connection there.
* id is set to the place in the roomArray.
****************************************************/

void initializeRooms(struct room* roomArray){    
    int i = 0; int j = 0;
    for(i = 0 ; i < 7; i++){
        memset(roomArray[i].name, '\0', 10);
        roomArray[i].TYPE = 0;
        for(j = 0; j < 6; j++){
            roomArray[i].connectionsarray[j] = -1;
        }
        roomArray[i].num_connections = 0;
        roomArray[i].canconnect = 1;
        roomArray[i].id = i;

    }
}

/**************************************************
* Type: Function
* Parameters: newestDirName
* Returns: N/A
* Purpose: Taken from slides. Takes timestamp to
* determine the newest directory made so that it's 
* the most fresh build/compilation from buildrooms. 
****************************************************/
void getMostRecentDirectory(char* newestDirName){

    int newestDirTime = -1; // Modified timestamp of newest subdir examined
    char targetDirPrefix[32] = "luyens.rooms."; // Prefix we're looking for
    

    memset(newestDirName, '\0', sizeof(newestDirName));
    DIR* dirToCheck; // Holds the directory we're starting in
    struct dirent *fileInDir; // Holds the current subdir of the starting dir
    struct stat dirAttributes; // Holds information we've gained about subdir
    dirToCheck = opendir("."); // Open up the directory this program was run in

    if (dirToCheck > 0) 
    { // Make sure the current directory could be opened
        while ((fileInDir = readdir(dirToCheck)) != NULL) { // Check each entry in dir
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) { // If entry has prefix
                //printf("Found the prefex: %s\n", fileInDir->d_name);
                stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry
                if ((int)dirAttributes.st_mtime > newestDirTime) { // If this time is bigger
                    newestDirTime = (int)dirAttributes.st_mtime;
                    memset(newestDirName, '\0', sizeof(newestDirName));
                    strcpy(newestDirName, fileInDir->d_name);
                    //printf("Newer subdir: %s, new time: %d\n", fileInDir->d_name, newestDirTime);
                }
            }
        }
    }
    closedir(dirToCheck); // Close the directory we opened
    //printf("Newest entry found is: %s\n", newestDirName);
}

int i = 0;


/**************************************************
* Type: Function
* Parameters: char*, room_name; struct room*, roomArray
* Returns: int; returns index of specified room
* Purpose: Goes through roomArray using a room name
* to find the index of the room. 
****************************************************/
int getRoomIndex(char* room_name, struct room* roomArray){
    int i = 0;
	for (i = 0; i<7; i++){
		if(strcmp(room_name, roomArray[i].name) == 0){
           // printf("UHHHHHHHHHHHH %d name: %s\n", i, roomArray[i].name);
            return i;
        }
	}
	return -1;
}

/**************************************************
* Type: Function
* Parameters: int; current room
              char *, file name
              char*, newestDirName
              struct room*, roomArray
* Returns: N/A
* Purpose: Reads a file created from buildrooms.
* Inserts room names into roomArray. 
****************************************************/
void readRoomFile(int CURRENT, char* filename, char* newestDirName, struct room* roomArray){
	FILE *fptr;
    char ch;
    char c[1000];
    char fname[300];
    strcpy(fname, newestDirName);
    strcat(fname, "/");
  	strcat(fname, filename);

    /*  open the file for reading */
    fptr = fopen(fname, "r");
    if (fptr != NULL)
    {    
    	fscanf(fptr, "ROOM NAME: %s\n", c);
    	//printf("readRoomFile %d Room Name: %s\n", CURRENT, c);
    	strcpy(roomArray[CURRENT++].name, c);
	}
    fclose(fptr); CURRENT++;
}

/**************************************************
* Type: Function
* Parameters: char*, filename
              char*, newestDirName
              struct room*, roomArray
* Returns: N/A
* Purpose: Reads the file to fill out the pieces 
* of each room in roomArray. First, we open the file.
* Then, we use fscanf to read line by line. Then, we
* use sscanf to pattern match the strings into the proper
* elements of rooms. 
****************************************************/
void readRoomConnections(char* filename, char* newestDirName, struct room* roomArray){
	FILE *fptr;
    char ch;
    char c[1000];
    char fname[300];
    char buffer[100];
    strcpy(fname, newestDirName);
    strcat(fname, "/");
  	strcat(fname, filename);
    /*  open the file for reading */
    fptr = fopen(fname, "r");
    if (fptr != NULL)
    {    
    	fscanf(fptr, "ROOM NAME: %s\n", c);
    	printf("rRC Room Name: %s\n", c);
    	char room_last[16];
        memset(room_last, '\0', 16);
    	while (!feof(fptr))
    	{
    		char room[16];
    		int n;
            fgets(buffer, 100, fptr);                                                                                                               ;
    		if(sscanf(buffer, "CONNECTION %d: %s\n", &n, room) == 0) break;
            //printf("***Room: %s \t Room Last: %s\n", room, room_last);
    		//(strcmp(room,room_last) == 0 && (getRoomIndex(c, roomArray) == roomArray[getRoomIndex(c, roomArray)].id)){
                //printf("Is this equal? %d \t %d\n", getRoomIndex(c, roomArray), roomArray[getRoomIndex(c, roomArray)].id);
                //printf("I'M IN ** Room: %s \t Room Last: %s\n", room, room_last);
    			strcpy(room_last, room);
    			printf("rRC Room Connection: %s\n", room);
                int flag = roomArray[getRoomIndex(c, roomArray)].num_connections;
    			roomArray[getRoomIndex(c, roomArray)].connectionsarray[flag] = getRoomIndex(room, roomArray);
                printf("rRC Room Index: %d\n", getRoomIndex(c, roomArray));
                printf("rRC Unsure: %d\n\n", roomArray[getRoomIndex(c, roomArray)].num_connections);
                printf("rRC what is this? %s\n", c);
    			roomArray[getRoomIndex(c, roomArray)].num_connections++;
		}
		char type[20];
		sscanf(buffer, "ROOM TYPE: %s\n",type);
        //printf("ROOM TYPE: %s\n",type);


		if(strcmp(type, "START_ROOM")==0) {
			roomArray[getRoomIndex(c, roomArray)].TYPE = 1;
			START = getRoomIndex(c, roomArray);
		}
		if(strcmp(type, "END_ROOM")==0){
			roomArray[getRoomIndex(c, roomArray)].TYPE = 2;
			END = getRoomIndex(c, roomArray);
		}
		if(strcmp(type, "MID_ROOM")==0) roomArray[getRoomIndex(c, roomArray)].TYPE = 0;
        //printf("ROOM TYPE%s\n", type);
	}
    fclose(fptr);
}


void readFiles(char* newestDirName, struct room* roomArray){
    DIR *d;
    int i = 0;
    struct dirent *dir;
    d = opendir(newestDirName);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){
			 //printf(" readFiles Name: %s\n", dir->d_name);

             readRoomFile(i, dir->d_name, newestDirName, roomArray);
             i++;
         }
            
        }
        closedir(d);
    }

    d = opendir(newestDirName);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            //dir->d_name;
            //printf("", dir->d_name);
            if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){
            readRoomConnections(dir->d_name, newestDirName, roomArray);
        }
        }
        closedir(d);
    }
}


void printRooms(struct room* roomArray){
    int j = 0; int i = 0;
	for(i = 0; i<7; i++){
		printf("%s\t%d\t%d   ", roomArray[i].name, roomArray[i].num_connections, roomArray[i].TYPE);

		for(j = 0; j<6; j++) {
            printf("%d", roomArray[i].connectionsarray[j]);
        }
    printf("\n");
	} 
}


void printConnections(int CURRENT, struct room* roomArray){
	int n = roomArray[CURRENT].num_connections;
    int i = 0;
    int total = n-1;
	for(i = 0; i<n;i++){
            int temp = roomArray[CURRENT].connectionsarray[i];
            if(i == total) {printf("%s.\n",roomArray[temp].name); break;}
			printf("%s, ", roomArray[temp].name);
	}
}

void printTime(){
	time_t t ; 
    struct tm *tmp ; 
    char MY_TIME[80]; 
    time( &t ); 
    tmp = localtime( &t );
    strftime(MY_TIME, sizeof(MY_TIME), "%I:%M%p, %A, %B %d, %Y", tmp); 
    printf("%s\n\n", MY_TIME ); 
    FILE* fp = fopen("currentTime.txt", "w");
    fprintf(fp, "%s", MY_TIME);
    fclose(fp);
}

void getStartandEndRooms(struct room* roomArray){
    int i = 0;
    for(i = 0; i<7; i++){
        if(roomArray[i].TYPE == 1) START = i;
        if(roomArray[i].TYPE == 2) END = i;
    }
}

void runGame(struct room* roomArray){

	int CURRENT = START;
	char next[16];
	int steps = 0;
	int locs[100];
   // char buffer[100];
	int i = 0;
	while(1){
		printf("CURRENT LOCATION: %s\n", roomArray[CURRENT].name);
		printf("POSSIBLE CONNECTIONS: ");
        //printf("CURRENT: %d\n", CURRENT);
		printConnections(CURRENT, roomArray);
		printf("WHERE TO? >");
		fgets(next, 16, stdin); //the "enter" sends a newline character as well!!!
        next[strcspn(next, "\n")] = 0;


		printf("\n");
		if(strcmp(next, "time")==0) {
			printTime();
			continue;
		}
		int n = getRoomIndex(next, roomArray);
		if(n == -1){
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		}else{
			CURRENT = n;
			locs[steps++] = CURRENT;
		}
		if(roomArray[CURRENT].TYPE == 2) {
			printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
			for(i = 0; i<steps; i++){
				printf("%s\n", roomArray[locs[i]].name);
			}

			break;
		}
	}
}

int main()
{
	struct room* roomArray = (struct room*)malloc(sizeof(struct room)*7);
    initializeRooms(roomArray);

    char newestDirName[256]; // Holds the name of the newest dir that contains prefix
	getMostRecentDirectory(newestDirName);
    printf("Newest entry found is: %s\n", newestDirName);

    readFiles(newestDirName, roomArray);
    printRooms(roomArray);
    //printf("******************************************");
    runGame(roomArray);
    free(roomArray);

    return 0;
}
