#include "static_sched.h"

struct sched_set tsn_sched_set = {0};
struct global_resource g_resource = {0};
struct flow_set tsn_flow_set = {0};
int max_slot_cycle = 0;
int loop_num = 0;
u16 period_slot_set[5] = {20, 40, 60, 80, 100};
//u16 period_slot_set[5] = {100, 100, 100, 50, 50};
//u16 period_slot_set[6] = {20, 30, 40, 60, 80, 100};

int back_num = 0;

int gcd_two(int x,int y)
{
    int a = 1;

    if(x < y)
    {
        a = x;
        x = y;
        y = a;
    }

    while(x % y != 0)
    {
        a = x % y;
        x = y;
        y = a;
    }
    return y;
}

int lcm(struct sched_set tsn_sched_set)  
{
    int result = tsn_sched_set.sched[0].period_slot, i = 0;

    for(i = 1; i < tsn_sched_set.cur_flow_num; i++)
    {
        result = (tsn_sched_set.sched[i].period_slot * result) / gcd_two(tsn_sched_set.sched[i].period_slot, result);
    }
    return result;
}

int gcd(struct flow_set tsn_flow_set)
{
    int result = tsn_flow_set.flow[0].period, i = 0;

    for(i = 1; i < tsn_flow_set.cur_flow_num; i++)
    {
        result = gcd_two(tsn_flow_set.flow[i].period, result);
    }
    return result;
}


/*初始化资源*/
int init_global_resource()
{
    int i = 0, j = 0, k = 0;
    
    memset(&g_resource, 0, sizeof(struct global_resource));
    g_resource.cur_sched_slot_num = lcm(tsn_sched_set);
//    printf("cur_sched_slot_num: %d!\n", g_resource.cur_sched_slot_num);
    for(i = 0; i < NODE_NUM; i++)
    {
        for(j = 0; j < PORT_NUM; j++)
        {
            for(k = 0; k < MAX_SCHED_SLOT; k++)
            {
                g_resource.cqf[i][j][k].total_len = CQF_QUEUE_LEN;
                g_resource.cqf[i][j][k].free_len = CQF_QUEUE_LEN;
            }
        }
    }
}

/*更新资源*/
int update_global_resource_increment()
{
    int s = 0, i = 0, j = 0, k = 0;
    int temp_cur_sched_slot_num = 0;
    u8 node = 0;
    u8 port = 0;
    u8 src_node = 0;
    int slot = 0;
    int flag = 0;

    back_num = 0;
    temp_cur_sched_slot_num = g_resource.cur_sched_slot_num * 2;
    memset(&g_resource, 0, sizeof(struct global_resource));
    g_resource.cur_sched_slot_num = temp_cur_sched_slot_num;
    for(i = 0; i < NODE_NUM; i++)
    {
        for(j = 0; j < PORT_NUM; j++)
        {
            for(k = 0; k < MAX_SCHED_SLOT; k++)
            {
                g_resource.cqf[i][j][k].total_len = CQF_QUEUE_LEN;
                g_resource.cqf[i][j][k].free_len = CQF_QUEUE_LEN;
            }
        }
    }

    for(s = 0; s < tsn_sched_set.cur_flow_num; s++)
    {
        if(tsn_sched_set.sched[s].flag == FAIL)
            continue;
        src_node = tsn_sched_set.sched[s].src_sw_id;
        flag = 1;
        for(i = 0; i < tsn_sched_set.sched[s].path_len; i++)
        {
            node = tsn_sched_set.sched[s].path_info[i].sw_id;
            port = tsn_sched_set.sched[s].path_info[i].port_id;
            for(j = 0; j < (g_resource.cur_sched_slot_num / tsn_sched_set.sched[s].period_slot); j++)
            {
                slot = (tsn_sched_set.sched[s].offset + j * tsn_sched_set.sched[s].period_slot + i + 1) % g_resource.cur_sched_slot_num;
                g_resource.cqf[node][port][slot].free_len -= tsn_sched_set.sched[s].pkt_num;
                g_resource.cqf[node][port][slot].used_len += tsn_sched_set.sched[s].pkt_num;
                if(g_resource.cqf[node][port][slot].used_len > CQF_QUEUE_LEN)
                {
                    flag = 0;
                    back_num++;
                   //printf("exceed: used_len: %d, free_len: %d!\n", g_resource.cqf[node][slot].used_len, g_resource.cqf[node][slot].free_len);
                }
            }
        }

        if(flag == 0)
        {
            for(i = 0; i < tsn_sched_set.sched[s].path_len; i++)
            {
                node = tsn_sched_set.sched[s].path_info[i].sw_id;
                port = tsn_sched_set.sched[s].path_info[i].port_id;
                for(j = 0; j < (g_resource.cur_sched_slot_num / tsn_sched_set.sched[s].period_slot); j++)
                {
                    slot = (tsn_sched_set.sched[s].offset + j * tsn_sched_set.sched[s].period_slot + i + 1) % g_resource.cur_sched_slot_num;
                    g_resource.cqf[node][port][slot].free_len += tsn_sched_set.sched[s].pkt_num;
                    g_resource.cqf[node][port][slot].used_len -= tsn_sched_set.sched[s].pkt_num;
                }
            }

            tsn_sched_set.sched[s].flag = FAIL;
            tsn_sched_set.cur_suc_num--;
        }
    }


//    printf("before_sched_slot_num: %d, cur_sched_slot_num: %d!\n", \
           temp_sched_slot, g_resource.cur_sched_slot_num);
