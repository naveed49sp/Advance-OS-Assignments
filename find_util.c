// This is findv01. It will search a file or directory based on its name
// Do provide four command line arguments i.e. path -name filename.abc 

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

extern int errno;
void traverse_dir(char *);
int is_dir(char *);


int main(int argc, char *argv[]){
	DIR * dp;
	struct dirent *entry;
	bool found = false;
	
	if (argc != 4){
	printf("Insufficient commmand line arguments...");
	exit(1);
	}

if ((dp = opendir(argv[1])) == NULL){
	fprintf(stderr, "Cannot open directory:%s\n",argv[1]);
	exit(1);
	}
	
errno = 0;
while ((entry = readdir(dp)) != NULL){
	if(entry == NULL && errno != 0){
		perror("readdir failed");
		exit(1);
	}
	else{
		if(strcmp(entry->d_name,argv[3]) == 0){
			if(!is_dir(argv[3])){ //Third argument is a file
				printf("File:%s\n", entry->d_name);
			}
			else{
				traverse_dir(argv[3]);
			}
			found = true;
		}
	}
}
	
	if(!found){
		printf("No such file or directory\n");
	}
	closedir(dp);
	return 0;
}

void traverse_dir(char * dir){
	char new_dir[100]; // new path variable for nested traverse
    struct dirent *entry;
    DIR *dp = opendir(dir);
    if (dp == NULL){
    	fprintf(stderr, "Cannot open directory:%s\n",dir);
      	return;
	}
	errno = 0;    
    while ((entry = readdir(dp)) != NULL){
		if(entry == NULL && errno != 0){
			perror("readdir failed");
			exit(1);
		}
		else{
        if (entry -> d_name[0] == '.' )    
        	continue;        
        printf("%s/%s\n",dir, entry->d_name);
        // nested traverse
        strcpy(new_dir, dir);
        strcat(new_dir, "/");
        strcat(new_dir, entry->d_name);
        if (is_dir(new_dir))
        	traverse_dir(new_dir);        		 
		}
    }
}


int is_dir(char *file){
    struct stat buf;
    if(stat(file, &buf)<0){
    	perror("Error in stat");
    	exit (1);
	}
    return S_ISDIR(buf.st_mode); //S_ISDIR macro returns non-zero if the file is a directory
}
