// This is the findv02, it will search files based on its types as well as names.
// Do provide four number of arguments i.e. path -name/-type filename/type letter. */
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

extern int errno;
void traverse_dir(char *);
void traverse_dir_type(char *, char *);
int is_dir(char *);
void search_on_name(char *, char *);
void search_on_type(char *, char* );


int main(int argc, char *argv[]){
	DIR * dp;
	struct dirent *entry;
		
	if (argc != 4){
		printf("Insufficient commmand line arguments...\n");
		exit(1);
	}

	if ((dp = opendir(argv[1])) == NULL){
		fprintf(stderr, "Cannot open directory:%s\n",argv[1]);
		exit(1);
	}
	
	errno = 0;
	while ((entry = readdir(dp)) != NULL){
		if(entry == NULL && errno != 0){
		perror("readdir failed\n");
		exit(1);
		}
	}
	
	if (strcmp(argv[2], "-name") == 0){
		search_on_name(argv[1], argv[3]);
	}
	else if( strcmp(argv[2], "-type") == 0){
		search_on_type(argv[1], argv[3]);
	}
	else{
		printf("Invalid Search option: Enter (-name|-type)\n");
		exit(1);
	}
	
	closedir(dp);
	return 0;
	
}

void search_on_name(char * dir, char* file){
	DIR * dp;
	struct dirent *entry;
	bool found = false;
	
	dp = opendir(dir); //Error handling is done in main function
	while ((entry = readdir(dp)) != NULL){
		if(strcmp(entry->d_name, file) == 0){
			if(!is_dir(file)){ //Third argument is a file
				printf("File:%s\n", entry->d_name);
			}
			else{
				traverse_dir(file);
			}
			found = true;
		}
	}
	if(!found){
		printf("No such file or directory.\n");
	}
	closedir(dp);
}

void search_on_type(char * dir, char * type_char){
	
	
	if (strcmp(type_char, "f") && strcmp(type_char, "r") && strcmp(type_char, "d") 
      					&& strcmp(type_char, "c") && strcmp(type_char, "b") 
	  					&& strcmp(type_char, "l") && strcmp(type_char, "s")){
	printf("Invalid Character: Type (f|r|d|c|b|l|s) \n ");
	exit(0);
	}
	else{
    	traverse_dir_type(dir,type_char);
	}
	
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
			perror("readdir failed\n");
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
    	perror("Error in stat\n");
    	exit (1);
	}
    return S_ISDIR(buf.st_mode); //S_ISDIR macro returns non-zero if the file is a directory
}

void traverse_dir_type(char *dir , char *file_type){
    char new_dir[100];
    struct dirent *entry;
    DIR *dp = opendir(dir);
    struct stat buf;

    while ((entry = readdir(dp)) != NULL){
        stat(entry->d_name, &buf);        
        if(!strcmp(file_type, "r") && S_ISREG(buf.st_mode)){
         printf("%s\n", entry->d_name);    
        }
        else if(!strcmp(file_type, "b") && S_ISBLK(buf.st_mode)){        
         printf("%s\n", entry->d_name);    
        }
        else if(!strcmp(file_type, "d") && S_ISDIR(buf.st_mode)){ //S_ISDIR macro returns non-zero if the file is a directory
         printf("%s\n", entry->d_name);
         // nested traverse
         //strcpy(new_dir, dir);
         //strcat(new_dir, "/");
         //if (entry -> d_name[0] == '.' )
         //	continue;
         //strcat(new_dir, entry->d_name);
         //traverse_dir_type(new_dir , file_type);		     
        }
        else if(!strcmp(file_type, "c") && S_ISCHR(buf.st_mode)){
         printf("%s\n", entry->d_name);    
        }
        else if(!strcmp(file_type, "f") && S_ISFIFO(buf.st_mode)){
         printf("%s\n", entry->d_name);    
        }
       else if(!strcmp(file_type, "s") && S_ISSOCK(buf.st_mode)){
         printf("%s\n", entry->d_name);    
        }
      else if(!strcmp(file_type, "l") && S_ISLNK(buf.st_mode)){
         printf("%s\n", entry->d_name);    
        }              
    }
    closedir(dp);
}