/*    for(i = 0; i < NODE_NUM; i++)
    {
        for(j = temp_sched_slot - 1; j >= 0; j--)
        {
            if(g_resource.cqf[i][j].used_len > 0)
            {
                g_resource.cqf[i][j * 2].used_len = g_resource.cqf[i][j].used_len;
                g_resource.cqf[i][j * 2].free_len = g_resource.cqf[i][j].free_len;
                g_resource.cqf[i][j].free_len = CQF_QUEUE_LEN;
                g_resource.cqf[i][j].used_len = 0;
            }
        }
    }
*/    
}

int update_global_resource_restart()
{
    int i = 0, j = 0, k = 0;
    int temp_cur_sched_slot_num = 0;

    temp_cur_sched_slot_num = g_resource.cur_sched_slot_num * 2;
    memset(&g_resource, 0, sizeof(struct global_resource));
    g_resource.cur_sched_slot_num = temp_cur_sched_slot_num;
    for(i = 0; i < NODE_NUM; i++)
    {
        for(j = 0; j < PORT_NUM; j++)
        {
            for(k = 0; k < MAX_SCHED_SLOT; k++)
            {
                g_resource.cqf[i][j][k].total_len = CQF_QUEUE_LEN;
                g_resource.cqf[i][j][k].free_len = CQF_QUEUE_LEN;
            }
        }
    }
}


int init_all_flow_feature(u32 flow_num)
{
    int i = 0, j = 0;

    memset(&tsn_flow_set, 0, sizeof(struct flow_set));
    tsn_flow_set.cur_flow_num = flow_num;
    for(i = 0; i < flow_num; i++)
    {
        tsn_flow_set.flow[i].flow_id = i;
        tsn_flow_set.flow[i].src_host_id = random(HOST_NUM_PER_NODE);
        tsn_flow_set.flow[i].dst_host_id = random(HOST_NUM_PER_NODE);
        tsn_flow_set.flow[i].src_sw_id = random(NODE_NUM);
REPEAT:
        tsn_flow_set.flow[i].dst_sw_id = random(NODE_NUM);
        if(tsn_flow_set.flow[i].src_sw_id == tsn_flow_set.flow[i].dst_sw_id)
            goto REPEAT;
        j = random(5);
        tsn_flow_set.flow[i].period = period_slot_set[j] * DEFAULT_SLOT_CYCLE;
//        tsn_flow_set.flow[i].pkt_num = 1; // pkt_num [64, 1500]
        tsn_flow_set.flow[i].pkt_num = (random(3) + 1); // pkt_num [64, 1500]
        tsn_flow_set.flow[i].deadline = (random(20) + 10) * 1000; 
    }
    
    max_slot_cycle = gcd(tsn_flow_set);
    printf("max_slot_cycle: %d\n", max_slot_cycle);
}

