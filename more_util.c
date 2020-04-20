// more v05 it get terminal size at runtime
// Show file percentage at bottom left corner
// Takes the input in non canonical and non-echo mode.
// search for a string in a file
// open file in vim on v

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

#define	LINELEN	512
char *file; //global variable

void do_more(FILE *);
int  get_input(FILE*, int);
int count_lines(FILE* );
int get_pagelen();
void canon_echo_off();
void canon_echo_on();
void search(FILE*, char *);
int main(int argc , char *argv[])
{
   int i=0;
   if (argc == 1){
   	  printf("This is help page ");
      do_more(stdin);
   }
   FILE * fp;
   while(++i < argc){
   	  file = argv[i];
      fp = fopen(file , "r");
      if (fp == NULL){
         perror("Can't open file");
         exit (1);
      }
      do_more(fp);
      fclose(fp);
   }
   canon_echo_on(); 
   return 0;
}

void do_more(FILE *fp)
{
   int PAGELEN = get_pagelen()-1;
   int num_of_lines = 0;
   int printed_lines = 0;   //They are never decremented
   int rv;
   char buffer[LINELEN];
   int file_size = count_lines(fp);
   int avg;
   int status;
   FILE* fp_tty = fopen("/dev//tty", "r");
   fseek(fp, 0, SEEK_SET); // current file offset 0 bytes in the beginning seek_set, seek_cur, seek_end
   while (fgets(buffer, LINELEN, fp)){
      fputs(buffer, stdout);
      num_of_lines++;
      printed_lines++;
      if (num_of_lines == PAGELEN){
      	 avg = printed_lines * 100/file_size;
      	 // Icanon and echo off
      	 canon_echo_off();
         rv = get_input(fp_tty, avg);		
         if (rv == 0){//user pressed q
            printf("\033[2K \033[1G");
            break;//
         }
         else if (rv == 1){//user pressed space bar
            printf("\033[2K \033[1G");
            num_of_lines -= PAGELEN;
         }
         else if (rv == 2){//user pressed return/enter
            printf("\033[2K \033[1G");
	        num_of_lines -= 1; //show one more line
            }
         else if (rv == 3){ // user want search
         	canon_echo_on();
         	printf("\033[2K \033[1G /"); //remove current line move cursor in column 1 and print /
         	char str[50];             
            fgets(str, 50, stdin); 
            search(fp,str);
            canon_echo_off();
		 }
		 else if (rv == 4){ // user pressed v
		 	printf("\033[2K \033[1G");
		 	if(fork() == 0){//it is child
		 		execlp("vim", "myvim", file, NULL);
			 }
			else{
      			wait(&status);
      			fclose(fp);
      			fp = fopen(file , "r");
      			do_more(fp);
   			}
		 }
         else if (rv == 5){ //invalid character
            printf("\033[2K \033[1G");
            break; 
         }
      }
  }
}

int get_input(FILE* cmdstream, int avg)
{
   int c;		
   printf("\033[7m --more--(%d%%) \033[m", avg);
    c = getc(cmdstream);
    if(c == 'q')
		return 0;
    else if ( c == ' ' )			
	 	return 1;
    else if ( c == '\n' )	
	 	return 2;
	else if (c == '/')
	  	return 3;
	else if (c == 'v')
		return 4;
	else
   		return 5;
}

int count_lines(FILE* fp)
{
	int count = 0; // Line counter (result)  
	char c; // To store a character read from file 
	
	// Extract characters from file and store in character c 
	for (c = getc(fp); c != EOF; c = getc(fp)) 
		if (c == '\n') // Increment count if this character is newline 
			count = count + 1; 
	return count; 
}

int get_pagelen()
{
	struct winsize wbuf;
	//1-open file descriptor of device file i.e. terminal,2- request, 3- request specific arguments
    if(ioctl(0, TIOCGWINSZ, &wbuf) == -1) 
	{
      perror("Error in ioctl");
      exit(1);
    }
    return wbuf.ws_row;
}

void canon_echo_off()
{
	//get attributes
    struct termios info;
    tcgetattr(0, &info); 
   	//make the echo bit off and set attributes
   	info.c_lflag &= ~ECHO;
   	info.c_lflag &= ~ICANON;       
   	tcsetattr(0, TCSANOW, &info);
}

void canon_echo_on()
{
	//get attributes
    struct termios info;
    tcgetattr(0, &info); 
   	//make the echo bit and cannoical bit on and set attributes
   	info.c_lflag |= ECHO;
   	info.c_lflag |= ICANON;       
	tcsetattr(0, TCSANOW, &info);
}

void search(FILE* fp, char *str)
{
	int line_num = 1;
  	bool found = false;
  	char temp[512];
  	fseek(fp, 0, SEEK_SET);
  	// with fgets we get content of the file line by line
  	// strstr will search for our string in the line
  	while(fgets(temp, 512, fp) != NULL) {
    	if((strstr(temp, str)) != NULL) {
    		printf("\033[1A \033[2K \033[1G \033[7m Found on line: %d \033[m\n", line_num);
      		found = true;
    	}
    	line_num++;
 	}

  if(!found) {
    printf("\033[1A \033[2K \033[1G \033[7m Pattern not found \033[m\n");
   }
}

