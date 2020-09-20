#ifndef __TABU_SCHED__
#define __TABU_SCHED__

#include "tsn_sched.h"

/*每个候选解从Flow_Success中选的流个数*/
#define STEP_SIZE 5
/*表长度*/
#define TABU_TBL_SIZE 1000
/*循环的次数*/
#define MAX_LOOP_NUM 50000
#define MAX_IN_NUM 1000
/*候选解数目*/
#define CANDIDATE_NUM 10 
/*最佳值重复的次数*/
#define MAX_REPEAT_NUM 1000

/*随机产生候选解的概率*/
#define RAND_PROB 0

typedef enum
{
    UNEQUAL = 0,
    EQUAL = 1
}COMPARE_FLAG;

struct tabu_entry
{
    u32 out[STEP_SIZE];
    u32 inn[MAX_IN_NUM];
    u32 in_num;
};

struct tabu_table
{
    u32 cur_num;
    struct tabu_entry entry[TABU_TBL_SIZE];
};

struct tabu_sched_solution
{
    int cur_suc_num;
    int cur_fail_num;
    struct tabu_entry entry;
    struct sched_info sched_suc[MAX_FLOW_NUM];
    struct sched_info sched_fail[MAX_FLOW_NUM];
    struct global_resource g_resource;
};

struct period_flag
{
    u16 flag[200];
    u16 total_slot_num;
    u16 cur_slot_num;
};

int compare_tabu_entry(struct tabu_entry entry1, struct tabu_entry entry2);
u32 compute_flow_density_path_diff(struct sched_info sched, u16 offset, struct global_resource resource);
u32 compute_flow_density_path_variance(struct sched_info sched, u16 offset, struct global_resource resource);
int insert_flow_with_adjust_offset_ascend(struct sched_info *sched, struct global_resource *resource);
int insert_flow_with_adjust_offset_descend(struct sched_info *sched, struct global_resource *resource);
int insert_flow_with_adjust_offset_density(struct sched_info *sched, struct global_resource *resource);
int insert_flow_with_adjust_offset_random(struct sched_info *sched, struct global_resource *resource);
int adjust_flow_with_offset_density(struct sched_info *sched, struct global_resource *resource);
int adjust_flow_with_offset_descend(struct sched_info *sched, struct global_resource *resource);
int adjust_flow_with_offset_random(struct sched_info *sched, struct global_resource *resource);
int user_defined_multi_stage_sort(struct sched_info sched1, struct sched_info sched2);
/*随机调度*/
int tabu_sched_out_random();
/*根据指导策略调度*/
int tabu_sched_out_pro();
int tabu_sched_out_tradeoff();
int tabu_sched_offset_random();
int tabu_sched_offset_pro();
int tabu_sched_offset_tradeoff();
#endif
