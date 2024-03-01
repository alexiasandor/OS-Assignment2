#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "a2_helper.h"
#include <semaphore.h>
#include <fcntl.h>
//declaram procesele
pid_t pid1,pid2,pid3,pid4,pid5,pid6,pid7,pid8,pid9;
#define NR_THREADS_FROM_6 4
#define NR_THREADS_BARIERA 37
#define NR_THREADS_PROC_DIF 6
#define NR_MAX_PROC_RUL 5

 pthread_t tids[NR_THREADS_FROM_6];
 pthread_t tids_bariera[NR_THREADS_BARIERA];
 pthread_t tids_proc_dif[NR_THREADS_PROC_DIF];

//declaram procesele
pid_t pid1,pid2,pid3,pid4,pid5,pid6,pid7,pid8,pid9;

pthread_mutex_t lock =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int isOK=0;
int isOK2=0;
int nrOfTh=0 ;// ca sa numaram thread-urile

sem_t *semaphore;

typedef struct {
    int id;
    pthread_mutex_t *lock;
    pthread_cond_t *cond;
}TH_STRUCT;

 TH_STRUCT params[NR_THREADS_FROM_6]; 
 TH_STRUCT params1[NR_THREADS_BARIERA];
 TH_STRUCT params2[NR_THREADS_PROC_DIF];

 //CREARE THREAD-URI SINCRONIZARE PROCESE DIFERITE
 void *thread_function_proc_dif(void *arg){
    TH_STRUCT *params2 =(TH_STRUCT*)arg;
    info(BEGIN,8,params2->id);

    info (END,8,params2->id);
    return NULL;
}

//CREARE THREAD-URI SINCRONIZARE PROCESE DIFERITE
void create_th_proc_dif(){
   for(int i=0;i<NR_THREADS_PROC_DIF;i++){
     params2[i].id=i+1;
     pthread_create(&tids_proc_dif[i],NULL,thread_function_proc_dif,&params2[i]);
    }
    for(int i=0;i<NR_THREADS_PROC_DIF;i++){
        pthread_join(tids_proc_dif[i],NULL);
    }

}

//CREARE THREAD-URI BARIERA
 void *thread_function_1(void *arg){
    TH_STRUCT *params1 =(TH_STRUCT*)arg;

    sem_wait(semaphore);
    info(BEGIN,5,params1->id);

    info (END,5,params1->id);
    sem_post(semaphore);
    return NULL;
}

//CREARE THREAD-URI BARIERA
void create_th_2(){
   for(int i=0;i<NR_THREADS_BARIERA;i++){
     params1[i].id=i+1;
     pthread_create(&tids_bariera[i],NULL,thread_function_1,&params1[i]);
    }
    for(int i=0;i<NR_THREADS_BARIERA;i++){
        pthread_join(tids_bariera[i],NULL);
    }

}
///
void *thread_function(void *arg){
    TH_STRUCT *params =(TH_STRUCT*)arg;
    if(params->id==4){
     pthread_mutex_lock(params->lock);
        while(isOK==0){
            pthread_cond_wait(params->cond,params->lock);
        }
     pthread_mutex_unlock(params->lock);
    }
    info(BEGIN,6,params->id);

    if(params->id==3){
        pthread_mutex_lock(params->lock);
            isOK=1;
            pthread_cond_signal(params->cond); //trezeste thread-ul 3
            while(isOK2==0){
                pthread_cond_wait(params->cond,params->lock);
            }
        pthread_mutex_unlock(params->lock);
    }

    info (END,6,params->id);

    if(params->id==4){
        pthread_mutex_lock(params->lock);
            isOK2=1;
            pthread_cond_signal(params->cond);
        pthread_mutex_unlock(params->lock);
    }
    return NULL;
}
void create_th_1(){
   for(int i=0;i<NR_THREADS_FROM_6;i++){
     params[i].id=i+1;
     params[i].lock=&lock;
     params[i].cond=&cond;
     pthread_create(&tids[i],NULL,thread_function,&params[i]);
    }
    for(int i=0;i<NR_THREADS_FROM_6;i++){
        pthread_join(tids[i],NULL);
    }

}

