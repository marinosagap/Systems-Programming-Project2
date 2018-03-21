#include "header.h"


char *fifo = "myfifo";
char msgbuf[MSGSIZE+1];
int fd_in, fd_out;
void resend_message();
int main(int argc, char *argv[]){
	int  i, nwrite;
	char  write_to[MSGSIZE] ,read_from [MSGSIZE],operations_file[MSGSIZE];

	if(argc <5)
	{
		perror("Lathos plhthos orismatwn \n");
		exit(1);
	}
	for(i =1;i<argc;i++)
	{
		if(!strcmp(argv[i] , "-w") ) //write to jms_in
		{
			strcpy(write_to,argv[++i]);
		}
		if(!strcmp(argv[i] , "-r") ) //read from  jms_out
		{
			strcpy(read_from,argv[++i]);
		}
		if(!strcmp(argv[i] , "-o") ) //operations file
		{
			strcpy(operations_file,argv[++i]);
		}
	}
	//printf("write_to : %s  , read_from: %s , operations : %s \n\n",write_to,read_from, operations_file);
	if ( mkfifo(write_to, 0666) == -1 ){
		if ( errno!=EEXIST ) { perror("receiver: mkfifo"); exit(6); };
		}
	if ( mkfifo(read_from, 0666) == -1 ){
		if ( errno!=EEXIST ) { perror("receiver: mkfifo"); exit(6); };
		}
	if ( (fd_out=open(read_from,   O_RDWR| O_NONBLOCK)) < 0)	{ perror("fd_out file open error"); exit(1); }


		printf("waiting for the othes side ...\n");//gia na epityxei to open prepei na kanei open me dikaiwma read only to 
	while( (fd_in=open(write_to, O_WRONLY| O_NONBLOCK)) < 0);//{ perror("fd_in file open error"); exit(1); }

//grafoume sto fd_in kai diabazoume apo to fd_out
	
	FILE * opfile= stdin;
	if(argc ==7)//an dwthike operations file
	{
		if((opfile = fopen(operations_file, "r")) == NULL)
		{
			perror("error in opening file \n");
			exit(1);
		}
	}

	FILE * reading = opfile;
	while(1){
		if(feof(opfile))reading = stdin;
		memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror
		fgets(msgbuf,MSGSIZE+1,reading);
		
		//strcpy(msgbuf, str);
		if ((nwrite=write(fd_in, msgbuf, MSGSIZE+1)) == -1) //me to pou stelnei kati perimenei meta na diavasei
			{ perror("Error in Writing"); exit(2); }
		if(!memcmp(msgbuf,"exit",strlen("exit")))break;
		
		//memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror
		
		signal(SIGALRM , resend_message);
		
		alarm(20);//orizw ena alarm se periptwsh pou o console den parei apanthsh apo ton coord gia 20 sec 
		while( read(fd_out, msgbuf, MSGSIZE+1) < 0) ;
		alarm(0);//anairw to alarm dioti phre apanthsh apo tton console

		printf("Message Received: %s \n", msgbuf);
		fflush(stdout);
		
		
		if(!memcmp(msgbuf,"answer_submit",strlen("answer_submit")) )//phre apanthsh apo ton jms_coord gia to submit <job>
		{//h apanthsh einai ths morfhs : "answer_submit <job> <logikos arithmos ergasias> <pid>"
			int i=0 ;
			while(msgbuf[i]!='\0')putchar(msgbuf[i++]);//ektypwnw olo to msgbuf

		} 
		else if(!memcmp(msgbuf,"exit",strlen("exit")))break;
		/*else if(!memcmp(msgbuf,"status",strlen("status")))
		{
			
		}*/
		else if(! memcmp(msgbuf,"Sending-status-all",strlen("Sending-status-all")))
		{
			
			while(1)
			{
				memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror
				while( read(fd_out, msgbuf, MSGSIZE+1) < 0) ;
				printf("%s \n", msgbuf);
				fflush(stdout);
				if(!memcmp(msgbuf,"End-status-all",strlen("End-status-all")))break;
			}

			
		}
		else if(! memcmp(msgbuf,"Sending-active",strlen("Sending-active")))//edw perimenei na tou steilei o coord oles tis acrive jobs
		{
			while(1)
			{
				memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror
				while( read(fd_out, msgbuf, MSGSIZE+1) < 0) ;
				printf("%s \n", msgbuf);
				fflush(stdout);
				if(!memcmp(msgbuf,"End-show-active",strlen("End-show-active")))break;
			}
		}
		else if(! memcmp(msgbuf,"Sending-finished",strlen("Sending-finished"))) //edw perimenei na tou epistrepsei o coord oles tis finished jobs
		{
			
			while(1)
			{
				memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror
				while( read(fd_out, msgbuf, MSGSIZE+1) < 0) ;
				printf("%s \n", msgbuf);
				fflush(stdout);
				if(!memcmp(msgbuf,"End-show-finished",strlen("End-show-finished")))break;
			}

			
		}
		else if(!memcmp(msgbuf,"Pool & NumOfJobs:",strlen("Pool & NumOfJobs:")))
		{
			printf("Pool & NumOfJobs:\n");
			while(1)
			{
				memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror
				while( read(fd_out, msgbuf, MSGSIZE+1) < 0) ;
				printf("%s \n", msgbuf);
				fflush(stdout);
				if(!memcmp(msgbuf,"END Pool & NumOfJobs:",strlen("END Pool & NumOfJobs:")))break;
		
			}
		}
		fflush(stdout);

	}	
	//sleep(15);
	fclose(opfile);	
	close(fd_out);
	close(fd_in);
	
		exit(0);
}
		
void resend_message()//synarthsh pou ksanastelnei to telaiyteo message 
{
	signal(SIGALRM , resend_message);//reestablish handler

	printf("\nRESENDING MESSAGEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE: %s\n\n",msgbuf);
	while( (write(fd_in, msgbuf, MSGSIZE+1) ) <0);
}
