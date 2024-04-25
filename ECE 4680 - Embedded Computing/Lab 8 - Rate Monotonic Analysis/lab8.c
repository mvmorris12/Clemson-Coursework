#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define NUMBER_TASKS    8
#define OVERHEAD_MS     0.153
#define FLT_TOLERANCE   0.0001

typedef struct task_t{
    float period_ms;
    float run_time_ms;
    float result_table_usage_ms;
    float io_channel_usage_ms;
    float disk_usage_ms;
    float blocking_dir_r1, blocking_dir_r2, blocking_dir_r3;
    float blocking_pt_r1, blocking_pt_r2, blocking_pt_r3;
    float blocking_r1;
    float blocking_r2;
    float blocking_r3;
    float max_blocking;
} task_t;


task_t *add_task(float p, float rt, float rtu, float iocu, float du){
    task_t *ret_task = (task_t*)malloc(sizeof(task_t));
    ret_task->period_ms = p;
    ret_task->run_time_ms = rt;
    ret_task->result_table_usage_ms = rtu;
    ret_task->io_channel_usage_ms = iocu;
    ret_task->disk_usage_ms = du;
    return ret_task;
}


void calculate_max_blocking(task_t **task_list){
    int i, j;
    printf("Max Blocking:\n");
    for (i=0; i<NUMBER_TASKS; i++){
        task_list[i]->max_blocking = 0.0;
        task_list[i]->blocking_dir_r1 = 0.0;
        task_list[i]->blocking_pt_r1 = 0.0;
        task_list[i]->blocking_dir_r2 = 0.0;
        task_list[i]->blocking_pt_r2 = 0.0;
        task_list[i]->blocking_dir_r3 = 0.0;
        task_list[i]->blocking_pt_r3 = 0.0;
        task_list[i]->blocking_r1 = 0.0;
        task_list[i]->blocking_r2 = 0.0;
        task_list[i]->blocking_r3 = 0.0;
        for (j=i+1; j<NUMBER_TASKS; j++){
            if (task_list[j]->result_table_usage_ms > task_list[i]->blocking_dir_r1){
                task_list[i]->blocking_dir_r1 = task_list[j]->result_table_usage_ms;
            }
            if (task_list[i]->io_channel_usage_ms && task_list[j]->io_channel_usage_ms > task_list[i]->blocking_dir_r2){
                task_list[i]->blocking_dir_r2 = task_list[j]->io_channel_usage_ms;
            }
            if (task_list[i]->disk_usage_ms && task_list[j]->disk_usage_ms > task_list[i]->blocking_dir_r3){
                task_list[i]->blocking_dir_r3 = task_list[j]->disk_usage_ms;
            }
        }
    }
    for (i=1; i<NUMBER_TASKS; i++){
        for (j=i+1; j<NUMBER_TASKS; j++){
            if (task_list[j]->result_table_usage_ms > task_list[i]->blocking_pt_r1){
                task_list[i]->blocking_pt_r1 = task_list[j]->result_table_usage_ms;
            }
            if (task_list[j]->io_channel_usage_ms > task_list[i]->blocking_pt_r2){
                task_list[i]->blocking_pt_r2 = task_list[j]->io_channel_usage_ms;
            }
            if (task_list[j]->disk_usage_ms > task_list[i]->blocking_pt_r3){
                task_list[i]->blocking_pt_r3 = task_list[j]->disk_usage_ms;
            }
        }
    }
    for (i=0; i<NUMBER_TASKS; i++){    
        task_list[i]->blocking_r1 = task_list[i]->blocking_dir_r1 > task_list[i]->blocking_pt_r1 ? task_list[i]->blocking_dir_r1 : task_list[i]->blocking_pt_r1;   
        task_list[i]->blocking_r2 = task_list[i]->blocking_dir_r2 > task_list[i]->blocking_pt_r2 ? task_list[i]->blocking_dir_r2 : task_list[i]->blocking_pt_r2;
        task_list[i]->blocking_r3 = task_list[i]->blocking_dir_r3 > task_list[i]->blocking_pt_r3 ? task_list[i]->blocking_dir_r3 : task_list[i]->blocking_pt_r3;
    }

    for (i=0; i<NUMBER_TASKS; i++){    
        task_list[i]->max_blocking = task_list[i]->blocking_r1 + task_list[i]->blocking_r2 + task_list[i]->blocking_r3;
        printf("T: %0.2f\t R: %0.2f\t b_R1: %0.2f\t b_R2: %0.2f\t b_R3: %0.2f\t b_Max: %0.2f\n", 
            task_list[i]->period_ms,
            task_list[i]->run_time_ms,
            task_list[i]->blocking_r1,
            task_list[i]->blocking_r2,
            task_list[i]->blocking_r3,
            task_list[i]->max_blocking);
    }
    printf("\n");
}


task_t **initialize_tasks(unsigned int items){
    task_t **ret_task_list = (task_t**)malloc(items * sizeof(task_t*));
    ret_task_list[0] = add_task(10.56, 1.30, 0.20, 0, 2.0);  /* Attitude     */
    ret_task_list[1] = add_task(40.96, 4.70, 0.20, 0, 3.0);  /* Velocity     */
    ret_task_list[2] = add_task(61.44, 9.0, 0, 3.0, 0);      /* Attitude Msg */
    ret_task_list[3] = add_task(100.0, 23.0, 0.3, 0, 0);     /* Display      */
    ret_task_list[4] = add_task(165.0, 38.3, 0, 6.0, 0);     /* Nav  Msg     */
    ret_task_list[5] = add_task(285.0, 10.0, 0, 0, 1.0);     /* RT BIT       */
    ret_task_list[6] = add_task(350.0, 3.0, 0.20, 0, 3.0);   /* Position     */
    ret_task_list[7] = add_task(700.0, 2.0, 0, 2.0, 0);      /* Test Msg     */

    calculate_max_blocking(ret_task_list);
    return ret_task_list;
}


float get_sum(task_t **task_list, float Tk, int i, int l){
    float ret_val = 0.0;
    int j;
    for (j=0; j<i; j++){
        ret_val += ((task_list[j]->run_time_ms + OVERHEAD_MS) * (ceil(l * Tk / task_list[j]->period_ms)));
    }    
    ret_val += task_list[i]->run_time_ms + task_list[i]->max_blocking;
    return ret_val - FLT_TOLERANCE;
}


void run_RMA(task_t **task_list){
    int i, k, l;
    float sum_val, comp_val;
    printf("Schedulable for:\n");
    for (i=0; i<NUMBER_TASKS; i++){
        for (k=0; k<=i; k++){
            for (l=1; l<=floor(task_list[i]->period_ms / task_list[k]->period_ms+FLT_TOLERANCE); l++){
                sum_val = get_sum(task_list, task_list[k]->period_ms, i, l);
                comp_val = l*task_list[k]->period_ms;
                if (sum_val <= comp_val + FLT_TOLERANCE){
                    printf("i: %d\t k,l: (%d, %d)\t %0.3f <= %0.3f\n", i+1, k+1, l, sum_val, comp_val);
                } 
            }
        }
    }
}


void free_tasks(task_t **task_list){
    for (int i=0; i<NUMBER_TASKS; i++){
        free(task_list[i]);
    }
    free(task_list);
}


int main(void){
    task_t **task_list = initialize_tasks(NUMBER_TASKS);
    run_RMA(task_list);
    free_tasks(task_list);
    printf("Done\n");
    return 0;
}