int compute_all_flow_sched_info(int slot_cycle)
{
    int i = 0 , j = 0;
    int id = 0;

    memset(&tsn_sched_set, 0, sizeof(struct sched_set));
    tsn_sched_set.cur_flow_num = tsn_flow_set.cur_flow_num;
    for(i = 0; i < tsn_sched_set.cur_flow_num; i++)
    {
        tsn_sched_set.sched[i].flow_id = tsn_flow_set.flow[i].flow_id;
        tsn_sched_set.sched[i].src_sw_id = tsn_flow_set.flow[i].src_sw_id;
        tsn_sched_set.sched[i].dst_sw_id = tsn_flow_set.flow[i].dst_sw_id;
        if(tsn_sched_set.sched[i].src_sw_id < tsn_sched_set.sched[i].dst_sw_id)
        {
            tsn_sched_set.sched[i].path_len = tsn_sched_set.sched[i].dst_sw_id - tsn_sched_set.sched[i].src_sw_id + 1;
        }
        else
        {
            tsn_sched_set.sched[i].path_len = NODE_NUM - tsn_sched_set.sched[i].src_sw_id + tsn_sched_set.sched[i].dst_sw_id + 1;
        }
 
        for(j = 0; j < tsn_sched_set.sched[i].path_len; j++)
        {
            tsn_sched_set.sched[i].path_info[j].sw_id = (tsn_sched_set.sched[i].src_sw_id + j) % NODE_NUM;
            tsn_sched_set.sched[i].path_info[j].port_id = 0;
        }
        tsn_sched_set.sched[i].path_info[tsn_sched_set.sched[i].path_len - 1].port_id = tsn_flow_set.flow[i].dst_host_id + 1;

        tsn_sched_set.sched[i].period = tsn_flow_set.flow[i].period;
        tsn_sched_set.sched[i].period_slot = tsn_flow_set.flow[i].period / slot_cycle;
        tsn_sched_set.sched[i].pkt_num = tsn_flow_set.flow[i].pkt_num;
        tsn_sched_set.sched[i].deadline = tsn_flow_set.flow[i].deadline;
        tsn_sched_set.sched[i].deadline_slot = tsn_sched_set.sched[i].deadline / slot_cycle;
        tsn_sched_set.sched[i].flag = FAIL;
//        tsn_sched_set.sched[i].deadline_slot = random(tsn_sched_set.sched[i].period_slot) + tsn_sched_set.sched[i].path_len + 1;
        //printf("flow id: %d, src_sw_id: %d, dst_sw_id: %d, path_len: %d, period: %d, pkt_num: %d, deadline: %d!\n",\
        tsn_sched_set.sched[i].flow_id, tsn_sched_set.sched[i].src_sw_id, tsn_sched_set.sched[i].dst_sw_id,\
        tsn_sched_set.sched[i].path_len, tsn_sched_set.sched[i].period_slot, tsn_sched_set.sched[i].pkt_num,\
        tsn_sched_set.sched[i].deadline_slot);
    }
}

int update_all_flow_sched_info_increment()
{
    int i = 0;
    
    for(i = 0; i < tsn_sched_set.cur_flow_num; i++)
    {
        tsn_sched_set.sched[i].period_slot = tsn_sched_set.sched[i].period_slot * 2;
        tsn_sched_set.sched[i].deadline_slot = tsn_sched_set.sched[i].deadline_slot * 2;
//        tsn_sched_set.sched[i].offset = tsn_sched_set.sched[i].offset * 2;
//        tsn_sched_set.sched[i].deadline_slot = random(tsn_sched_set.sched[i].period_slot) + tsn_sched_set.sched[i].path_len + 1;
        //printf("flow id: %d, src_sw_id: %d, dst_sw_id: %d, path_len: %d, period: %d, pkt_num: %d, deadline: %d!\n",\
        tsn_sched_set.sched[i].flow_id, tsn_sched_set.sched[i].src_sw_id, tsn_sched_set.sched[i].dst_sw_id,\
        tsn_sched_set.sched[i].path_len, tsn_sched_set.sched[i].period_slot, tsn_sched_set.sched[i].pkt_num,\
        tsn_sched_set.sched[i].deadline_slot);
    }
}

