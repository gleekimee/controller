#include "sa_sched.h"

extern struct sched_set tsn_sched_set;
extern struct global_resource g_resource;
extern int loop_num;
extern double rand_prob;

static struct sa_sched_solution candidate_solution = {0};
static struct sa_sched_solution init_solution = {0};
static struct sa_sched_solution cur_solution = {0};

static int generate_init_solution_random()
{
    int i = 0, j = 0, s = 0, t = 0;
    int offset = 0;
    u8 node = 0;
    u8 port = 0;
    u16 slot = 0;
    u8 src_node = 0;
    int flag = 1;

    for(j = 0; j < tsn_sched_set.cur_flow_num; j++)
    {
        if(tsn_sched_set.sched[j].flag == SUCCESS)
            continue;
//REPEAT:            
        if(random(2) == 1)
            offset = random(tsn_sched_set.sched[j].period_slot);
        else
            continue;
            
        flag = 0;
        src_node = tsn_sched_set.sched[j].src_sw_id;
        for(s = 0; s < tsn_sched_set.sched[j].path_len; s++)
        {
            node = tsn_sched_set.sched[j].path_info[s].sw_id;
            port = tsn_sched_set.sched[j].path_info[s].port_id;
            for(t = 0; t < (g_resource.cur_sched_slot_num / tsn_sched_set.sched[j].period_slot); t++)
            {
                slot = (offset + t * tsn_sched_set.sched[j].period_slot + s + 1) % g_resource.cur_sched_slot_num;
                g_resource.cqf[node][port][slot].free_len -= tsn_sched_set.sched[j].pkt_num;
                g_resource.cqf[node][port][slot].used_len += tsn_sched_set.sched[j].pkt_num;
                if(g_resource.cqf[node][port][slot].used_len > CQF_QUEUE_LEN)
                {
                    flag = 1;
                }
            }
        }
        
        if(flag == 1)
        {
            for(s = 0; s < tsn_sched_set.sched[j].path_len; s++)
            {
                node = tsn_sched_set.sched[j].path_info[s].sw_id;
                port = tsn_sched_set.sched[j].path_info[s].port_id;
                for(t = 0; t < (g_resource.cur_sched_slot_num / tsn_sched_set.sched[j].period_slot); t++)
                {
                    slot = (offset + t * tsn_sched_set.sched[j].period_slot + s + 1) % g_resource.cur_sched_slot_num;
                    g_resource.cqf[node][port][slot].free_len += tsn_sched_set.sched[j].pkt_num;
                    g_resource.cqf[node][port][slot].used_len -= tsn_sched_set.sched[j].pkt_num;
                }
            }
 //           goto REPEAT;
        }
        else
        {
            tsn_sched_set.sched[j].flag = SUCCESS;
            tsn_sched_set.sched[j].offset = offset;
            tsn_sched_set.cur_suc_num++;
        }

    }

    memset(&init_solution, 0, sizeof(struct tabu_sched_solution));
    for(i = 0; i < tsn_sched_set.cur_flow_num; i++)
    {
        if(tsn_sched_set.sched[i].flag == SUCCESS)
        {
            init_solution.sched_suc[init_solution.cur_suc_num] = tsn_sched_set.sched[i];
            init_solution.cur_suc_num++;
        }
        else
        {
            init_solution.sched_fail[init_solution.cur_fail_num] = tsn_sched_set.sched[i];
            init_solution.cur_fail_num++;
        }
    }
    init_solution.g_resource = g_resource;
    cur_solution = init_solution;

    printf("init solution: %d\n", init_solution.cur_suc_num);
}


static int generate_init_solution_pro()
{
    int i = 0;
    memset(&init_solution, 0, sizeof(struct sa_sched_solution));
    for(i = 0; i < tsn_sched_set.cur_flow_num; i++)
    {
        if(tsn_sched_set.sched[i].flag == SUCCESS)
        {
            init_solution.sched_suc[init_solution.cur_suc_num] = tsn_sched_set.sched[i];
            init_solution.cur_suc_num++;
        }
        else
        {
            init_solution.sched_fail[init_solution.cur_fail_num] = tsn_sched_set.sched[i];
            init_solution.cur_fail_num++;
        }
    }
    init_solution.g_resource = g_resource;
    cur_solution = init_solution;
//    printf("init solution: %d\n", init_solution.cur_suc_num);
}


