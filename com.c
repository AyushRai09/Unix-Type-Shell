#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<math.h>
#define delimiters " \t\r\n\a;"
int global_flag=0;
char curr[1000];
int pro_id[10000]={0};
char pro_name[1000][1000];
int pro_count=1;
int made_jobs()
{
	int i, j, k,status;
	for(i=1;pro_name[i][0]!='\0';i++)
	{
		if(pro_id[i]!=-1)
		{
			if(waitpid(pro_id[i],&status,WNOHANG)!=0)
			{pro_id[i]=-1;}
		}
		if(pro_id[i]!=-1)
			printf("[%d] %s [%d]\n",i, pro_name[i], pro_id[i]);
	}
	return 1;
}
int made_kjob(char **args)
{
	int i, j, k;
	int sum=0,sum2=0,arg_length=0;
	for(i=0;args[i]!='\0';i++)
		arg_length++;

	if(arg_length==3)
	{

		int len=strlen(args[1]);

		//	printf("%d\n",len);
		i=len-1;
		j=0;
		while(i>=0)
		{
			sum=sum+(args[1][i]-'0')*pow(10,j);
			i--;j++;
		}
		//	printf("arg1 is:%d\n",sum);
		sum2=0;
		len=strlen(args[2]);
		i=len-1;
		j=0;
		while(i>=0)
		{
			sum2=sum2+(args[2][i]-'0')*pow(10,j);
			i--;j++;
		}
		//	printf("arg2 is:%d\n",sum);
		kill(pro_id[sum],sum2);
		return 1;
	}
	else
		printf("Insufficient number of aruments to command kjob\n");
}
int made_fg(char **args)
{
	int i, j, sum=0, len,wpid, loop_runner;
	len=strlen(args[1]);
	i=len-1;
	j=0;
	while(i>=0)
	{
		sum=sum+(args[1][i]-'0')*pow(10,j);
		i--;j++;
	}
	printf("%s\n",pro_name[sum]);
	do 
	{
		wpid = waitpid(pro_id[sum], &loop_runner, WUNTRACED);
	} while (!WIFEXITED(loop_runner) && !WIFSIGNALED(loop_runner));


	return 1;
}
int made_killallbg()
{
	int i, j, k;
	for(i=1;pro_name[i][0]!='\0';i++)
		kill(pro_id[i],9);
	return 1;
}

int made_proinfo(char **args)
{
	char str[30000];
	char proc[1000]="/proc/";
	char status[1000]="/status";
	strcat(proc,args[1]);
	strcat(proc,status);
	FILE *p;
	if((p=fopen(proc,"r"))==NULL){
		fprintf(stderr,"Unable to open the file\n");
		return 1;
	}
	while(fgets(str,70,p)!=NULL)
		puts(str);
	fclose(p);
	return 1;
}
int made_echo(char **args)
{
	int i,arg_count=0,j;
	for(i=1;args[i]!='\0';i++)
		arg_count++;
	int len=strlen(args[arg_count]);
	if(args[1][0]=='"')
	{	
		if(args[arg_count][len-1]=='"')
		{
			for(i=1;args[i]!='\0';i++)
			{
				for(j=0;args[i][j]!=' ' && args[i][j]!='\0';j++)
					if(args[i][j]!='"')
						printf("%c",args[i][j]);
				printf(" ");
			}
			printf("\n");

		}
		else
			fprintf(stderr,"Missing inverted linema at the extreme end of input string\n");
	}
	else if(args[arg_count][len-1]=='"')
	{
		if(args[1][0]=='"')
		{
			for(i=1;args[i]!='\0';i++)
			{
				for(j=0;args[i][j]!=' ' && args[i][j]!='\0';j++)
					if(args[i][j]!='"')
						printf("%c",args[i][j]);
				printf(" ");
			}
			printf("\n");

		}
		else
			fprintf(stderr,"Missing inverted linema at the extreme end of input string\n");
	}
	else
	{
		for(i=1;args[i]!='\0';i++)
			printf("%s ",args[i]);
		printf("\n");
	}
	return 1;
}
int made_getcwd(char **args)
{
	char arr[1000];
	if (getcwd(arr,sizeof(arr))!=NULL)
		printf("Current working directory is: %s\n",arr);
	return 1;
}