int update_all_flow_sched_info_restart()
{
    int i = 0;
    
    for(i = 0; i < tsn_sched_set.cur_flow_num; i++)
    {
        tsn_sched_set.sched[i].flag = FAIL;
        tsn_sched_set.sched[i].period_slot = tsn_sched_set.sched[i].period_slot * 2;
        tsn_sched_set.sched[i].deadline_slot = tsn_sched_set.sched[i].deadline_slot * 2;
        tsn_sched_set.sched[i].offset = 0;
//        tsn_sched_set.sched[i].offset = tsn_sched_set.sched[i].offset * 2;
//        tsn_sched_set.sched[i].deadline_slot = random(tsn_sched_set.sched[i].period_slot) + tsn_sched_set.sched[i].path_len + 1;
        //printf("flow id: %d, src_sw_id: %d, dst_sw_id: %d, path_len: %d, period: %d, pkt_num: %d, deadline: %d!\n",\
        tsn_sched_set.sched[i].flow_id, tsn_sched_set.sched[i].src_sw_id, tsn_sched_set.sched[i].dst_sw_id,\
        tsn_sched_set.sched[i].path_len, tsn_sched_set.sched[i].period_slot, tsn_sched_set.sched[i].pkt_num,\
        tsn_sched_set.sched[i].deadline_slot);
    }
    tsn_sched_set.cur_suc_num = 0;
}


float compute_resource_ratio()
{
    float ratio = 0;
    int i = 0, j = 0, k = 0;
    int port_sum = 0;

    for(i = 0; i < NODE_NUM; i++)
    {
        for(j = 0; j < PORT_NUM; j++)
        {
            for(k = 0; k < g_resource.cur_sched_slot_num; k++)
            {
                if(g_resource.cqf[i][j][k].used_len > 0)
                {
                    if(g_resource.cqf[i][j][k].used_len > CQF_QUEUE_LEN)
                        printf("node: %d, port: %d, slot: %d, used_len: %d, free_len: %d\n", \
                               i, j, k, g_resource.cqf[i][j][k].used_len, g_resource.cqf[i][j][k].free_len);
                    ratio += g_resource.cqf[i][j][k].used_len;
                }
            }
        }
    }
    port_sum = (HOST_NUM_PER_NODE + 1) * NODE_NUM;
    return ratio / (CQF_QUEUE_LEN * g_resource.cur_sched_slot_num * port_sum);
}