int main(int argc, char **argv)
{    
    sem_unlink("sem");
     semaphore=sem_open("sem",O_CREAT,0644,5);

    init();
   //pornim primul proces
    info(BEGIN, 1, 0);
    
        //salvam in variabila pid2, apelul sistemului fork
        pid2=fork();
        //verificam daca procesul a reusit 
        if(pid2== -1)
        {
            perror("PID2 Could not create child process");
            return -1;
        } 
        else if(pid2==0)
        {
             // pornim al doilea proces
             info(BEGIN,2,0);
             //salvam in pid3 apelul functiei fork();
             pid3=fork();
    
            if(pid3==-1)
            {
                perror("PID3 Could not create child process");
                return -1;
            }
            else if(pid3==0)
            {
                 // pornim al 3 lea proces 
                info(BEGIN,3,0);
                //salvam in pid5 apelul lui fork;
                 pid5=fork();
                if(pid5==-1)
                {
                    perror("PID5 Could not create child process");
                    return -1;
                }
                else if(pid5==0)
                {
                    //pornim al 5 lea proces
                    info(BEGIN,5,0);
                    //salvam in pid6 apelul lui fork()
                    ////////////////////////////////////////////////////////////////EX3/////////////
                    create_th_2();
                    pid9=fork();
                    if(pid9==-1)
                    {
                        perror("PID9 Could not create child process");
                        return -1;
                    } 
                    else if(pid9==0)
                    {
                        //pornim procesul 
                        info(BEGIN,9,0);
                        //oprim pentru ca am ajuns in capat
                        info (END,9,0);
                        return 9;
                    }else
                    {
                        //int status_pid9=0;
                        waitpid(pid9,0,0);
                    }

                 // inchidem al 5-lea proces 
                     info(END,5,0);
                     return 5;
                } 
                {
                    waitpid(pid5,0,0);
                }
                pid6= fork();
                    if(pid6==-1)
                    {
                        perror("PID6 Could not create child process");
                        return -1;
                    }
                    else if(pid6==0)
                    {
                        //pornim procesul
                        info(BEGIN,6,0);
                      //////////////////////////////////////////////////EX 2///////////////////////////////////////
                      
                      //pthread_t tids[NR_THREADS_FROM_6];
                        //cream thread-urile
                        create_th_1();
                        //oprim procesul
                        info(END,6,0);
                        return 6;
                    } 
                    else
                    {
                        waitpid(pid6,0,0);
                    }
                info(END,3,0);
                return 3;
               
            }
            else
            {
                waitpid(pid3,0,0);
              
            }
              pid4=fork();
                if(pid4==-1)
                {
                     perror("PID4 Could not create child process");
                     return -1;
                }
                else if(pid4==0)
                {
                    info(BEGIN,4,0);
                    info(END,4,0);
                    return 4;
                }
                else
                {   
                    waitpid(pid4,0,0);
                }
            
         info(END,2,0);
         return 2;
        }
        else
        {
        waitpid(pid2,0,0);
        }
     
      pid7=fork();
      if(pid7==-1)
      {
        perror("PID7 Could not create child process");
        return -1;
      }
      else if(pid7==0){
        //pornim si oprim 7
        info(BEGIN,7,0);
        info(END,7,0);
        return 7;
      }
      else {
        waitpid(pid7,0,0);
      }
     pid8=fork();
     if(pid8==-1)
      { 
         perror("PID8 Could not create child process");
         return -1;
      }
      else if(pid8==0){
        //pornim si oprim 8
        info(BEGIN,8,0);
        create_th_proc_dif();
        info(END,8,0);
        return 8;
      }
      else
      {
        waitpid(pid8,0,0);
    }
  info(END,1,0);
  return 0;
}