int cd(char **args)
{
	if (args[1] == NULL)
		fprintf(stderr,"Command cd needs argument to execute\n");
	else
	{
		char tilda[]="~";
		if(strcmp(args[1],tilda)==0)
		{	
			if(global_flag==0)
			{
				getcwd(curr,sizeof(curr));
				global_flag=1;
			}
			char args2[100][100];
			strcpy(args2[0],curr);
			chdir(args2[0]);
			return 1;
		}
		int var=chdir(args[1]);
		if(var!=0)
			fprintf(stderr,"No such directory exists\n");
	}
	return 1;
}
int  made_exit(char **args)
{
	return 0;
}

int  create_process(char **args, int linemand_flag)
{
	pid_t pid, wpid;
	int loop_runner,index=0,index2=0,index3=0;
	int i;
	char str[]="&";
	int wait_flag=0;
	char s1[]=">", s2[]="<", s3[]=">>";

	pid = fork();
	for(i=0;args[i]!='\0';i++)
		if(strcmp(args[i],str)==0)
		{args[i]='\0';wait_flag=1;}

	for(i=0;args[i]!='\0';i++)
	{
		if(strcmp(args[i],s1)==0)
			index=i+1;
		else if(strcmp(args[i],s2)==0)
			index2=i+1;
		else if(strcmp(args[i],s3)==0)
			index3=i+1;
	}
	if (pid == 0)
	{
		if(index !=0 || index2 != 0 || index3 !=0)//redirection case
		{
			if(index!=0)//out
			{
				int fd1=creat(args[index],0777);
				dup2(fd1,STDOUT_FILENO);
				close(fd1);
				args[index-1]='\0';
				//printf("args2:%s\n", args[2]);
			}

			if(index2!=0)//in
			{
				int fd0=open(args[index2],O_RDWR);
				dup2(fd0,STDIN_FILENO);
				close(fd0);
				args[index2-1]='\0';
			}
			if(index3 !=0)//append_out
			{
				int fd2=open(args[index3],O_APPEND | O_RDWR | O_CREAT, 0777);
				dup2(fd2, STDOUT_FILENO);
				printf("reached in linemand_flag=3 condition with linemand_flag:%d\n",linemand_flag);
				close(fd2);
				args[index3-1]='\0';
			}2
		} 
		if(execvp(args[0], args)==-1);
		fprintf(stderr,"Command not found\n");
		exit(EXIT_FAILURE);
	}
	else 
	{
		if(wait_flag==0)
			do 
			{
				wpid = waitpid(pid, &loop_runner, WUNTRACED);
			} while (!WIFEXITED(loop_runner) && !WIFSIGNALED(loop_runner));
		else
		{
			pro_id[pro_count]=pid;
			strcpy(pro_name[pro_count],args[0]);
			pro_count++;
			pro_name[pro_count][0]='\0';
		}
	}
	return 1;
}
int runpipe(int pfd[],char **args)
{
	int pid, i, j, k;
	pid=fork();
	char **args1, **args2, **args3;
	char str[]="|";

	args1=malloc(sizeof(args));
	args2=malloc(sizeof(args));

	for(i=0;strcmp(args[i],str)!=0;i++)
		args1[i]=args[i];
	for(k=0,j=i+1;args[j]!='\0';j++,k++)
		args2[k]=args[j];
	if(pid==0)
	{
		dup2(pfd[0],0);
		close(pfd[1]);
		execvp(args2[0],&args2[0]);
		//	printf("hi\n");
		perror(args2[0]);
	}
	else if(pid>0)
	{
		dup2(pfd[1],1);
		close(pfd[0]);
		execvp(args1[0],&args1[0]);
		//	printf("hi2\n");
		perror(args1[0]);
	}
	else if(pid <0)
	{
		//	printf("hi3\n");
		perror("fork");
		exit(1);
	}
	return 1;
}
int pipe_process(char **args)
{
	int pid, status,wpid,loop_runner;
	int fd[2];
	pipe(fd);
	pid=fork();
	//	printf("pid:%d\n",pid);
	if(pid==0)
	{
		//	printf("before func call\n");
		runpipe(fd,args);
		exit(0);
	}
	else if(pid>0)
	{
		//	printf("reached the parent below\n");
		//		pid=wait(&status);
		do 
		{
			wpid = waitpid(pid, &loop_runner, WUNTRACED);
		} while (!WIFEXITED(loop_runner) && !WIFSIGNALED(loop_runner));
		//	while((pid=wait(&status))!=-1)
		//		fprintf(stderr,"process %d exits with %d\n",pid,WEXITSTATUS(status));
	}
	else if(pid<0)
	{
		perror("fork");
		exit(1);
	}
	return 1;
}
int func (int in, int out, char *line)
{
	pid_t pid;
	int i, j, k;
	char *par[1000],*copy;
	copy=malloc(400);
	char s1[]=">",s2[]="<", s3[]=">>";

	char * parsed_argu = strtok(line," ");
	for(int i = 0; i <i 120011; i++) {   //space separated parsing of the pipe containing command.
		par[i] = parsed_argu;
		parsed_argu = strtok(NULL," ");
		if(par[i] == NULL) break;
	}
	for(i=0;par[i]!='\0';i++)
	{
		//	printf("hsdfdelll\n");
		if(strcmp(par[i],s1)==0) //case of '>'
		{
			//	printf("helll\n");
			copy=par[i+1];
			int fda=creat(par[i+1],O_RDWR);
			dup2(fda,out);
			close(fda);
			par[i]='\0';
		}
		else if(strcmp(par[i],s2)==0)
		{
			printf("hello\n");
			int fdb=open(par[i+1],O_RDWR);
			dup2(fdb,in);
			close(fdb);
			par[i]='\0';
		}
		else if(strcmp(par[i],s3)==0) // case of '>>'
		{
			int fdc=open(par[i+1],O_RDWR | O_APPEND | O_CREAT,0777);
			dup2(fdc,out);
			close(fdc);
			par[i]='\0';
		}
	}
	if ((pid = fork ()) == 0)
	{
		if (in != 0)
		{
			dup2 (in, 0);
			close (in);
		}

		if (out != 1)
		{
			dup2 (out, 1);
			close (out);
		}

		return execvp (par[0], par);
	}
	return pid;
}
int piping(char * lined)
{
	int duplicate1, duplicate2;
	int in, fd [2],i,length;
	char *par[1000];
	char line[169][281], s1[]=">",s2[]="<", s3[]=">>", *split;
	duplicate1 = dup(0);
	duplicate2 = dup(1);
	if(lined[strlen(lined)-1] == '\n') 
		lined[strlen(lined)-1] = '\0';
	split = strtok(lined,"|"); //parsing acc. to pipes
	while(split)
	{
		strcpy(line[length],split);
		split = strtok(NULL,"|");
		length++;   //finding no. of arguments based on pipes
	}
	in = 0;
	for (i = 0; i < length-1; ++i)  //
	{
		pipe (fd);
		func (in, fd [1], line[i]);
		close (fd [1]);
		in = fd [0];
	}
	char * parsed_argu = strtok(line[i]," "); 
	for(int i = 0; i < 1000; i++)
	{
		par[i] = parsed_argu;
		parsed_argu = strtok(NULL," ");
		if(par[i] == NULL) break;
	}
	for(i=0;par[i]!='\0';i++)
	{
		//	printf("hsdfdelll\n");
		if(strcmp(par[i],s1)==0) //case of '>'
		{
			//		printf("helll\n");
			//		copy=par[i+1];
			int fda=creat(par[i+1],O_RDWR);
			dup2(fda,1);
			close(fda);
			par[i]='\0';
		}
		else if(strcmp(par[i],s2)==0) // case of '<'
		{
			printf("hello\n");
			int fdb=open(par[i+1],O_RDWR);
			dup2(fdb,0);
			close(fdb);
			par[i]='\0';
		}
		else if(strcmp(par[i],s3)==0) // case of '>>'
		{
			int fdc=open(par[i+1],O_RDWR | O_APPEND | O_CREAT, 0777);
			dup2(fdc,1);
			close(fdc);
			par[i]='\0';
		}
	}
	//The last command we are not making it to go to func, instead we are executing it here only. Therefore need to do the below code.
	pid_t pid = fork();
	if (pid == -1)
	{
		dup2(duplicate1, 0);
		close(duplicate1);
		dup2(duplicate2,1);
		close(duplicate2);
		return 1;
	}
	else if (pid == 0)
	{
		if (in != 0)
			dup2 (in, 0);
		execvp(par[0], par);  
		dup2(duplicate1, 0);
		close(duplicate1);
		dup2(duplicate2,1);
		close(duplicate2);
		return 1;
	}
	else {
		int childStatus;
		waitpid(pid, &childStatus, 0);
		dup2(duplicate1, 0);
		close(duplicate1);
		dup2(duplicate2,1);
		close(duplicate2);
		return 1;
	}
}