/*增量调度*/
int static_schedule_increment(user_defined_flow_sched sched_func1, \
                    user_defined_flow_sched sched_func2, user_defined_flow_sort sort_func, u32 flow_num)
{
    int cur_slot_cycle = 0;
    int count = 0;
	int i = 0, j = 0;
    time_t start, finish;
	int suc_num[TEST_NUM][10] = {0};
    int min_suc_num = 0xFFFFFF;
    int max_suc_num = 0;
    int ave_suc_num = 0;
    int sloop_num[TEST_NUM][10] = {0};
    int min_loop_num = 0xFFFFFF;
    int max_loop_num = 0;
    int ave_loop_num = 0;
    double duration[TEST_NUM][10] = {0};
    double min_duration = 0xFFFFFF;
    double max_duration = 0;
    double ave_duration = 0;
    float resource_ratio[TEST_NUM][10] = {0};
    float min_resource_ratio = 0xFFFFFF;
    float max_resource_ratio = 0;
    float ave_resource_ratio = 0;


	for(i = 0; i < TEST_NUM; i++)
	{
        init_all_flow_feature(flow_num);
        count = 0;
        start = clock();
        cur_slot_cycle = max_slot_cycle;
        compute_all_flow_sched_info(cur_slot_cycle);
        init_global_resource();
        if(sort_func != NULL)
        {
            sort_func();
        }
        
        back_num = 0;
        while(cur_slot_cycle >= MIN_SLOT_CYCLE)
        {
            sched_func1();
            if(sched_func2 != NULL)
                sched_func2();
            finish = clock();
            duration[i][count] = ((double)(finish - start) / CLOCKS_PER_SEC);
            sloop_num[i][count] = loop_num;
            suc_num[i][count] = tsn_sched_set.cur_suc_num;
            resource_ratio[i][count] = compute_resource_ratio();

            printf("TEST_NUM: %d, Increment [%d]th: slot_cycle: %d, sched_cycle: %d, suc_num: %d, back_num: %d\n", \
                   i, count, cur_slot_cycle, g_resource.cur_sched_slot_num, tsn_sched_set.cur_suc_num, back_num);

            printf("TEST_NUM: %d, Increment [%d]th: flow_num: %d, loop_num: %d, time_consumed: %lf, resource_ratio: %f\n", \
                   i, count, tsn_sched_set.cur_flow_num, loop_num, duration[i][count], resource_ratio[i][count]);

            if(tsn_sched_set.cur_suc_num < tsn_sched_set.cur_flow_num)
            {
                cur_slot_cycle = cur_slot_cycle / 2;
                update_all_flow_sched_info_increment();  
                update_global_resource_increment();  
                count++;
            }
            else
                break;
        }

	}

    cur_slot_cycle = max_slot_cycle;
    for(i = 0; i < count; i++)
    {
        min_suc_num = 0xFFFFFF;
        max_suc_num = 0;
        ave_suc_num = 0;
        min_loop_num = 0xFFFFFF;
        max_loop_num = 0;
        ave_loop_num = 0;
        min_duration = 0xFFFFFF;
        max_duration = 0;
        ave_duration = 0;
        min_resource_ratio = 0xFFFFFF;
        max_resource_ratio = 0;
        ave_resource_ratio = 0;

        for(j = 0; j < TEST_NUM; j++)
        {
            if(min_suc_num > suc_num[j][i])
                min_suc_num = suc_num[j][i];
            if(max_suc_num < suc_num[j][i])
                max_suc_num = suc_num[j][i];
            ave_suc_num += suc_num[j][i];
        }
        ave_suc_num = ave_suc_num / TEST_NUM;
        
        for(j = 0; j < TEST_NUM; j++)
        {
            if(min_loop_num > sloop_num[j][i])
                min_loop_num = sloop_num[j][i];
            if(max_loop_num < sloop_num[j][i])
                max_loop_num = sloop_num[j][i];
            ave_loop_num += sloop_num[j][i];
        }
        ave_loop_num = ave_loop_num / TEST_NUM;
        
        for(j = 0; j < TEST_NUM; j++)
        {
            if(min_duration > duration[j][i])
                min_duration = duration[j][i];
            if(max_duration < duration[j][i])
                max_duration = duration[j][i];
            ave_duration += duration[j][i];
        }
        ave_duration = ave_duration / TEST_NUM;
        
        for(j = 0; j < TEST_NUM; j++)
        {
            if(min_resource_ratio > resource_ratio[j][i])
                min_resource_ratio = resource_ratio[j][i];
            if(max_resource_ratio < resource_ratio[j][i])
                max_resource_ratio = resource_ratio[j][i];
            ave_resource_ratio += resource_ratio[j][i];
        }
        ave_resource_ratio = ave_resource_ratio / TEST_NUM;
        printf("SUM Increment [%d]th: flow_num: %d, cur_slot_cycle: %d, min_suc_num: %d, max_suc_num: %d, ave_suc_num: %d\n", \
               i, flow_num, cur_slot_cycle, min_suc_num, max_suc_num, ave_suc_num);
        printf("SUM Increment [%d]th: min_loop_num: %d, max_loop_num: %d, ave_loop_num: %d\n", i, min_loop_num, max_loop_num, ave_loop_num);
        printf("SUM Increment [%d]th: min_duration: %lf, max_duration: %lf, ave_duration: %lf\n", i, min_duration, max_duration, ave_duration);
        printf("SUM Increment [%d]th: min_resource_ratio: %f, max_resource_ratio: %f, ave_resource_ratio: %f\n", i, \
               min_resource_ratio, max_resource_ratio, ave_resource_ratio);
               
        cur_slot_cycle = cur_slot_cycle / 2;           
    }


}

