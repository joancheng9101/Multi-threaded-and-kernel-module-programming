#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
int **matrix1 = NULL;
int **matrix2 = NULL;
long int **ans = NULL;
int c1, c2, r1, r2;
int thread_num;
struct timespec start ,finish;
double elapsed;
clock_gettime(CLOCK_MONOTONIC, &start);
void* multiplication(void* id);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int main(int argc, char *argv[]){
    FILE* m1 = fopen((const char*)argv[2], "r");
    if(m1 == NULL){
        printf("file not exit\n");
    }
    fscanf(m1,"%d %d",&r1,&c1);
    matrix1 = (int**)malloc(sizeof(int*)*r1);
    for(int i = 0; i < r1 ; i++) matrix1[i] = (int*)malloc(sizeof(int)*c1);
    for(int i=0 ; i<r1; i++){
        for(int j =0; j<c1; j++){
            fscanf(m1,"%d",&matrix1[i][j]);
        }
    }
    FILE* m2 = fopen(argv[3], "r");
    if(m2 == NULL){
        printf("file not exit\n");
    }
    fscanf(m2,"%d %d",&r2,&c2);
    matrix2 = (int**)malloc(sizeof(int*)*r2);
    for(int i = 0; i < r2 ; i++) matrix2[i] = (int*)malloc(sizeof(int)*c2);
    for(int i=0 ; i<r2; i++){
        for(int j =0; j<c2; j++){
            fscanf(m2,"%d",&matrix2[i][j]);
        }
    }

    ans = (long int**)malloc(sizeof(long int*)*r1);
    for(int i = 0; i < r1 ; i++) ans[i] = (long int*)malloc(sizeof(long int)*c2);

    thread_num = atoi(argv[1]);
    printf("PID:%d\n",getpid());
    pthread_t tid[thread_num];
    for(int id= 0; id< thread_num; id++){
        pthread_create(&tid[id],NULL,multiplication,(void*)id);
    }
    for(int id= 0; id< thread_num; id++){
        pthread_join(tid[id],NULL);
    }
    FILE *wr_fp;
    wr_fp = fopen("result.txt","w+");
    fprintf(wr_fp,"%d %d\n",r1,c2);
    for(int i = 0; i< r1; i++){
        for(int j =0 ; j< c2; j++){
            fprintf(wr_fp,"%ld ",ans[i][j]);
        }
        fprintf(wr_fp,"\n");
    }
    for(int i = 0; i < r1; i++) free(matrix1[i]);
    free(matrix1);
    for(int i = 0; i < r2; i++) free(matrix2[i]);
    free(matrix2);
    for(int i = 0; i < r1; i++) free(ans[i]);
    free(ans);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec-start.tv_nsec) / 100000000.0;
    printf("%lu",elapsed);
    return 0;
}

void* multiplication(void* id){
    int my_rank = (int) id;
    int loading;
    int start, end;
    loading = r1 / thread_num;
    start = my_rank * loading;
    if(r1 % thread_num <= my_rank){
        start += r1 % thread_num;
        end = start + loading;
    }else{
        start += my_rank;
        end = start + loading + 1;
    }
    for(int i = start ; i < end; i++){
        for(int j=0 ; j<c2; j++){
            ans[i][j]=0;
            for(int k=0 ; k<c1; k++){
                ans[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
    pthread_mutex_lock(&mutex);
    pid_t tid;
    tid = gettid();
    char str[100];
    sprintf(str,"%d",tid);
    char read_buf[100];
    int th = open("/proc/thread_info",O_RDWR);
    lseek(th,0,SEEK_SET);
    write(th,str,strlen(str));
    lseek(th,0,SEEK_SET);
    read(th,read_buf,100);
    puts(read_buf);
    close(th);
    pthread_mutex_unlock(&mutex);


   // #define MAX_BUF_SIZE 512

/* Print the proc file "lsproc" */
// int main(int argc, char **argv)
//
	//int th = open("/proc/thread_info",O_RDWR);
	// if(!fp)
	// 	puts("Proc file doesn't exist");
	// else
	// {
		// char buf[512];
        // lseek(fp,0,SEEK_SET);
        // write(fp,"123",4);
        // lseek(fp,0,SEEK_SET);
		// while(fgets(buf, sizeof(buf), fp))
		// 	printf("%s", buf);
		// fclose(fp);
		// fp = NULL;
	// }

	return 0;

    pthread_exit(0);
}