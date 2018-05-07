#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>  
#include <string.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>  

#define EXE	"app"
#define PATH	"/home/ubuntu/"
#define RSLOG	"/tmp/zstart.log"
#define ZBLOG	"/tmp/zigbee.log"

#define LOG_Print(format,...) do{ \
		FILE *fd = fopen(RSLOG,"a"); \
		if(fd == NULL) exit(1); \
		time_t timep;	struct tm *p; \
		time(&timep);	p = gmtime(&timep);\
		fprintf(fd,"%4d/%.2d/%.2d %.2d:%.2d:%.2d :\t"format"",1900+p->tm_year,p->tm_mon+1,p->tm_mday,p->tm_hour+8,p->tm_min,p->tm_sec,## __VA_ARGS__); \
		fclose(fd); \
		}while(0)
#ifndef LOG_Print
#define LOG_Print(format,...) printf(""format"",## __VA_ARGS__)
#endif

#define DAEMON_MODE

#ifdef DAEMON_MODE
void ini_daemon();
#endif
unsigned long get_file_length(const char *file_name);

int main(int argc,char *argv[])
{
#ifdef DAEMON_MODE
	ini_daemon();
#endif

	char *argv_execv[]={EXE, NULL, NULL};
	//char tempbuff[256] = {0};
	int ret, i, status;
	FILE *fd;
	pid_t rpid;
	while(1){
		rpid = fork();
		if(rpid < 0){
			printf("Err on fork\n");
			continue;
		}
		if(rpid == 0){
			while(1)
			{
			char *argv_execv[]={EXE, NULL, NULL};
			char tmp[512] = "";
			sleep(6);
			FILE *fd = popen("ps -ef | grep app","r");
			fgets(tmp, 512, fd);
			pclose(fd);
			if(tmp[19] == '1'){
				printf("Notice: %s",tmp);
			}
			if(tmp[19] != '1'){
				LOG_Print("Notice: Starting %s ... \n",EXE);
				//sprintf(tempbuff,PATH"/"EXE);
				if(execv(PATH"/"EXE, argv_execv) < 0){
					printf("Err on execv\n");
					continue;
				}
				break;
			}
			}
			exit(0);
		}
		if(rpid > 0){
			rpid = wait(&status);
		}
	}
	return 0;
}


#ifdef DAEMON_MODE
void ini_daemon()
{
	pid_t pid;
	int i;
	pid = fork();
	if(pid < 0){
		exit(1);
	}else if(pid > 0){
		exit(0);
	}
	setsid();
	chdir("/");
	umask(0);
	/*
	for(i = 0; i < getdtablesize(); i++){
		close(i) ;
	}
	*/
										
}
#endif

unsigned long get_file_length(const char *file_name)
{
	struct stat tmp;
	if(stat(file_name, &tmp) < 0){
		return -1;
	}
	return (unsigned long)tmp.st_size;
}