static void generate_candidate_solution_tradeoff()
{
    int i = 0, j = 0, k = 0, s = 0;
    int num = 0;
    int start = 0;
    struct sched_info temp = {0};
    u8 src_node  = 0;
    u16 node = 0;
    u16 port = 0;
    u16 slot = 0;
    u32 temp_num = 0;
    u32 exchange = 0;
    u32 switch_one = 0;
    u32 switch_two = 0;
    double pick = 0;
    int pro_num = 0;

    candidate_solution = cur_solution;
    pick = ((double)rand()) / RAND_MAX;

    if(pick > rand_prob)
    {
        for(i = 0; i < SA_STEP_SIZE; i++)
        {
            for(j = 0; j < candidate_solution.cur_suc_num - i - 1; j++)
            {
                if(user_defined_multi_stage_sort(candidate_solution.sched_suc[j], candidate_solution.sched_suc[j + 1]))
     //           if(candidate_solution.sched_suc[j].pkt_num > candidate_solution.sched_suc[j + 1].pkt_num)
    //            if(candidate_solution.sched_suc[j].period_slot < candidate_solution.sched_suc[j + 1].period_slot)
    //            if(candidate_solution.sched_suc[j].deadline_slot < candidate_solution.sched_suc[j + 1].deadline_slot)
    //            if(candidate_solution.sched_suc[j].path_len > candidate_solution.sched_suc[j + 1].path_len)
                {
                    temp = candidate_solution.sched_suc[j];
                    candidate_solution.sched_suc[j] = candidate_solution.sched_suc[j + 1];
                    candidate_solution.sched_suc[j + 1] = temp;
                }
            }
        }
    }
    else
    {
        for(i = 0; i < candidate_solution.cur_suc_num; i++)
        { 
            exchange = random(candidate_solution.cur_suc_num);
            temp = candidate_solution.sched_suc[candidate_solution.cur_suc_num - 1 - i];
            candidate_solution.sched_suc[candidate_solution.cur_suc_num - 1 - i] = candidate_solution.sched_suc[exchange];
            candidate_solution.sched_suc[exchange] = temp;
    //        printf("i: %d, flow_id: %d\n", i, cur_solution.sched_suc[i].flow_id);
        }
    }

    for(i = 0; i < SA_STEP_SIZE; i++)
    {
        start = candidate_solution.cur_suc_num - SA_STEP_SIZE + i;
//            printf("candidate: %d, out: %d\n", num, candidate_set[num].entry.out[i]);
        candidate_solution.sched_suc[start].flag = FAIL;
        candidate_solution.sched_fail[candidate_solution.cur_fail_num + i] = candidate_solution.sched_suc[start];
        src_node = candidate_solution.sched_suc[start].src_sw_id;
        for(j = 0; j < candidate_solution.sched_suc[start].path_len; j++)
        {
            node = candidate_solution.sched_suc[start].path_info[j].sw_id;
            port = candidate_solution.sched_suc[start].path_info[j].port_id;
            for(k = 0; k < (candidate_solution.g_resource.cur_sched_slot_num / candidate_solution.sched_suc[start].period_slot); k++)
            {
                slot = (candidate_solution.sched_suc[start].offset + k * candidate_solution.sched_suc[start].period_slot + j + 1) \
                       % candidate_solution.g_resource.cur_sched_slot_num;
                candidate_solution.g_resource.cqf[node][port][slot].free_len += candidate_solution.sched_suc[start].pkt_num;
                candidate_solution.g_resource.cqf[node][port][slot].used_len -= candidate_solution.sched_suc[start].pkt_num;
            }
        }
    }
    
    candidate_solution.cur_suc_num -= SA_STEP_SIZE;
    candidate_solution.cur_fail_num += SA_STEP_SIZE;

    if(pick > rand_prob)
    {
        //for(i = 0; i < SA_STEP_SIZE; i++)
        for(i = 0; i < candidate_solution.cur_fail_num - 1; i++)
        {
            for(j = candidate_solution.cur_fail_num - 2; j >= i; j--)
            {
                if(user_defined_multi_stage_sort(candidate_solution.sched_fail[j], candidate_solution.sched_fail[j + 1]))
                {
                    temp = candidate_solution.sched_fail[j];
                    candidate_solution.sched_fail[j] = candidate_solution.sched_fail[j + 1];
                    candidate_solution.sched_fail[j + 1] = temp;
                }
            }
        }
        
        for(i = 0; i < candidate_solution.cur_fail_num; i++)
        {
            insert_flow_with_adjust_offset_density(&candidate_solution.sched_fail[i], &candidate_solution.g_resource);
    //        insert_flow_with_adjust_offset_descend(&candidate_solution.sched_fail[i], &candidate_solution.g_resource);
            if(candidate_solution.sched_fail[i].flag == SUCCESS)
            {
                candidate_solution.sched_suc[candidate_solution.cur_suc_num] = candidate_solution.sched_fail[i];
                candidate_solution.cur_suc_num++;
            }
        }
 
    }
    else
    {
        for(i = 0; i < candidate_solution.cur_fail_num; i++)
        {
            switch_one = random(candidate_solution.cur_fail_num);
            switch_two = random(candidate_solution.cur_fail_num);
            temp = candidate_solution.sched_fail[switch_one];
            candidate_solution.sched_fail[switch_one] = candidate_solution.sched_fail[switch_two];
            candidate_solution.sched_fail[switch_two] = temp;
        }
        
        for(i = 0; i < candidate_solution.cur_fail_num; i++)
        {
            insert_flow_with_adjust_offset_random(&candidate_solution.sched_fail[i], &candidate_solution.g_resource);
    //        insert_flow_with_adjust_offset_descend(&candidate_solution.sched_fail[i], &candidate_solution.g_resource);
            if(candidate_solution.sched_fail[i].flag == SUCCESS)
            {
                candidate_solution.sched_suc[candidate_solution.cur_suc_num] = candidate_solution.sched_fail[i];
                candidate_solution.cur_suc_num++;
            }
        }
    }

   
    j = 0;
    i = 0;

    temp_num = candidate_solution.cur_fail_num;
    while(i < candidate_solution.cur_fail_num)
    {
        if(candidate_solution.sched_fail[i].flag == SUCCESS)
        {
            i++;
            temp_num--;
        }
        else
        {
            if(j < i)
            {
                candidate_solution.sched_fail[j] = candidate_solution.sched_fail[i];
            }

            j++;
            i++;
        }
    }
    candidate_solution.cur_fail_num = temp_num;
}

