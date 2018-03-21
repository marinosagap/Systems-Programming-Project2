#include "header.h"
int pool_num=0;
int JOBS_NUM=0;
char path[MSGSIZE];
int MAX_JOBS;
int SHUTDOWN=0;//global metablhth pou mas deixnei an egine shutdown apo ton console
char * getNextToken (char * line,const char *delim){
    return strtok(line, delim);
}
char *fifo = "myfifo";
void show_active_finished(List * pool_list, int active)//synarthsh pou stelnei sta pool gia na brei ola ta active  h ola ta finished jobs
{												//an h metablhth active ==1 tote theloume ta active alliws ta finished
	char msgbuf[MSGSIZE+1];
	memset(msgbuf,'\0',MSGSIZE+1);
	if(active  == 1 )
		sprintf(msgbuf,"show-active");
	else 
		sprintf(msgbuf,"show-finished");
	list_node * temp = pool_list->start;
	while(temp!=NULL)
	{
		if(temp->pool_id !=-1)
		{
			while( (write(temp->fd_write,msgbuf, MSGSIZE+1) )<0);//stelnoume se ola ta pools to mhnyma show-active
		}
		temp= temp->next;
	}
}
void status_fun2(List * pool_list,int type_of_status,int fd_console,int time_duration)
{
	char msgbuf[MSGSIZE+1];
	list_node * temp = pool_list->start;
	int counter=0;
	while(temp!= NULL)
	{
		int i ;
		for(i=0;i<temp->number_jobs;i++)
		{
			counter++;
			//printf("AAAAAAAAAAAAAAAAAAAAAAAAAA\n");
			fflush(stdout);
			///if(type_of_status ==0)//an theloume geniko status
			{
				if(temp->job_array[i] == -1 && (type_of_status ==0 || type_of_status== 1) ) //gia to status-all kai show-finished
				{

					time_t seconds;
					seconds = time(NULL);
					if(time_duration!=-1)
					{

						//printf("FINISHEDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\n");
						if(seconds-temp->seconds_array[i]  < time_duration)
						{
							memset(msgbuf,'\0',MSGSIZE+1);
							sprintf(msgbuf,"status %d : Finished",counter);
							write(fd_console,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job
					
						}
					}
					else 
					{
						memset(msgbuf,'\0',MSGSIZE+1);
						sprintf(msgbuf,"status %d : Finished",counter);
						write(fd_console,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job
					
					}
				}
				else if(temp->job_array[i]>0 &&(type_of_status ==0 || type_of_status == 2))//gia to status-all kai show-active
				{
					//printf("ACTIVEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
					time_t seconds;
					seconds = time(NULL);
					if(time_duration!=-1)
					{
						
						if(seconds-temp->seconds_array[i]  < time_duration)
						{
							memset(msgbuf,'\0',MSGSIZE+1);
							sprintf(msgbuf,"status %d : Active (running for %ld sec)",temp->job_array[i],seconds-temp->seconds_array[i]);
							write(fd_console,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job
						}

					}
					else
					{
						memset(msgbuf,'\0',MSGSIZE+1);
						sprintf(msgbuf,"status %d : Active (running for %ld sec)",temp->job_array[i],seconds-temp->seconds_array[i]);
						write(fd_console,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job
					}	
				}
				else if(temp->job_array[i] ==-2 && type_of_status ==0 )
				{
					time_t seconds;
					seconds = time(NULL);
					if(time_duration!=-1)
					{
						
						if(seconds-temp->seconds_array[i]  < time_duration)
						{	
							memset(msgbuf,'\0',MSGSIZE+1);
							sprintf(msgbuf,"status %d : Suspended",counter);
							write(fd_console,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job
						}

					}
					else
					{
							memset(msgbuf,'\0',MSGSIZE+1);
							sprintf(msgbuf,"status %d : Suspended",counter);
							write(fd_console,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job
					}
				}
				else if(temp->job_array[i] ==-3 && type_of_status ==0 )
				{
					time_t seconds;
					seconds = time(NULL);
					if(time_duration!=-1)
					{
						
						if(seconds-temp->seconds_array[i]  < time_duration)
						{	
							memset(msgbuf,'\0',MSGSIZE+1);
							sprintf(msgbuf,"status %d : Terminated by Signal",counter);
							write(fd_console,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job
						}

					}
					else
					{
							memset(msgbuf,'\0',MSGSIZE+1);
							sprintf(msgbuf,"status %d : Terminated by Signal",counter);
							write(fd_console,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job
					}
				}
			}

		}
		temp = temp->next;
	}
}
void single_status(int job_name, List *pool_list,int fd_console)
{	
	char msgbuf[MSGSIZE+1];
	memset(msgbuf,'\0',MSGSIZE+1);

	if(job_name > JOBS_NUM)
	{

		sprintf(msgbuf,"job %d Doesnt exist\n",job_name);
		write(fd_console,msgbuf, MSGSIZE+1);//epistrefei mynhma ston console oti edwse lathos arithmo
	}

	int pool_to_send_status = job_name/MAX_JOBS;

	if((job_name % MAX_JOBS) >0)pool_to_send_status ++;
	int i;
	list_node * temp = pool_list->start; 
	for(i =1;i<pool_to_send_status ;i++)//gia kathe pool epikoinwnia 
	{
		if(temp->next != NULL)temp= temp->next;
	}
	int job_array_place = job_name - (pool_to_send_status -1)*MAX_JOBS;
	job_array_place--;
	printf("COORD : job_array_place = %d  , pool_id = %d , i == %d \n\n",job_array_place,pool_to_send_status,i);


	if(temp->job_array[job_array_place] == -1 )
	{
		memset(msgbuf,'\0',MSGSIZE+1);
		sprintf(msgbuf,"status %d : Finished",job_name);
		write(fd_console,msgbuf,MSGSIZE+1);  
	}
	else if(temp->job_array[job_array_place]>0)
	{
		time_t seconds;
		seconds = time(NULL);
		memset(msgbuf,'\0',MSGSIZE+1);
		sprintf(msgbuf,"status %d : Active (running for %ld sec)",job_name,seconds-temp->seconds_array[job_array_place]);
		write(fd_console,msgbuf,MSGSIZE+1);  
	}
	else if(temp->job_array[job_array_place] ==-2  )
	{
		memset(msgbuf,'\0',MSGSIZE+1);
		sprintf(msgbuf,"status %d : Suspended",job_name);
		write(fd_console,msgbuf,MSGSIZE+1);  
	}
	else if(temp->job_array[job_array_place] ==-3 )
	{
		memset(msgbuf,'\0',MSGSIZE+1);
		sprintf(msgbuf,"status %d : Terminated by Signal",job_name);
		write(fd_console,msgbuf,MSGSIZE+1);  
	}

}
void status_fun(int job_name,int fd_in,List * pool_list){//synarthsh pou stelnei gia ena 
	char temp[MSGSIZE+1];
	char msgbuf[MSGSIZE+1];
	memset(temp,'\0',MSGSIZE+1);
	if(job_name > JOBS_NUM)
			{
				sprintf(temp,"job %d Doesnt exist\n",job_name);
				write(fd_in,temp, MSGSIZE+1);//epistrefei mynhma ston console oti edwse lathos arithmo
				
			}
			else
			{	
				memset(msgbuf,'\0',MSGSIZE+1);
				sprintf(msgbuf,"status %d",job_name);
				if( (coord_send_status(pool_list, job_name,msgbuf) ) ==0)//an to sygkekrimmeno pool ths job ayths einai done 
				{
					sprintf(temp,"job : %d , status : Finished\n",job_name);
					write(fd_in,temp, MSGSIZE+1);//stelnei oti egine finished


				}
				else
				{
					/*while((read))
					if(!memcmp(msgbuf,"status",strlen("status")))//an tou erthei h apanthsh se ena status apo to pool
					{

						int j=0 ;
						while(msgbuf[j]!='\0')putchar(msgbuf[j++]);//ektypwnw olo to msgbuf
						putchar('\n');
						while( ( write(console_fd_in,msgbuf, MSGSIZE+1)  ) <0) ;
						fflush(stdout);
					}*/
				}
				//else to stelnei sto katallhlo pool kai apanta to pool analoga

				//write(fd_in,temp, MSGSIZE+1);
			}
}

void pool_numofjobs(List * pool_list,int fd_console)
{
	list_node * temp = pool_list->start;
	char msgbuf[MSGSIZE+1];

	int pool_num = 0;
	while(temp!= NULL)
	{
		int i;
		pool_num++;
		int active_counter = 0;
		for(i =0;i<temp->number_jobs;i++)
		{
			if(temp->job_array[i]>0)//an einai acivve
				active_counter++;
		}
		if(active_counter>0)
		{
			memset(msgbuf,'\0',MSGSIZE+1);
			sprintf(msgbuf,"%d %d",pool_num,active_counter);
			while((write(fd_console,msgbuf,MSGSIZE+1)) < 0);
		}
		temp = temp->next;
	}
}
void send_signal_to_job(List * pool_list,int job_name,int fd_console,int signal)
{
	char msgbuf[MSGSIZE+1];
	if(job_name > JOBS_NUM)
	{

		sprintf(msgbuf,"job %d Doesnt exist\n",job_name);
		write(fd_console,msgbuf, MSGSIZE+1);//epistrefei mynhma ston console oti edwse lathos arithmo
		return ;
	}

	int pool_to_send_status = job_name/MAX_JOBS;

	if((job_name % MAX_JOBS) >0)pool_to_send_status ++;
	int i;
	list_node * temp = pool_list->start; 
	for(i =1;i<pool_to_send_status ;i++)//gia kathe pool epikoinwnia 
	{
		if(temp->next != NULL)temp= temp->next;
	}
	int job_array_place = job_name - (pool_to_send_status -1)*MAX_JOBS;
	job_array_place--;
	printf("COORD : job_array_place = %d  , pool_id = %d , i == %d \n\n",job_array_place,temp->pool_id,i);
	if(temp->job_array[job_array_place] == -1 )//an to job einai finished den tou stelnoume kapoio shma
	{
		memset(msgbuf,'\0',MSGSIZE+1);
		sprintf(msgbuf,"JobID %d : Finished",job_name);
		write(fd_console,msgbuf,MSGSIZE+1);  
	}
	else if(temp->job_array[job_array_place]>0)//an to job einai active
	{
		
		if(signal == 19)
			kill(temp->pid_array[job_array_place],signal);
		memset(msgbuf,'\0',MSGSIZE+1);
		if(signal == 19)sprintf(msgbuf,"Sent suspend signal to JobID %d  with pid %d",job_name,temp->pid_array[job_array_place]);
		if(signal == 18)sprintf(msgbuf,"JobID %d  with pid %d is already running",job_name,temp->pid_array[job_array_place]);
		write(fd_console,msgbuf,MSGSIZE+1);  
	}
	else if(temp->job_array[job_array_place] ==-2  )//an to job einai suspended
	{
		if(signal == 18)
			kill(temp->pid_array[job_array_place] ,signal);
		memset(msgbuf,'\0',MSGSIZE+1);
		if(signal ==19)sprintf(msgbuf,"JobID %d : already Suspended",job_name);
		if(signal ==18)sprintf(msgbuf,"Sent resume signal to JobID %d",job_name);
		write(fd_console,msgbuf,MSGSIZE+1);  
	}
	else if(temp->job_array[job_array_place] ==-3  )//an to job exei ginei signal killed terminated 
	{
		memset(msgbuf,'\0',MSGSIZE+1);
		sprintf(msgbuf,"JobID %d : Terminated by a Signal ",job_name);
		write(fd_console,msgbuf,MSGSIZE+1);  

	}

}
void kill_pools(List * pool_list,int signal) //stelnoume SIGKILL se ola ta pools
{

	list_node * temp = pool_list->start ;
	while(temp != NULL)
	{
		kill(temp->pool_pid,signal);//stelnw SIGTERM sta pools
		temp = temp->next;
	}
}
void kill_jobs(List * pool_list,int signal) //stelnoume SIGKILL se ola ta jobs
{
	list_node * temp = pool_list->start ;
	while(temp != NULL)
	{
		int i ;
		for(i = 0;i< temp->number_jobs;i++)
		{
			if(temp->pid_array>0)
				kill(temp->pid_array[i],signal);
		}
		temp = temp->next;
	}
}

void shut_down(List * pool_list,int signal,int fd_console)
{
	list_node * temp = pool_list->start;
			int active_counter = 0;

	while(temp != NULL)
	{	
		int counter =0;
		int i ;
		for(i =0;i< temp->number_jobs;i++)
		{
			if(temp->job_array[i]==-2 || temp->job_array[i] >0)//metraw oles tis active kai tis signaled terminated
				counter ++;
		}
		active_counter+= counter;
		printf("COORD: shuting pool with pid : %d\n",temp->pool_pid);
		kill(temp->pool_pid,signal);//stelnw SIGTERM sta pools
		temp= temp->next;
	}
	char msgbuf[MSGSIZE+1];
	memset(msgbuf,'\0',MSGSIZE+1);
	sprintf(msgbuf,"Served %d jobs, %d were still in progress",JOBS_NUM,active_counter);//apantw ston console
	while((write(fd_console,msgbuf,MSGSIZE+1)) <0);

}
List * global_pool_list ;

int time_to_exit(List * pool_list)//synarthsh pou mas deixnei an prepei o coord na kanei exit
{
	if(SHUTDOWN)
	{
		list_node * temp = pool_list->start;
		while(temp!= NULL)
		{
			if(temp->pool_id!= -1) return 0; //an estw kai ena pool einai akoma zwntano tote den prepei na oloklhrwsei

			temp = temp->next;
		}
		return 1;//prepei na oloklhrwsei
	}
	else return 0;
}

int main(int argc, char *argv[]){
	int type_of_status=0;//default to 0 gia status,gia finished 1 ,gia active 2
	int fd_in,fd_out, i, number_of_jobs;
	char msgbuf[MSGSIZE+1];
	char write_to[MSGSIZE],read_from[MSGSIZE];

	if (argc!=9) { printf("lathos plithos orismatwn\n"); exit(1); }
	for(i =1;i<9;i++)
	{
		if(!strcmp(argv[i] , "-l") )//path
		{
			strcpy(path,argv[++i]);
		}
		if(!strcmp(argv[i] , "-n") )
		{
			number_of_jobs = atoi(argv[++i]);//megistos arithmos ergasiwn pou mporei na ekteleitai apo kathe pool
		}
		if(!strcmp(argv[i] , "-r") ) //jms_in
		{
			strcpy(read_from,argv[++i]);
		}
		if(!strcmp(argv[i] , "-w") ) //jms_out
		{
			strcpy(write_to,argv[++i]);
		}
	}
	
	MAX_JOBS = number_of_jobs;

	 int result = mkdir(path, 0777);
	 if(result ==-1)
	 {
	 	perror("ERROR MKDIR\n");
	 	exit(1);
	 }

	List * pool_list;//lista me ta pools
	pool_list = create_list();
	global_pool_list = pool_list;

	if ( mkfifo(write_to, 0666) == -1 ){//fifo me to jms_console
		if ( errno!=EEXIST ) { perror("receiver: mkfifo"); exit(6); };
		}
	if ( mkfifo(read_from, 0666) == -1 ){
		if ( errno!=EEXIST ) { perror("receiver: mkfifo"); exit(6); };
		}

	if ( (fd_out=open(read_from,   O_RDWR| O_NONBLOCK)) < 0)	{ perror("fd_out file open error"); exit(1); }


		printf("waiting for the othes side ...\n");//gia na epityxei to open prepei na kanei open me dikaiwma read only to 
	while( (fd_in=open(write_to, O_WRONLY| O_NONBLOCK)) < 0);//{ perror("fd_in file open error"); exit(1); }

	for (;;)
	{
		char input[50];
		memset(input,'\0',50);
		memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror
		if((time_to_exit(pool_list) )==1 ) break;

		/*while(read(fd_out, msgbuf, MSGSIZE+1) < 0); {
			perror("problem in reading"); exit(5);
			}*/
		//int read = (read(fd_out, msgbuf, MSGSIZE+1));
		while(( read(fd_out, msgbuf, MSGSIZE+1) ) <0)//diavazei apo to console
		{												//oso den exei na epikoinwnhsei me ton console epikoinwnei me ta pools
		//	printf("\n\n\ncomunicating with poollsssssssssssssssssssssssssssssssssssssssss\n\n\n\n");
			//sleep(3);
			communicate_with_pools(pool_list,fd_in ,fd_out,type_of_status);
			if((time_to_exit(pool_list) )==1 ) break;

		}
		msgbuf[MSGSIZE] ='\0';
		printf("Message Received: %s\n", msgbuf);
		fflush(stdout);
		sscanf(msgbuf, "%s",input);
		//write(fd_in,msgbuf, MSGSIZE+1);//epistrefei mynhma ston console


		if(!memcmp(input,"exit",strlen("exit")))
		{
			write(fd_in,msgbuf, MSGSIZE+1);

			printf("JMS_coord exiting...\n");
			list_node * temp = pool_list->start;
			while(temp!= NULL)
			{
				if(temp->pool_id != -1)
				{
					printf("pool %d hasent exited yet\n\n",temp->pool_id);
					sleep(4);
				}
				temp = temp->next;
			}
			break;
		}
		else  if(! memcmp(input,"killall-jobs",strlen("killall-jobs")))//tou esteile o console :show-pools
		{
			memset(msgbuf,'\0',MSGSIZE+1);
			sprintf(msgbuf,"killall"); //stelnei ston console 
			while((write(fd_in,msgbuf,MSGSIZE+1)) <0);
		
			kill_jobs(pool_list,9);
			
		}
		else  if(! memcmp(input,"shutdown",strlen("shutdown")))//tou esteile o console :show-pools
		{
			SHUTDOWN=1;
			shut_down(pool_list,15,fd_in);
			/*memset(msgbuf,'\0',MSGSIZE+1);
			sprintf(msgbuf,"SHUTDOWN"); //stelnei ston console 
			while((write(fd_in,msgbuf,MSGSIZE+1)) <0);*/
		}
		else  if(! memcmp(input,"show-pools",strlen("show-pools")))//tou esteile o console :show-pools
		{
			memset(msgbuf,'\0',MSGSIZE+1);
			sprintf(msgbuf,"Pool & NumOfJobs:"); //stelnei ston console 
			while((write(fd_in,msgbuf,MSGSIZE+1)) <0);
			pool_numofjobs(pool_list,fd_in);

			memset(msgbuf,'\0',MSGSIZE+1);
			sprintf(msgbuf,"END Pool & NumOfJobs:"); //stelnei ston console 
			while((write(fd_in,msgbuf,MSGSIZE+1)) <0);

		}
		else if(!memcmp(input,"suspend",strlen("suspend")))
		{
			char temp1[MSGSIZE+1];

			memset(temp1,'\0',MSGSIZE+1);
			sscanf(msgbuf,"%s %s",input ,temp1);
			int job_num = atoi(temp1);

			send_signal_to_job(pool_list,job_num,fd_in,19);//19 einai to shma sigstop
			
		}
		else if(!memcmp(input,"resume",strlen("resume")))
		{
			char temp1[MSGSIZE+1];

			memset(temp1,'\0',MSGSIZE+1);
			sscanf(msgbuf,"%s %s",input ,temp1);
			int job_num = atoi(temp1);

			send_signal_to_job(pool_list,job_num,fd_in,18);//19 einai to shma sigstop
			
		}
		else  if(! memcmp(input,"show-active",strlen("show-active")))//tou esteile o console :show-active
		{
			type_of_status=2;
			sprintf(msgbuf,"Sending-active %d",JOBS_NUM);
			write(fd_in,msgbuf,MSGSIZE+1);
			status_fun2(pool_list,2,fd_in,-1);

			memset(msgbuf,'\0',MSGSIZE+1);
			sprintf(msgbuf,"End-show-active"); //stelnei ayto ws endeiksh oti teleiwse h status-all
			write(fd_in,msgbuf,MSGSIZE+1);
		}
		else  if(! memcmp(input,"show-finished",strlen("show-finished")))//tou esteile o console :show-active
		{
			type_of_status=1;
			sprintf(msgbuf,"Sending-finished %d",JOBS_NUM);
			write(fd_in,msgbuf,MSGSIZE+1);

			status_fun2(pool_list,1,fd_in,-1);

				memset(msgbuf,'\0',MSGSIZE+1);
			sprintf(msgbuf,"End-show-finished"); //stelnei ayto ws endeiksh oti teleiwse h status-all
			write(fd_in,msgbuf,MSGSIZE+1);
		}
		else if(! memcmp(input,"status-all",strlen("status-all")))//tou esteile o console :status-all
		{
			type_of_status=0;
			char temp[MSGSIZE+1];
			memset(temp,'\0',MSGSIZE+1);
			sprintf(temp,"Sending-status-all");
			write(fd_in,temp,MSGSIZE+1);
			/*int job_id;
			for(job_id=1 ; job_id<=JOBS_NUM;job_id++)
			{	
				status_fun(job_id,fd_in,pool_list);
			}*/
			char ** args ;
			args =  create_argument_list(msgbuf);
			int time_duration;
			if(args[0] ==NULL)//den dwthike timeduration
				time_duration =-1;
			else 
				time_duration = atoi(args[0]);
			//printf("aguments : %s ||  %s\n\n\n",args[0],args[1]);
			delete_argument_list(args);

			status_fun2( pool_list,0,fd_in,time_duration);

			memset(msgbuf,'\0',MSGSIZE+1);
			sprintf(msgbuf,"End-status-all"); //stelnei ayto ws endeiksh oti teleiwse h status-all
			write(fd_in,msgbuf,MSGSIZE+1);
		}
		else if(!memcmp(input,"status",strlen("status")))//an dextei apo ton console :status job_id
		{
			type_of_status=0;
			char temp[MSGSIZE+1];
			memset(temp,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror

			sscanf(msgbuf,"%s %s",input,temp);
			int job_name = atoi(temp);
			memset(temp,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror
			//sprintf(temp," job %d doesnt exist ",job_name);
		//	strcat(msgbuf,temp);
			single_status(job_name, pool_list,fd_in);
			
			printf("COORD :  telossssss_send status telos \n\n");


		}
		else if(!memcmp(input,"submit",strlen("submit")))//an dextei status o coord apo to console
		{
			//write(fd_in,msgbuf, MSGSIZE+1);//epistrefei mynhma ston console
			//se periptwsh pou prepei na dhmiourghthei kainourgio pool
			if(( coord_send_submit(pool_list,msgbuf) ) == 0 ) //an prepei na dhmiourghthei kainourgio pool 
			{
				printf("DHMIOURGIA NEOU POOL\n\n");
			//	sleep(2);
				pid_t pid ;
				list_node * new_node ;

				new_node = coord_work();//dhmiourgia twn fifo ka tou kainourgiou list_node/pool'
				
				pid = fork();//dhmiourgia enos pool
				if(pid > 0)//parent/jms_coord
				{
					/*memset(msgbuf,'\0',MSGSIZE+1);
					sprintf(msgbuf,"JobID: %d ,PID: %d",JOBS_NUM+1,pid);
					write(fd_in,msgbuf,MSGSIZE+1);
*/
					coord_work_open_descriptors(new_node);
					new_node->pool_pid = pid;

					insert_new(pool_list,new_node);//eisagwgh tou new_node sthn lista
					coord_send_submit(pool_list,msgbuf);//twra tha tou steilei to submit sto kanourgio pool afou mono ayto einai diathesimo
				}
				else if(pid == 0)//Child work/pool
				{	
					///signal(SIGTERM,sigterm_action);
					list_node* temp = copy_list_node_for_pool(new_node);
					delete_list(pool_list);//katastrwfh ths listas sto pool
					delete_list_node(new_node);
					//char entolh[MSGSIZE];
					//sscanf(msgbuf, "%s %s",input,entolh);
					//edw pairnoume ta argument gia thn exec

					//argument_list=create_argument_list(msgbuf);
					

					//signal(SIGTERM,sigterm_action);

					pool_communication(temp);

					//pool_function(temp,entolh,argument_list);
					
					delete_list_node(temp);
					
					exit(0);//edw kanei exit to pool
				}
				else //error with fork
				{
					perror("Error with fork\n\n");
					exit(1);
				}
			}
			else
			{
				/*memset(msgbuf,'\0',MSGSIZE+1);
				sprintf(msgbuf,"JobID: %d ,PID: %d",JOBS_NUM+1,pid);
				write(fd_in,msgbuf,MSGSIZE+1);
				*/	
			}			
			
		}
		else
		{
			write(fd_in,msgbuf, MSGSIZE+1);//epistrefei mynhma ston console

		}
		/*strcpy(msgbuf, "jms_coord: ");
		strcat(msgbuf, input);
		write(fd_in,msgbuf, MSGSIZE+1);//epistrefei mynhma ston console
	*/
		strcpy(msgbuf,"");
	
		communicate_with_pools(pool_list,fd_in,fd_out,type_of_status );
	}
	delete_list(pool_list);
	close(fd_in);
	close(fd_out);
	int j ;
	/*unlink(write_to);
	unlink(read_from);*/	
	printf("pool_num = %d \n\n",pool_num);
	for(j = 1;j<=pool_num;j++)
	{
		printf("deleting pools\n");
		char write[MSGSIZE] = "write_pool_";
		char read[MSGSIZE] = "read_pool_";
		char str[MSGSIZE];
		sprintf(str,"%d",j);
		strcat(read,str);
		strcat(write,str);
		/*unlink(read);
		unlink(write);*/
		remove(read);
		remove(write);
	}
	remove(read_from);
	remove(write_to);
	printf("COORD : Exits\n");
	fflush(stdout);
	exit(0);
}