int static_schedule_restart(user_defined_flow_sched sched_func1, \
                    user_defined_flow_sched sched_func2, user_defined_flow_sort sort_func, u32 flow_num)
{
    int cur_slot_cycle = 0;
    int count = 0;
    int i = 0, j = 0;
    time_t start, finish;
    int suc_num[TEST_NUM][10] = {0};
    int min_suc_num = 0xFFFFFF;
    int max_suc_num = 0;
    int ave_suc_num = 0;
    int sloop_num[TEST_NUM][10] = {0};
    int min_loop_num = 0xFFFFFF;
    int max_loop_num = 0;
    int ave_loop_num = 0;
    double duration[TEST_NUM][10] = {0};
    double min_duration = 0xFFFFFF;
    double max_duration = 0;
    double ave_duration = 0;
    float resource_ratio[TEST_NUM][10] = {0};
    float min_resource_ratio = 0xFFFFFF;
    float max_resource_ratio = 0;
    float ave_resource_ratio = 0;

	for(i = 0; i < TEST_NUM; i++)
    {
        init_all_flow_feature(flow_num);
        count = 0;
        start = clock();
        cur_slot_cycle = max_slot_cycle;
        compute_all_flow_sched_info(cur_slot_cycle);
        init_global_resource();
        if(sort_func != NULL)
        {
            sort_func();
        }

        while(cur_slot_cycle >= MIN_SLOT_CYCLE)
        {
            sched_func1();
            if(sched_func2 != NULL)
                sched_func2();

            finish = clock();
            duration[i][count] = ((double)(finish - start) / CLOCKS_PER_SEC);
            sloop_num[i][count] = loop_num;
            suc_num[i][count] = tsn_sched_set.cur_suc_num;
            resource_ratio[i][count] = compute_resource_ratio();
            
            
            printf("TEST_NUM: %d, Restart [%d]th: slot_cycle: %d, sched_cycle: %d, suc_num: %d\n", \
                   i, count, cur_slot_cycle, g_resource.cur_sched_slot_num, tsn_sched_set.cur_suc_num);

            printf("TEST_NUM: %d, Restart [%d]th: flow_num: %d, loop_num: %d, time_consumed: %lf, resource_ratio: %f\n", \
                   i, count, tsn_sched_set.cur_flow_num, loop_num, duration[i][count], resource_ratio[i][count]);

            if(tsn_sched_set.cur_suc_num < tsn_sched_set.cur_flow_num)
            {
                cur_slot_cycle = cur_slot_cycle / 2;
                update_all_flow_sched_info_restart();
                update_global_resource_restart();
                count++;
            }
            else
                break;
        }
    }

    cur_slot_cycle = max_slot_cycle;
    for(i = 0; i < count; i++)
    {
        min_suc_num = 0xFFFFFF;
        max_suc_num = 0;
        ave_suc_num = 0;
        min_loop_num = 0xFFFFFF;
        max_loop_num = 0;
        ave_loop_num = 0;
        min_duration = 0xFFFFFF;
        max_duration = 0;
        ave_duration = 0;
        min_resource_ratio = 0xFFFFFF;
        max_resource_ratio = 0;
        ave_resource_ratio = 0;

        for(j = 0; j < TEST_NUM; j++)
        {
            if(min_suc_num > suc_num[j][i])
                min_suc_num = suc_num[j][i];
            if(max_suc_num < suc_num[j][i])
                max_suc_num = suc_num[j][i];
            ave_suc_num += suc_num[j][i];
        }
        ave_suc_num = ave_suc_num / TEST_NUM;
        
        for(j = 0; j < TEST_NUM; j++)
        {
            if(min_loop_num > sloop_num[j][i])
                min_loop_num = sloop_num[j][i];
            if(max_loop_num < sloop_num[j][i])
                max_loop_num = sloop_num[j][i];
            ave_loop_num += sloop_num[j][i];
        }
        ave_loop_num = ave_loop_num / TEST_NUM;
        
        for(j = 0; j < TEST_NUM; j++)
        {
            if(min_duration > duration[j][i])
                min_duration = duration[j][i];
            if(max_duration < duration[j][i])
                max_duration = duration[j][i];
            ave_duration += duration[j][i];
        }
        ave_duration = ave_duration / TEST_NUM;
        
        for(j = 0; j < TEST_NUM; j++)
        {
            if(min_resource_ratio > resource_ratio[j][i])
                min_resource_ratio = resource_ratio[j][i];
            if(max_resource_ratio < resource_ratio[j][i])
                max_resource_ratio = resource_ratio[j][i];
            ave_resource_ratio += resource_ratio[j][i];
        }
        ave_resource_ratio = ave_resource_ratio / TEST_NUM;
        printf("SUM Restart [%d]th: flow_num: %d, cur_slot_cycle: %d, min_suc_num: %d, max_suc_num: %d, ave_suc_num: %d\n", \
               i, flow_num, cur_slot_cycle, min_suc_num, max_suc_num, ave_suc_num);
        printf("SUM Restart [%d]th: min_loop_num: %d, max_loop_num: %d, ave_loop_num: %d\n", i, min_loop_num, max_loop_num, ave_loop_num);
        printf("SUM Restart [%d]th: min_duration: %lf, max_duration: %lf, ave_duration: %lf\n", i, min_duration, max_duration, ave_duration);
        printf("SUM Restart [%d]th: min_resource_ratio: %f, max_resource_ratio: %f, ave_resource_ratio: %f\n", i, \
               min_resource_ratio, max_resource_ratio, ave_resource_ratio);
               
        cur_slot_cycle = cur_slot_cycle / 2;           
    }

}