int sa_sched_out_tradeoff()
{
    int i = 0, j = 0;
    int repeat_num = 0;
    double cur_temp = MAX_TEMP;
    int repeat_flag = 1;
    int df = 0;
	loop_num = 0;

    generate_init_solution_random();
    while(cur_temp > MIN_TEMP)
    {
//        printf("cur_solution.g_resource.sched_slot_num: %d\n", cur_solution.g_resource.cur_sched_slot_num);
        if(cur_solution.cur_suc_num == tsn_sched_set.cur_flow_num)
            break;
        
        for(i = 0; i < SA_CANDIDATE_NUM; i++)
        {
            generate_candidate_solution_tradeoff();
            if(candidate_solution.cur_suc_num == cur_solution.cur_suc_num)
            {
                repeat_num++;
                if(repeat_num >= SA_MAX_REPEAT_NUM)
                {
                    repeat_flag = 0;
                    break;
                }
            }
            else
                repeat_num = 0;
            
            if(candidate_solution.cur_suc_num >= cur_solution.cur_suc_num)
            {
                cur_solution = candidate_solution;
            }
            else
            {
                df = candidate_solution.cur_suc_num - cur_solution.cur_suc_num;
                if(exp((double)df/cur_temp) > ((double)rand())/RAND_MAX)
                {
                    cur_solution = candidate_solution;
                }
            }
        }

        if(repeat_flag == 0)
        {
			printf("repeat flag: %d\n", repeat_flag);
            break;
        }
        cur_temp = cur_temp * ANNEAL_PARA;
		loop_num++;
    }

    g_resource = cur_solution.g_resource;
    for(i = 0; i < tsn_sched_set.cur_flow_num; i++)
    {
        tsn_sched_set.sched[i].flag = FAIL;
    }

    for(i = 0; i < cur_solution.cur_suc_num; i++)
    {
        for(j = 0; j < tsn_sched_set.cur_flow_num; j++)
        {
            if(cur_solution.sched_suc[i].flow_id == tsn_sched_set.sched[j].flow_id)
            {
                tsn_sched_set.sched[j] = cur_solution.sched_suc[i];
                break;
            }
        }
    }
    tsn_sched_set.cur_suc_num = cur_solution.cur_suc_num;
}

