#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include  <signal.h>
#define PERMS 0644
#define MSGSIZE 200

extern char path[MSGSIZE];
extern int MAX_JOBS;
//#define MAX_JOBS 4
extern int pool_num;
extern int JOBS_NUM;
typedef struct list_node //o jms_coord exei mia lista apo list_node opou kathe list_node einai epikoinwnia me ena pool
{
	int pool_pid;//to pid tou pool
	int pool_id;//to unique id tou pool
	int fd_write ,fd_read;//the descriptors that coord  writes and reads from and to pools
	char write_to[MSGSIZE],read_from[MSGSIZE];
	struct list_node* next;
	int number_jobs;//posa jobs exei to sygkekrimeno pool
	int *pid_array;//pinakas me ta pid twn job
	int *job_array;//pinakas me ta job
	int jobs_done;//posa jobs exoun teleiwsei
	int *seconds_array;
	int sigterm_flag; //auto to flag deixnei an esteile o console shutdown sto coord kai prepei to pool na kanei exit
	
}list_node ;



typedef struct List //o jms_coord exei mia lista 
{
	list_node * start;
	list_node * end;
	int number_of_pools ;//arithmos pool tou coord
}List;

void refresh_nodes(List * pool_list);//stelnoume se ola ta pool na ginei refresh twn node tou coord
void pool_send_node(list_node * new_node, char * msgbuf);//synarthsh pou stelnei ston coord olo to node me tis kainourgies allages
void status_fun2(List * pool_list,int type_of_status,int fd_console,int time_duration);//synarthsh pou stelnei gia ola ta jobs to status ston console 
//xrhsimopoieitai gia thn status-all,show-finished,show-active
void single_status(int job_name, List *pool_list,int fd_console);//synarthsh pou stelnei ena status ston console
void sigterm_action(int signal);//signal handler

void copy_two_nodes(list_node * new_node,list_node* old_node);//sunarthsh poui antigrafei ton new ston old xwris na tou allazei tous file descriptors kai ton next


void delete_argument_list(char **argument_list);//synarthsh pou apodesmebei ton xwro ths argument_list pou xrhsimopoieitai sthn execvp
int coord_send_status(List * pool_list,int job_name,char * msgbuf);//synarthsh pou stelnei thn erwthsh status job sto katallhlo pool

char ** create_argument_list(char * msgbuf);//synarthsh pou dexetai to string "ls -l -a -i " gia paradeigma kai mas epistrefei thn lista {"ls","-l","-a","-i",NULL}
list_node * copy_list_node_for_pool(list_node* mynode);//synarthsh pou epistrefei ena antigrafo node tou mynode
void pop_list_node(List * mylist, list_node * new_node); //synarthsh pou afairei kai katastrefei enan kombo apo thn lista 
void pool_communication(list_node* new_node); //epikoinwnia pool me coord kai jobs
void pool_job_done(list_node* new_node,int i);//synarthsh pou stelnei mhnyma ston coord oti mia job oloklhrwthike
void pool_end_of_pool(list_node * new_node);//synarthsh pou kaloume otan teleiwnei to pool kai stelnei mhnyma ston coord end_of pool
list_node*  find_available_pool(List * pool_list); //synarthsh pou briskei to pool gia to submit job
void pool_submit_job(list_node* new_node,char *msgbuf);//synarthsh pou ektelei ena job ,pairnei to mhnhma msgbuf = "submit ls -l -i" kai to kanei exec katallhla
int coord_send_submit(List* pool_list,char *msgbuf);//synarthsh pou stelnoume sto katallhlo pool to katallhlo job



list_node * coord_work();//douleia tou coord me ta pool
void pool_function(list_node * , char *,char**); //ti ginetai sto pool
void communicate_with_pools(List * pool_list,int console_fd_in , int console_fd_out,int type_of_status) ;//synarthsh pou ginetai h epikoinwnia jms_coord kai pools
void coord_work_open_descriptors(list_node* temp);
void delete_list_node(list_node*);//katastrofh enos list_node

list_node * create_list_node();
void insert_new(List * mylist,list_node* new_node);
List* create_list();
void delete_list(List * mylist );
