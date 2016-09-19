#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include "com.h"
#include<math.h>
#include<signal.h>
#define delimiters " \t\r\n\a;"
char *builtin_str[]={"cd","exit","pwd","echo","proinfo", "jobs", "kjob", "fg","killallbg"};
int (*builtin_func[]) (char **) = {&cd,&made_exit,&made_getcwd,&made_echo,&made_proinfo,&made_jobs,&made_kjob, &made_fg, &made_killallbg};
static volatile int loop_runner=1;
void intHandler(int dummy)
{
	loop_runner=0;
}
char * read_line(void)
{
	int var_size=1000;
	int bufsize =var_size;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	while (1) 
	{
		c = getchar();
		//if (c == EOF || c == '\n') {
		if (c=='\n'|| c==';')
		{
			buffer[position] = '\0';
			return buffer;
		}
		else 
			buffer[position] = c;

		position++;

		if (position >= bufsize) 
		{
			bufsize += var_size;
			buffer = realloc(buffer, bufsize);
		}
	}
}
char ** delimiter_func(char *line)
{		
		int var_size=64;
		int bufsize = var_size, position = 0;
		char **array = malloc(bufsize * sizeof(char*));
		char *token;

		token = strtok(line, delimiters);
		while (token != NULL) 
		{
			array[position] = token;
			position++;

			if (position >= bufsize)
			{
				bufsize = bufsize+var_size;
				array = realloc(array, bufsize * sizeof(char*));
			}

			token = strtok(NULL, delimiters);
		}
		array[position] = NULL;
		return array;
}
int search_string(char s1[], char s2[])
{
	int i, j, k;
	int len1=strlen(s1);
	int len2=strlen(s2);
	int flag=0;
	i=0;
	j=0;
	k=0;
	while(s1[i]!='\0')
	{
		j=0;k=i;
		while(s1[k]==s2[j] && s1[k]!='\0')
		{
//			printf("%c %c\n",s1[k], s2[j]);
			k++;j++;
		}
		
		if(j==len2)
		{flag=1;break;}
		i++;
	}
	if(flag==1)
		return k-1;
	else
		return 0;
		
}
int main(int argc, char **argv)
{
		int i, j, k,command_flag=0;
		char *line, *copy_line;
		char **args, **args2, pipeline_flag=0;
		int  prompt_flag,flag, builtin_flag;
		char host[10000];
		char *ayush;
		char path[10000];
		char tinda[10000];
		getcwd(tinda,sizeof(tinda));
		signal(SIGINT,intHandler);
		while(loop_runner)
		{
			command_flag=0,pipeline_flag=0;//to determin whether the command includes pipes, redirection character or the command is a normal one.
			builtin_flag=0;
			flag=0;
			ayush=getenv("USER");
			printf("%s@",ayush);                  //*********
			gethostname(host,sizeof(host));
			printf("%s:", host);                   //********
			int len=strlen(ayush);
			getcwd(path,sizeof(path));
			int variable=search_string(path, tinda);
			if(variable!=0)
				path[variable]='~';
			
			for(i=variable;path[i]!='\0';i++)
			printf("%c",path[i]);         //********
			printf(">> ");
			

			//*********
			line=read_line();
			for(i=0;line[i]!='\0';i++)
			{
				if(line[i]=='|')
				{pipeline_flag=1;break;}
			}
			if(pipeline_flag==1)
			{
				loop_runner=piping(line);
				continue;
			}
		//	copy_line=malloc(sizeof(line));
		//	strncpy(copy_line, line, sizeof(line));
		//	copy_line[sizeof(line)-1]='\0';

		//	for(i=0;line[i]!='\0';i++)
		//		copy_line[i]=line[i];
		//	printf("%c %c\n",copy_line[1], copy_line[0]);
		//	copy_line=read_line();
	        //	strcpy(copy_line,line);
		//	printf("%s\n",copy_line);
			for(i=0;line[i]!='\0';i++)
			{
				if(line[i]=='>' && line[i+1]=='>')
				{command_flag=3;break;}
				else if(line[i]=='>')
					command_flag=1;
				else if(line[i]=='<')
					command_flag=2;
				else if(line[i]=='|')
					command_flag=4;
			}
			args=delimiter_func(line);
	//		args2=delimiter_func(line);
	//		for(i=0;args2[i]!='\0';i++)
	//			printf("args2:%s\n",args2[i]);
		//	printf("%c\n",args2[2][3] );
		//	printf("prompt_flag:%d\n",prompt_flag);
		//	if(prompt_flag==1)
		//		printf("\n");
			//executing a command takes place in three steps, first it needs to be read, then tokenize(separating the program name and it's arguments and then executing it. Depending whether it's a builtin command or not, new process is either started or not started.

			// all the processes and builtin commands will return 1
			// which will keep this loop running and therefore will create the effect of 
			// terminal going on. However on exit
			// loop_runner=0 and thus it will break the loop and will end the program.
			if(args[0]==NULL)
			{loop_runner=1;continue;}//nothing given, just skip to the next line.
			for (i = 0; i < 9; i++) 
				if (strcmp(args[0], builtin_str[i]) == 0) 
				{
					if(strcmp("exit",builtin_str[i])==0)
						flag=1;
					builtin_flag=1;
					loop_runner=(*builtin_func[i])(args); //if the command is any one of the builtin command, go to that particular function
				}
		//	printf("loop_runner before:%d\n",loop_runner);
			if(flag==1)
				loop_runner=0;
//			printf("command_flag:%d\n",command_flag);
			if(builtin_flag==0)//this is for the exit condition, loop_runner came out to be 1 earlier which made the loop continue to run, so kept a flag.
			{
			//	if(command_flag==4)
			//	{
				//	printf("%s %s\n", args2[0], args2[1]);
				//	printf("headfsfds\n");
			//		loop_runner=pipe_process(args);
			//	}
			//	else
					loop_runner=create_process(args,command_flag);
			}
					
		}
		return 0;
}
