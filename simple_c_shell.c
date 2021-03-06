#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
char path[1024]; //current
char cmd[256];
char cmdempty[256];
char* toks[10];
char* startmsg="default";
int parse(char* st, char** toks)
{
  char* tok = strtok(st," \t");
  int t = 0;
  while((tok != NULL) && (t < 10))
  {
    toks[t] = tok;
    t++;
    tok = strtok(NULL," \t");
  }
  toks[t] = NULL;
  return t;
}
void f_echo(char** st)
{
for(int i=1;i<10;i++)
{
  if(st[i])
{
write(1,st[i],strlen(st[i]));
write(1," ",1);
}
}
  write(1,"\n",1);
}
void cd(char* st)
{
    int r=chdir(st);
    if (r!=0) 
{
perror("Error");
return;
}
    for(int i=0;i<1024;i++)path[i]=0;
    getcwd(path,sizeof(path));
}
void pwd()
{
    write(1,path,sizeof(path));
    write(1,"\n",1);
}
void ls(char* pathx)
{
    struct dirent *de=NULL;
    DIR* d;
    if (pathx==NULL)d=opendir(path);
	else d=opendir(pathx);
    if(d == NULL)
    {
      write(1,"Couldn't open directory\n",sizeof("Couldn't open directory\n"));
    }
    else
    {
        while(de = readdir(d))
                {
                        char* name=de->d_name;
                        write(1,name,strlen(name));
                        write(1,"\n",1);
                }
    }
    closedir(d);
}
int main()
{
  int mainproc=1;
  char* user;
  user=getenv("USER");
  char* startmessage="Hello, ";
  write(1,startmessage,strlen(startmessage));
  write(1,user,strlen(user));
  startmessage="\nI can run echo,cd,ls,pwd,strmsg and call outer programs\n";
  write(1,startmessage,strlen(startmessage));
  toks[0]="";
  while ((strcmp(toks[0],"exit") != 0)){
    for (int i=0;i<10;i++) toks[i]="";
    for(int i=0;i<1024;i++)path[i]=0;
    getcwd(path,sizeof(path));
    if ( strcmp(startmsg,"default") != 0 ) write(1,startmsg,strlen(startmsg));
    else
    {
    write(1,path,sizeof(path));
    write(1,"@",1);
    write(1,user,sizeof(user));
    }
    write(1,">",1);
    int n = read(0,&cmd,256);
	toks[0]="";
        if(cmd[0]!='\n'){ //recognizing empty string
    cmd[n-1] = '\0';
    int t = parse(cmd, toks);
    if (strcmp(toks[0],"echo") == 0)
    {
      f_echo(toks);
    }
    else
    if (strcmp(toks[0],"cd") == 0)
    {
      cd(toks[1]);
    }
    else
    if (strcmp(toks[0],"pwd") == 0)
    {
      pwd();
    }
    else
    if (strcmp(toks[0],"strmsg") == 0)
    {
	char* warning="call with \"default\" to use basic \"path@user\" message\n";
	write(1,warning,strlen(warning));
	if(toks[1]!=NULL)startmsg=toks[1];
		else write(1,"error\n",6);
    }
    else
    if (strcmp(toks[0],"ls") == 0)
    {
	if ( sizeof(toks)>1) ls(toks[1]);
	else ls(NULL);
    }
    else
    {
      int p=fork();
      if (p==0)
      {
        execvp(toks[0],toks);
        perror("Unknown command");
	exit(1);
      }
      else if (p>0)
      {
        wait(p);
	write(1,"\n",1);
      }
    }
  }
  };
  return 0;
}
