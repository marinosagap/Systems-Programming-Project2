#include "header.h"
int pool_num;
int JOBS_NUM;
struct sigaction old_sigaction;
list_node * global_pool_node;//global deikths sto pool_node ths listas pool_list
char path[MSGSIZE];
int MAX_JOBS;

void delete_argument_list(char **argument_list)//synarthsh pou apodesmebei ton xwro ths argument_list pou xrhsimopoieitai sthn execvp
{
	int k;
	for(k=0;k<30;k++)
	{
		if(argument_list[k] == NULL)break;
		free(argument_list[k]);
	}
	free(argument_list);
}

char ** create_argument_list(char * msgbuf)//synarthsh pou dexetai to string "ls -l -a -i " gia paradeigma kai mas epistrefei thn lista {"ls","-l","-a","-i",NULL}
{
	char ** argument_list;
	argument_list = malloc(30 * sizeof(char*));//gia 30 orismata 
	if(argument_list == NULL){perror("malloc error\n\n");exit(1);}
	const char s[] =" \n\t()<>|&;";
	char *token =strtok(msgbuf,s);
	int k=0;
	printf(" PRINTING ARGUMENT LIST\n\n");
	while( token != NULL)
	{
		token = strtok(NULL,s);
		if(token==NULL)	break;
		argument_list[k] = malloc( (MSGSIZE +1)*sizeof(char));
		if(argument_list[k] == NULL){perror("malloc error\n\n");exit(1);}
		memset(argument_list[k],'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror
		sscanf(token,"%s",argument_list[k]);
		//printf("%s\n",argument_list[k]);
		k++;
	//	sleep(1);
	}
	argument_list[k] = NULL;
	return argument_list;
}
List* create_list()//synarthsh pou dhmiourgei mia lista kai thn epistrefei 
{
	List * temp = (List*)malloc(sizeof(List));
	temp -> start = NULL;
	temp -> end = NULL;
	temp->number_of_pools = 0;
	return temp;
}

list_node * copy_list_node_for_pool(list_node* mynode)//synarthsh pou epistrefei ena antigrafo node tou mynode alla me anti
{
	list_node * temp = (list_node*)malloc(sizeof(list_node));
	if(temp == NULL){perror("Error malloc\n");exit(1);}


	memcpy(temp->write_to, mynode->read_from,MSGSIZE);
	memcpy(temp->read_from, mynode->write_to,MSGSIZE);

	//temp->fd_read = mynode->fd_write;
	//temp->fd_write = mynode->fd_read;
	temp->next =NULL;
	temp->pool_id = mynode->pool_id;
	temp->number_jobs = mynode->number_jobs;

	temp->sigterm_flag = mynode->sigterm_flag;
	temp->jobs_done= mynode->jobs_done;
	temp->pid_array = (int *)malloc(MAX_JOBS * sizeof(int));//pinakas me ta pid twn job tou pool
	if(temp->pid_array == NULL){perror("malloc error\n"); exit(1); }

	temp->job_array = (int *)malloc(MAX_JOBS * sizeof(int));//pinakas me ta pid twn job tou pool
	if(temp->job_array == NULL){perror("malloc error\n"); exit(1); }

	temp->seconds_array = (int *)malloc(MAX_JOBS * sizeof(int));//pinakas me ta pid twn job tou pool
	if(temp->seconds_array == NULL){perror("malloc error\n"); exit(1); }

	memcpy(temp->pid_array, mynode->pid_array,MAX_JOBS* sizeof(int));
	memcpy(temp->job_array, mynode->job_array,MAX_JOBS* sizeof(int));
	memcpy(temp->seconds_array, mynode->seconds_array,MAX_JOBS* sizeof(int));

	
	return temp;	
}
list_node*  find_available_pool(List * pool_list) //synarthsh pou briskei to pool gia to submit job
{//epistrefoume to pool unique id
	list_node * temp = pool_list->start;
	while(temp!=NULL)
	{
		if(temp->pool_id !=-1 && temp->number_jobs < MAX_JOBS) //an exei xwro kai gia to kainourgio job to pool tote koble
		{
			return temp; //epistrefoume to pool_id tou pool pou tha kanei to job
		}
		temp = temp->next;
	}
	return NULL; //se periptwsh pou ola ta pool einai gemata apo job
}

void pop_list_node(List * mylist, list_node * new_node) //synarthsh pou afairei kai katastrefei enan kombo apo thn lista 
{
	if(mylist == NULL || new_node == NULL || mylist->number_of_pools == 0){printf("NOTHING TO DELETE\n\n");}return;
	list_node * temp = mylist->start;
	//list_node * prev = NULL;
	if(mylist->number_of_pools == 1 ) //an exei mono ayton ton kombo h lista
	{
		if(temp->pool_id == new_node->pool_id)
		{
			mylist->start = NULL;
			mylist->end = NULL;
			delete_list_node(temp);
		}
	}
	int i ;
	for(i = 0;i < mylist->number_of_pools ; i++)
	{
		if(temp->pool_id == new_node->pool_id)
		{
			if(i==0)//an eimaste sthn arxh ths listas
			{
				mylist->start = mylist->start->next;
			}
			if(i == mylist->number_of_pools -1)//an eimaste sto telos
			{
			//	if(prev == NULL)//an h lista exei mono ayto to stoixeio
			}
		}
	}
	mylist->number_of_pools--;
}
void insert_new(List * mylist,list_node * new_node)
{
	mylist -> number_of_pools++;
	if(mylist->start == NULL)
	{
		mylist->start = new_node;
		mylist->end = mylist->start ;
//		printf("inserted in  start of list\n");
		return ;
	}

	mylist->end->next = new_node;
	mylist->end = mylist->end->next;
	mylist->end->next = NULL;
//	printf("H eisagwgh egine \n");
}

void delete_list(List * mylist )
{
	if(mylist == NULL)return ;
	list_node* temp1 = mylist->start;

	while(temp1 != NULL)
	{	
		
		list_node * temp2;
		if(temp1->next!=NULL) temp2 = temp1->next;
		else temp2 = NULL;
	    
		//temp ->next =NULL;
		delete_list_node(temp1);
		temp1 = temp2;
	}	
	free(mylist);
}
void delete_list_node(list_node * temp)
{
	printf("DELETING pool pool-> = %d\n\n",temp->pool_id);
	free(temp->seconds_array);
	free(temp->job_array);
	free(temp->pid_array);
	free(temp);
}


list_node * create_list_node()//synarthsh pou dhmiourgei ena list_node dhladh ena pool 
// mazi me ta non_blocking pipes tou px write_pool , read_pool
{
	list_node * temp_node = (list_node*)malloc(sizeof(list_node));

	if(temp_node == NULL){perror("malloc error\n"); exit(1); }
	char write_to[MSGSIZE],read_from[MSGSIZE];
	pool_num++;
	char str[MSGSIZE];
	sprintf(str,"%d",pool_num);
	strcpy(write_to,"write_pool_");
	strcpy(read_from,"read_pool_");

	strcat(write_to,str);
	strcat(read_from,str);
	temp_node->next =NULL;
	temp_node->pool_id = pool_num;
	strcpy(temp_node->write_to, write_to);
	strcpy(temp_node->read_from, read_from);

	temp_node->jobs_done=0;
	temp_node->number_jobs=0;
	temp_node -> sigterm_flag =0;
	temp_node->pid_array = (int *)malloc(MAX_JOBS * sizeof(int));//pinakas me ta pid twn job tou pool
	if(temp_node->pid_array == NULL){perror("malloc error\n"); exit(1); }

	temp_node->job_array = (int *)malloc(MAX_JOBS * sizeof(int));//pinakas me ta pid twn job tou pool
	if(temp_node->job_array == NULL){perror("malloc error\n"); exit(1); }
	
	temp_node->seconds_array = (int *)malloc(MAX_JOBS * sizeof(int));//pinakas me ta pid twn job tou pool
	if(temp_node->seconds_array == NULL){perror("malloc error\n"); exit(1); }
	

	int k ;
	for(k = 0;k<MAX_JOBS;k++)
	{
		temp_node->pid_array[k] = -1;//gia thn arxikopoihsh
		temp_node->job_array[k] = -1;//gia thn arxikopoihsh
		temp_node->seconds_array[k] =-1;
	}

	/*printf("unique id = %d  write_to = %s   read_from  = %s\n\n",temp_node->pool_id,write_to,read_from);
	printf("dhmiourgia kainourgiou named pipe gia to kainourgio pool\n\n");
	*///sleep(3);

	return temp_node;
}
void pool_end_of_pool(list_node * new_node)//synarthsh pou kaloume otan teleiwnei to pool kai stelnei mhnyma ston coord end_of pool
{
	char msgbuf[MSGSIZE+1];
	memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror

	write(new_node->fd_write,"end_of_pool", MSGSIZE+1);//stelnoume ston coord oti teleiwse h leitourgia tou pool
	printf("\t\t\tPOOL (unique_id = %d ): waiting for coord to reply\n",new_node->pool_id);
	//sleep(2);
	
	memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror

	while((read(new_node ->fd_read, msgbuf, MSGSIZE+1) )<0);//perimenoume na mas pei o coord oti elabe to mhnyma oti to pool oloklhrwnei 
	
	printf("\t\t\tPOOL : coord replied : ");
	int j=0 ;
	
	while(msgbuf[j]!='\0')putchar(msgbuf[j++]);//ektypwnw olo to msgbuf
	putchar('\n');
	fflush(stdout);	

	printf("\t\t\tPOOL : pid = %d exitingggggggggggg\n\n",getpid());
	delete_list_node(global_pool_node);
					
	exit(0);//edw kanei exit to pool
}
void pool_job_done(list_node* new_node,int i)//synarthsh pou stelnei mhnyma ston coord oti mia job oloklhrwthike
{
	char msgbuf[MSGSIZE+1];
	fflush(stdout);
	char temp[MSGSIZE+1];
	memset(temp,'\0',MSGSIZE+1);
	sprintf(temp,"job_num: %d , job_pid : %d , from pool:%d ",new_node->job_array[i],new_node->pid_array[i],new_node->pool_id);
	memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror

	memcpy(msgbuf,"job_done ",strlen("job_done "));
	strcat(msgbuf,temp);
	int j=0 ;
	printf("\t\t\tPOOL  : ");
	while(msgbuf[j]!='\0')putchar(msgbuf[j++]);//ektypwnw olo to msgbuf
	putchar('\n');
	fflush(stdout);
	new_node->job_array[i]=-1;

	write(new_node->fd_write,msgbuf,MSGSIZE+1);    //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job

	pool_send_node(new_node,msgbuf);//amesws meta to job done stelnoume sto pool kai to ananeomeno node

}
void sigterm_action(int signal)
{
	global_pool_node ->sigterm_flag = 1;//allazou
	//printf("\t\t\tPOOL : poolid : %d  , poolpid:  %d,  number_jobs = %d , jobs_done = %d\n\n",global_pool_node ->pool_id, global_pool_node ->pool_pid,global_pool_node ->number_jobs,global_pool_node ->jobs_done );
	int i ;
	for(i = 0;i< global_pool_node->number_jobs;i++)
	{
		kill(global_pool_node->pid_array[i] , 15); //to pool stelnei se ola ta jobs SIGTERM shma
	}
	pool_end_of_pool(global_pool_node); //termatismos tou pool
}
void pool_communication(list_node* new_node) //epikoinwnia pool me coord kai jobs
{	
	printf("\t\t\tPOOL : pool_communication\n\n");
	coord_work_open_descriptors(new_node);//anoigoume tous descriptors gia to pool -coord
	global_pool_node = new_node;
	static struct sigaction act;
	act.sa_handler = sigterm_action;
	sigfillset(&(act.sa_mask));
	sigaction(SIGTERM,&act,&old_sigaction);

	//for(i = 0;i< new_node->number_jobs;i++)
	while(new_node->jobs_done < MAX_JOBS ) //an den exoun oloklhrwthei akoma ola ta jobs 
	{
	     if(new_node -> jobs_done == new_node->number_jobs  && new_node->sigterm_flag ==1) break; // an dexteike shutdown dhladh shma sigterm ao ton coord

		//mila me ton coord
		char msgbuf[MSGSIZE+1];
		memset(msgbuf,'\0',MSGSIZE+1);
		if( (read(new_node ->fd_read, msgbuf, MSGSIZE+1) )>=0) //an diavasei kati apo ton coord
		{
			if(!memcmp(msgbuf,"submit",strlen("submit")))//an dextei submit apo ton coord gia submit job
			{
				if(new_node->number_jobs <MAX_JOBS)//an mporei na kanei kai alles douleies tote ektelei to job
					pool_submit_job(new_node, msgbuf);
			}
			else if(!memcmp(msgbuf,"show-active",strlen("show-active")))//an dextei submit apo ton coord gia submit job
			{
				int i ;
				for(i =0;i< new_node->number_jobs;i++)
				{
					if( new_node->job_array[i] >0)//tote einai active;
					{
						memset(msgbuf,'\0',MSGSIZE+1);
						sprintf(msgbuf,"active JOBID %d",i+1);
						write(new_node->fd_write,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job
					}
				}
			}
			else if(!memcmp(msgbuf,"show-finished",strlen("show-finished")))//an dextei submit apo ton coord gia submit job
			{
				int i ;
				for(i =0;i< new_node->number_jobs;i++)
				{
					if( new_node->job_array[i] ==-1)//tote einai active;
					{
						memset(msgbuf,'\0',MSGSIZE+1);
						sprintf(msgbuf,"finished JOBID %d",i+1);
						write(new_node->fd_write,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job
					}
				}
			}
			else if(!memcmp(msgbuf,"status",strlen("status"))) //an to pool dextei status
			{

				char temp1[MSGSIZE+1], temp2[MSGSIZE+1];
				printf("\t\t\tTO POOL : %d   dexthke status\n\n",new_node->pool_id);
				//sleep(2);
				sscanf(msgbuf,"%s%s",temp1,temp2);
				int job_name = atoi(temp2);
				//int job_array_place = job_name % new_node->pool_id;
				int job_array_place = job_name - (new_node->pool_id -1)*MAX_JOBS;
				job_array_place--;
				//int job_array_place = job_name/new_node->pool_id;
				//if( (job_name % new_node->pool_id ) >0 )job_array_place ++;

				printf("\t\t\tPOOL : job_array_place = %d\n\n",job_array_place);
			//	sleep(4);
				if(new_node->job_array[job_array_place] == -1)//an h job ayth einai Finished tote exei to -1 sto pinaka
				{
					memset(msgbuf,'\0',MSGSIZE+1);
					sprintf(msgbuf,"status %d : Finished",job_name);
					write(new_node->fd_write,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job

				}
				else if(new_node->job_array[job_array_place] == -2)//an exei ginei to sygkegkrimmeno job suspended
				{
					memset(msgbuf,'\0',MSGSIZE+1);
					sprintf(msgbuf,"status %d : Suspended",job_name);
					write(new_node->fd_write,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job

				}
				else
				{
					time_t seconds;
					seconds = time(NULL);
					memset(msgbuf,'\0',MSGSIZE+1);
					sprintf(msgbuf,"status %d : Active (running for %ld sec)",job_name,seconds - new_node->seconds_array[job_array_place] );
					write(new_node->fd_write,msgbuf,MSGSIZE+1);  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job

				}
			}
			else if(!memcmp(msgbuf,"send_node",strlen("send_node")))//an dextei submit apo ton coord gia submit job
			{
				pool_send_node(new_node,msgbuf);
			}
			

		}



		//mila sta paidia
		pid_t pid;
		int status;
		pid  = waitpid(-1, &status, WUNTRACED | WCONTINUED|WNOHANG);
		
		if(pid ==0 || pid ==-1)continue;//an den allakse tpt se kapoio paidi tote kanoume continue

        if (WIFEXITED(status) ) 
        {
            printf("\t\t\tPOOL exited, status=%d  pid  =%d  jobs_done = %d\n\n", WEXITSTATUS(status),pid,new_node->jobs_done+1);
        	//teleiwse to paidi - job
        	int job_id=-1;
            new_node->jobs_done++;
            int j ;
            for(j =0;j<MAX_JOBS;j++)//gia na vriskoume ton logiko arithmo tou job
            {
            	if(new_node->pid_array[j] == pid)
            	{
            		job_id = j;
            		//new_node->job_array[j]=-1; //to kanw meta sthn pool_job_done
            		break;
            	}


            }
           // if(job_id ==-1) new_node->jobs_done--;//ayto einai se periptwsh pou kanei wait ena paidi pou den yparxei to pid ston pinaka me tapid
           
            pool_job_done(new_node,job_id);//stelnoume mhnhma oti mia job oloklhrwthike
        } 
        else if (WIFSIGNALED(status))
        {
        	int j ;
        	int job_id;
        	new_node->jobs_done++;
            for(j =0;j<MAX_JOBS;j++)//gia na vriskoume ton logiko arithmo tou job
            {
            	if(new_node->pid_array[j] == pid)
            	{
            		job_id = j;
            		new_node->job_array[job_id]=-3;//pou shmainei oti exei ginei killed
            		//new_node->job_array[j]=-1; //to kanw meta sthn pool_job_done
            		break;
            	}


            }
            printf("\t\t\tPOOL: killed by signal %d\n", WTERMSIG(status));
            fflush(stdout);
            pool_send_node(new_node,msgbuf);

        } 
        else if (WIFSTOPPED(status))
        {
        	int j ;
        	int job_id;
            for(j =0;j<MAX_JOBS;j++)//gia na vriskoume ton logiko arithmo tou job
            {
            	if(new_node->pid_array[j] == pid)
            	{
            		job_id = j;
            		new_node->job_array[job_id]=-2;//pou shmainei oti exei ginei suspeended
            		//new_node->job_array[j]=-1; //to kanw meta sthn pool_job_done
            		break;
            	}


            }
			pool_send_node(new_node,msgbuf);

            printf("stopped by signal %d\n", WSTOPSIG(status));

        }
        else if (WIFCONTINUED(status))
        {
        	int j ;
        	int job_id;
            for(j =0;j<MAX_JOBS;j++)//gia na vriskoume ton logiko arithmo tou job
            {
            	if(new_node->pid_array[j] == pid)
            	{
            		job_id = j;
            		new_node->job_array[job_id]= job_id +1 + (new_node->pool_id-1)*MAX_JOBS ;//pou shmainei oti exei ginei suspeended
            		//new_node->job_array[j]=-1; //to kanw meta sthn pool_job_done
            		break;
            	}


            }
            pool_send_node(new_node,msgbuf);
         //   printf("continued\n");
        }
	}
	pool_end_of_pool(new_node);//synarthsh pou kaloume otan teleiwnei to pool kai stelnei mhnyma ston coord end_of pool

}
//h ananewsh ginetai meta apo submit kai job done ston coord
void pool_send_node(list_node * new_node, char * msgbuf)//synarthsh pou stelnei ston coord olo to node me tis kainourgies allages
{
	memset(msgbuf,'\0',MSGSIZE+1);
	sprintf(msgbuf,"sending_node");
	while( ( write(new_node->fd_write,msgbuf,MSGSIZE+1) ) <0) ;  //stelnoume munhma ston coord oti teleiwse to sygkekrimmeno job
	//stelnoume ston pool oti tou stelnoume oloklhro to node
	while( ( write(new_node->fd_write, (list_node*)new_node,sizeof(list_node))) <0 );
	while( ( write(new_node->fd_write, (int*)new_node->pid_array,MAX_JOBS * sizeof(int))) <0 ); //tou stelnw kai ton pinakas  pid_array
	while( ( write(new_node->fd_write, (int*)new_node->job_array,MAX_JOBS * sizeof(int))) <0 ); //tou stelnw kai ton pinaka job_array
	while( ( write(new_node->fd_write, (int*)new_node->seconds_array,MAX_JOBS * sizeof(int))) <0 ); //tou stelnw kai ton pinaka seconds_array

}
void pool_submit_job(list_node* new_node,char * msgbuf)//synarthsh pou ektelei ena job
{
	//char msgbuf[MSGSIZE+1];
	JOBS_NUM++;
	pid_t pid;
	new_node ->number_jobs++;//ayksanoume ton arithmo twn job pou trexoume
	pid = fork();
		//if(pid>0)break;
		if(pid >0)//father work /pool
		{
			memset(msgbuf,'\0',MSGSIZE+1);
			sprintf(msgbuf,"JOBID : %d , PID : %d",JOBS_NUM,pid);
			//printf("\t\t\tPOOL:sending msgbuf  =  %s  || to coord\n\n",msgbuf);sleep(3);
			while( ( write(new_node->fd_write,msgbuf, MSGSIZE+1))<0) {}//stelnei sto pool to pid ths diergasias
			new_node->pid_array[new_node->number_jobs-1] = pid;
			new_node->job_array[new_node->number_jobs-1] = JOBS_NUM;


		    time_t seconds;

		    seconds = time(NULL);
			new_node->seconds_array[new_node->number_jobs-1] = seconds;

			pool_send_node(new_node,msgbuf);

			//new_node->pid_array[new_node->number_jobs-1] = pid;//apothikeboume to pid
			printf("\t\t\tPOOL: job pid == %d i == %d \n\n\n\n",pid,new_node->number_jobs-1);
		}
		else if (pid == 0)//child work/job
		{
			sigaction(SIGTERM,&old_sigaction,NULL);
			char directory_name[MSGSIZE];	
			strcpy(directory_name,path);
			char temp[MSGSIZE];
			//time_t t = time(NULL);
			//struct tm tm = *localtime(&t);
			//if()
			char buff[100];
			 time_t now = time(NULL);
		    strftime (buff, 100, "%Y%m%d_%H%M%S", localtime(&now));
		   // printf ("JOB :/jms_sdi1400002_%s\n", buff);

			//printf("\t\t\tJOB :/jms_sdi1400002_%d_%d_%d %d %d_%d %d %d\n\n\n",JOBS_NUM,getpid(),tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
			//sprintf(temp,"/jms_sdi1400002_%d_%d_%d%d%d_%d%d%d",JOBS_NUM,getpid(),tm.tm_year+ 1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);	
			sprintf(temp,"/jms_sdi1400002_%d_%d_%s",JOBS_NUM,getpid(),buff);
			strcat(directory_name,temp);
			 int result = mkdir(directory_name, 0777);
			 if(result ==-1)
	 		 {
	 		 	perror("ERROR MKDIR\n");
	 			//sleep(3);
	 			exit(1);
	 			
	 		}
	 		char stdout_file[MSGSIZE];
	 		char stderr_file[MSGSIZE];
	 		sprintf(stdout_file,"/stdout_%d.txt",JOBS_NUM);
	 		char directory_name2[MSGSIZE];
	 		strcpy(directory_name2,directory_name);	

	 		strcat(directory_name,stdout_file);
	 		//int fd_out =open(directory_name,O_RDWR); 
	 		int  fd_out;
			if (( fd_out=open(directory_name ,O_CREAT|O_RDWR ,PERMS))==-1)
			{
				perror("creatingggggggggggggggggggggggggggggggggggggggg\n\n\n");
				exit (1);
			}

		 	sprintf(stderr_file,"/stderr_%d.txt",JOBS_NUM);
			strcat(directory_name2,stderr_file);

	 		int fd_err;// = open(directory_name2,O_RDWR);
			if (( fd_err=open(directory_name2 ,O_CREAT|O_RDWR ,PERMS))==-1)
			{
				perror("creatinggggggggggggggggggggggggggggggggggggggggg\n\n\n");
				exit (1);
			}

/*
	 		close(fd_out);
	 		close(fd_err);*/
			delete_list_node(new_node);
			
			//char * entolh;
			char** arguments;
			arguments = create_argument_list(msgbuf);
			
			dup2(fd_out,STDOUT_FILENO);
			dup2(fd_err,STDERR_FILENO);
			if(execvp (arguments[0],arguments) <0)
			{
				write(fd_err, "POOL  : to exec apetyxe",60);
				//perror("POOL  : to exec apetyxeweeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee erno == %d \n\n",errno);
				//sleep(2);
				delete_argument_list(arguments);
				close(fd_out);
				close(fd_err);
				exit(errno);
			}
			exit(0);//oloklhrwsh tou job

		}
		else 
		{
			perror("Error with fork");
			exit(1);
		}
}
void pool_function(list_node* new_node,char * entolh,char** arguments) //ti ginetai sto pool me ta jobs
{
	printf("\t\t\tpoolwork...\n");
	
	coord_work_open_descriptors(new_node);//anoigoume tous descriptors gia to pool -coord

	char msgbuf[MSGSIZE+1];

	//pool_submit_job(new_node,entolh, arguments);//synarthsh pou ektelei ena job

		
	pool_communication(new_node);
	//printf("pool_communication returned\n");

	memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror

	pool_end_of_pool(new_node);//synarthsh pou kaloume otan teleiwnei to pool kai stelnei mhnyma ston coord end_of pool
}

int coord_send_status(List * pool_list,int job_name,char * msgbuf)//synarthsh pou stelnei thn erwthsh status job sto katallhlo pool
{//epistrefei 0 an to pool einai done ara kai to job finished 
	//alliws epistrefei 1 kai tote perimenei apo 
	printf("coord _send status JOBS_NUM = %d  , job_name = %d\n\n",JOBS_NUM,job_name);
	int pool_to_send_status = job_name/MAX_JOBS;

	if((job_name % MAX_JOBS) >0)pool_to_send_status ++;
	printf("COORD status : pool_to_sent  = %d\n\n", pool_to_send_status);
	//sleep(3);
	int i;
	list_node * temp = pool_list->start; 
	for(i =1;i<pool_to_send_status ;i++)//gia kathe pool epikoinwnia 
	{

		
		if(temp->next != NULL)temp= temp->next;

	}
	//printf("i == %d \n\n",i);sleep(2);
	if(temp->pool_id ==-1)//an exei oloklhrwsei to pool tote kai to job exei oloklhrwthei
	{
		printf("COORD : this pool is done \n\n");
		return 0;
	}
	else
	{
		
		while((write(temp->fd_write,msgbuf, MSGSIZE+1) ) <0 );//stelnei to aithma gia to status sto sygkekrimmeno pool
		printf("coord _send status telos \n\n");

		return 1;

	}

}

int coord_send_submit(List* pool_list,char *msgbuf)//synarthsh pou stelnoume sto katallhlo pool to katallhlo job
{	
	int i;
	list_node * temp = pool_list->start; 
	for(i =1;i<=pool_list -> number_of_pools ;i++)//gia kathe pool epikoinwnia 
	{
		if(temp->pool_id!=-1)//an yparxei to sygkekrimeno pool
		if(temp->number_jobs <MAX_JOBS)//an mporei na kanei kai alles jobs tote to submit stelnetai se ayto to pool
		{
			while( ( write(temp->fd_write,msgbuf, MSGSIZE+1))<0){} //stelnei sto pool gia na kanei submit to job
					
			/*memset(msgbuf,'\0',MSGSIZE+1);
			while( ( read(temp->fd_read,msgbuf, MSGSIZE+1))<0) //perimenei na parei pisw apo to pool to mynhma gia to pid tou job
			{
				
			}*/
			temp->number_jobs++;
			JOBS_NUM++;
			//printf("COORD  aftere submittttttttttttttttttt : %s\n\n",msgbuf);fflush(stdout);sleep(3);
			//printf("COORD :  To job %d   paei sto pool %d \n\n\n",JOBS_NUM, i);

			return 1;//brethike pool diathesimo
		}
		temp = temp->next;
		if(temp ==NULL)break;
	}
	//pool_list->number_of_pools++;//xreiazetai na dhmiourghtiei kainourgio pool
	//printf("NUMBER OF POOLS %d\n\n",pool_list->number_of_pools);
	return 0; //den yparxei pool gia thn douleia twra prepei na dhmiourghthei kainourgio pool
}
void refresh_nodes(List * pool_list)//stelnoume se ola ta pool na ginei refresh twn node tou coord
{
	list_node * temp = pool_list->start;
	while(temp!= NULL)
	{
		if(temp->pool_id != -1)
		{

			char msgbuf[MSGSIZE +1];
			memset(msgbuf,'\0',MSGSIZE+1);
			sprintf(msgbuf,"send_node");
			while( (write(temp->fd_write, msgbuf,MSGSIZE+1)) <0);
		}
		temp = temp->next;
	}

}
void copy_two_nodes(list_node * new_node,list_node* old_node)//sunarthsh poui antigrafei ton new ston old xwris na tou allazei tous file descriptors kai ton next
{

	new_node->next = old_node->next;
	new_node->pool_pid = old_node->pool_pid;
	new_node-> fd_write = old_node ->fd_write;
	new_node-> fd_read = old_node->fd_read;

	old_node-> pool_pid = new_node->pool_pid;
	old_node->pool_id = new_node->pool_id;
	old_node->number_jobs= new_node->number_jobs;
	old_node->sigterm_flag = new_node->sigterm_flag;

	/*memcpy(old_node->pid_array, new_node->pid_array,MAX_JOBS* sizeof(int));
	memcpy(old_node->job_array, new_node->job_array,MAX_JOBS* sizeof(int));
	memcpy(old_node->seconds_array, new_node->seconds_array,MAX_JOBS* sizeof(int));
*/
	/*int i = 0;
	for(i = 0;i<MAX_JOBS;i++)
	{
		old_node->pid_array[i] = new_node->pid_array[i];
		old_node->job_array[i] = new_node->job_array[i];
		old_node->seconds_array[i] = new_node->seconds_array[i];
		printf("COORD: job %d , pid %d \n",old_node->job_array[i],old_node->pid_array[i]);

	}*/
	//fflush(stdout);
	//sleep(5);
	//memcpy(old,new_node,sizeof(list_node));
}

void communicate_with_pools(List * pool_list, int console_fd_in, int console_fd_out,int type_of_status)//epikoinwnia tou jms_coord me kathe pool
{
	//refresh_nodes(pool_list);//stelnoume se ola ta pool na ginei refresh twn node tou coord

	//printf("COORDD COMUNICATION WITH POOL\n");
	char msgbuf[MSGSIZE+1];
	int i;
	list_node * temp = pool_list->start; 
	for(i =1;i<=pool_list -> number_of_pools ;i++)//gia kathe pool epikoinwnia 
	{
		memset(msgbuf,'\0',MSGSIZE+1);//arxikopoihsei tou string gt ebgaze to valgrind ena eror
		if(temp->pool_id!=-1)
		if((read(temp ->fd_read, msgbuf, MSGSIZE+1) )<0 ) //diavazei apo to pipe tou sygkekrimenou pool 
		{
			//printf("coord nothing to read i = %d  pool_id = %d\n",i,temp->pool_id);
			if(temp->next != NULL )temp= temp->next; 
			continue;
		}
		//an den exei grapsei tpt ayto to pool tote shmainei oti den exei teleiwsei isws to pool
		if( !memcmp(msgbuf ,"end_of_pool",strlen("end_of_pool")) ) //an diavasei end_of_pool apo to pool tote to pool teleiwnei kai kanoume wait
		{
			
			
			//printf("COORD : end of pool\n");fflush(stdout);

			while( ( write(temp->fd_write,"OK_end_of_pool", MSGSIZE+1))<0) ;//stelnoume ston coord oti teleiwse h leitourgia tou pool
			//printf("COORD : end of pool\n");fflush(stdout);
			int status;
			pid_t pid;
			/*do 
			{
				pid  = waitpid(-1, &status, WUNTRACED | WCONTINUED|WNOHANG);
			}
			if(pid ==0 || pid ==-1)continue;//an den allakse tpt se kapoio paidi tote kanoume continue

        if (WIFEXITED(status) ) {
            
*/

			printf("COORD: waiting for child to exit...\n");
			//sleep(2);
			pid = wait(&status);//edw perimenoume to pool na kanei exit
			//while((waitpid(pid ,&status,WNOHANG) ) !=pid);
			printf("COORD : pool pid : %d unique_id :%d  enddddddddddddddddddddddddddddddddd \n\n",pid,temp->pool_id);
			fflush(stdout);
			temp->pool_id =-1;//anti gia pop
			
		//}//pool_list->number_of_pools --;
			//list_node * temp2 = temp;
			//temp = temp->next;
			//pop_list_node(pool_list, temp2);//afairesh tou kombou gia to sygkekrimeno pool
			//continue;
		}
		else if(!memcmp(msgbuf,"job_done",strlen("job_done")))
		{//mynhma apo to pool : job_done <job_ID> <job_pid>
			printf("COORD : ");
			int j=0 ;
			while(msgbuf[j]!='\0')putchar(msgbuf[j++]);//ektypwnw olo to msgbuf
			putchar('\n');
			fflush(stdout);
			while((read(temp ->fd_read,msgbuf,MSGSIZE+1) )<0);

			if( !memcmp(msgbuf,"sending_node",strlen("sending_node")))
			{
				list_node * new_node = malloc(sizeof(list_node));
				while((read(temp ->fd_read,new_node,sizeof(list_node)) )<0);

				printf("COORD : just got new node : poolid : %d , number_jobs : %d , jobs_done :%d\n",new_node->pool_id,new_node->number_jobs,new_node->jobs_done);
				fflush(stdout);
				//sleep(5);
			   	
			   	int pid_array[MAX_JOBS];
			   	while((read(temp ->fd_read,pid_array,MAX_JOBS * sizeof(int)) )<0);//divazoume ton pinaka
			   	memcpy(temp->pid_array, pid_array,MAX_JOBS * sizeof(int));
			   	

				int job_array[MAX_JOBS];
				while((read(temp ->fd_read,job_array,MAX_JOBS * sizeof(int)) )<0);//divazoume ton pinaka
				memcpy(temp->job_array, job_array,MAX_JOBS * sizeof(int));

				int seconds_array[MAX_JOBS];
				while((read(temp ->fd_read,seconds_array,MAX_JOBS * sizeof(int)) )<0);//divazoume ton pinaka
				memcpy(temp->seconds_array, seconds_array,MAX_JOBS * sizeof(int));

			   	copy_two_nodes(new_node,temp);

				free(new_node);
			}
			//write(console_fd_in,msgbuf, MSGSIZE+1);

			//sleep(2);
		}
		else if(!memcmp(msgbuf,"status",strlen("status")))//an tou erthei h apanthsh se ena status apo to pool
		{

			int j=0 ;
			while(msgbuf[j]!='\0')putchar(msgbuf[j++]);//ektypwnw olo to msgbuf
			putchar('\n');
				fflush(stdout);

			
				while( ( write(console_fd_in,msgbuf, MSGSIZE+1)  ) <0) ;
			
			//sleep(2);
		}
		else if(!memcmp(msgbuf,"active",strlen("active")))//an tou erthei h apanthsh se ena status apo to pool
		{

			int j=0 ;
			while(msgbuf[j]!='\0')putchar(msgbuf[j++]);//ektypwnw olo to msgbuf
			putchar('\n');
			while( ( write(console_fd_in,msgbuf, MSGSIZE+1)  ) <0) ;
			fflush(stdout);
			//sleep(2);
		}
		else if(!memcmp(msgbuf,"finished",strlen("finished")))//an tou erthei h apanthsh se ena status apo to pool
		{

			int j=0 ;
			while(msgbuf[j]!='\0')putchar(msgbuf[j++]);//ektypwnw olo to msgbuf
			putchar('\n');
			while( ( write(console_fd_in,msgbuf, MSGSIZE+1)  ) <0) ;
			fflush(stdout);
			//sleep(2);
		}
		else if(!memcmp(msgbuf,"JOBID",strlen("JOBID")))//an tou erthei h apanthsh se ena submit gia to pid tou tou job
		{//tote apanta pisw ston console
			while((write(console_fd_in,msgbuf,MSGSIZE+1)) <0);

			while((read(temp ->fd_read,msgbuf,MSGSIZE+1) )<0);

			if( !memcmp(msgbuf,"sending_node",strlen("sending_node")))
			{
				list_node * new_node = malloc(sizeof(list_node));
				while((read(temp ->fd_read,new_node,sizeof(list_node)) )<0);

				printf("COORD : just got new node : poolid : %d , number_jobs : %d , jobs_done :%d\n",new_node->pool_id,new_node->number_jobs,new_node->jobs_done);
				fflush(stdout);
				//sleep(5);
				/*new_node->next = temp->next;
				new_node->pool_pid = temp->pool_pid;
				new_node-> fd_write = temp ->fd_write;
				new_node-> fd_read = temp->fd_read;
			    memcpy(temp,new_node,sizeof(list_node));*/

			    int pid_array[MAX_JOBS];
			   	while((read(temp ->fd_read,pid_array,MAX_JOBS * sizeof(int)) )<0);//divazoume ton pinaka
			   	memcpy(temp->pid_array, pid_array,MAX_JOBS * sizeof(int));


				int job_array[MAX_JOBS];
				while((read(temp ->fd_read,job_array,MAX_JOBS * sizeof(int)) )<0);//divazoume ton pinaka
				memcpy(temp->job_array, job_array,MAX_JOBS * sizeof(int));

				int seconds_array[MAX_JOBS];
				while((read(temp ->fd_read,seconds_array,MAX_JOBS * sizeof(int)) )<0);//divazoume ton pinaka
				memcpy(temp->seconds_array, seconds_array,MAX_JOBS * sizeof(int));

			    copy_two_nodes(new_node,temp);

				free(new_node);
			}
		}
		else if( !memcmp(msgbuf,"sending_node",strlen("sending_node")))
		{
			list_node * new_node = malloc(sizeof(list_node));
			while((read(temp ->fd_read,new_node,sizeof(list_node)) )<0);

			printf("COORD : just got new node : poolid : %d , number_jobs : %d , jobs_done :%d\n",new_node->pool_id,new_node->number_jobs,new_node->jobs_done);
			fflush(stdout);
			

			int pid_array[MAX_JOBS];
			while((read(temp ->fd_read,pid_array,MAX_JOBS * sizeof(int)) )<0);//divazoume ton pinaka
			memcpy(temp->pid_array, pid_array,MAX_JOBS * sizeof(int));

			int job_array[MAX_JOBS];
			while((read(temp ->fd_read,job_array,MAX_JOBS * sizeof(int)) )<0);//divazoume ton pinaka
			memcpy(temp->job_array, job_array,MAX_JOBS * sizeof(int));

			int seconds_array[MAX_JOBS];
			while((read(temp ->fd_read,seconds_array,MAX_JOBS * sizeof(int)) )<0);//divazoume ton pinaka
			memcpy(temp->seconds_array, seconds_array,MAX_JOBS * sizeof(int));

			copy_two_nodes(new_node,temp);

			free(new_node);
		}
		strcpy(msgbuf,"");
		if(temp->next != NULL)temp = temp->next;
	}
//	printf("ENDDDDD COORDD COMUNICATION WITH POOL\n");

}
list_node * coord_work()//douleia tou coord me ta pool me dhmiourgia ton pipes gia thn epikoinwnia me ena pool 
{	

	list_node * temp_node = create_list_node();

	if ( mkfifo(temp_node->write_to, 0666) == -1 ){//fifo me to  kainourgio pool
		if ( errno!=EEXIST ) { perror("receiver: mkfifo"); exit(6); };
		}
	if ( mkfifo(temp_node->read_from, 0666) == -1 ){
		if ( errno!=EEXIST ) { perror("receiver: mkfifo"); exit(6); };
		}
	

	//printf("telos coord coord_work\n");
	return temp_node;
}
void coord_work_open_descriptors(list_node* temp_node)
{
	int fd_out,fd_in;
//	printf("OPeninggggggg descriptorssssssssssssssssssssssssssssssssss\n\n");fflush(stdout);
	if ( (fd_out=open(temp_node->read_from,O_RDWR| O_NONBLOCK)) < 0)	{ perror("fd_out file open error"); exit(1); }

	while( (fd_in=open(temp_node->write_to, O_WRONLY| O_NONBLOCK)) < 0);//{ perror("fd_in file open error"); exit(1); }
//	printf("OPeninggggggg descriptorssssssssssssssssssssssssssssssssss DONEEEEEEEEEEEE\n\n");fflush(stdout);
	fflush(stdout);
//	if( (fd_in=open(temp_node->write_to, O_WRONLY| O_NONBLOCK)) < 0){ perror("fd_in file open error"); exit(1); }


	temp_node->fd_write = fd_in;
	temp_node->fd_read = fd_out;
	//strcpy(temp_node->write_to, write_to);
	//strcpy(temp_node->read_from,read_from);
	//temp_node->next = NULL;
	//temp_node->number_jobs =1;
}