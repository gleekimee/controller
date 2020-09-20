#include "static_sched.h"
#include "greedy_sched.h"
#include "tabu_sched.h"
#include "sa_sched.h"
#include "enum_sched.h"

int main(int argc, char *argv[])
{
    int flow_num = MAX_FLOW_NUM;

	srand((unsigned int)time(0));
    while(flow_num <= MAX_FLOW_NUM)
    {
        
        init_all_flow_feature(flow_num);
      
//        printf("sched all flow with enum sched\n");
//        static_schedule_restart(enum_sched, NULL, NULL);

#if 0        
        printf("sched all flow without adjust offset, sort flow by pkt num\n");
        static_schedule_increment(sched_all_flow_without_adjust_offset, NULL, sort_flow_by_pkt_num);
        static_schedule_restart(sched_all_flow_without_adjust_offset, NULL, sort_flow_by_pkt_num);
        printf("\n");

        printf("sched all flow with adjust offset ascend, sort flow by pkt num\n");
        static_schedule_increment(sched_all_flow_with_adjust_offset_ascend, NULL, sort_flow_by_pkt_num);
        static_schedule_restart(sched_all_flow_with_adjust_offset_ascend, NULL, sort_flow_by_pkt_num);
        printf("\n");

        printf("sched all flow with adjust offset descend, sort flow by pkt num\n");
        static_schedule_increment(sched_all_flow_with_adjust_offset_descend, NULL, sort_flow_by_pkt_num);
        static_schedule_restart(sched_all_flow_with_adjust_offset_descend, NULL, sort_flow_by_pkt_num);
        printf("\n");

        printf("global sched all flow with adjust offset density, sort flow by pkt num\n");
        static_schedule_increment(global_sched_all_flow_with_adjust_offset_density, NULL, sort_flow_by_pkt_num);
        static_schedule_restart(global_sched_all_flow_with_adjust_offset_density, NULL, sort_flow_by_pkt_num);
        printf("\n");

        printf("sched all flow with adjust offset density, sort flow by pkt num\n");
        static_schedule_increment(sched_all_flow_with_adjust_offset_density, NULL, sort_flow_by_pkt_num);
        static_schedule_restart(sched_all_flow_with_adjust_offset_density, NULL, sort_flow_by_pkt_num);
        printf("\n");

        printf("sched all flow with adjust offset density, tabu_sched_out, sort flow by pkt num\n");
        static_schedule_increment(tabu_sched_out_random, NULL, NULL);
        static_schedule_restart(tabu_sched_out_random, NULL, NULL);
        static_schedule_increment(sched_all_flow_with_adjust_offset_density, tabu_sched_out_pro, sort_flow_by_pkt_num);
        static_schedule_restart(sched_all_flow_with_adjust_offset_density, tabu_sched_out_pro, sort_flow_by_pkt_num);
        printf("\n");

        printf("sched all flow with adjust offset density, tabu_sched_offset, sort flow by pkt_num\n");
        static_schedule_increment(tabu_sched_offset_random, NULL, NULL);
        static_schedule_restart(tabu_sched_offset_random, NULL, NULL);
        static_schedule_increment(sched_all_flow_with_adjust_offset_density, tabu_sched_offset_pro, sort_flow_by_pkt_num);
        static_schedule_restart(sched_all_flow_with_adjust_offset_density, tabu_sched_offset_pro, sort_flow_by_pkt_num);
        printf("\n");

        printf("sched all flow with adjust offset density, sa_sched_out, sort flow by pkt num\n");
        static_schedule_increment(sa_sched_out_random, NULL, NULL);
        static_schedule_restart(sa_sched_out_random, NULL, NULL);
        static_schedule_increment(sched_all_flow_with_adjust_offset_density, sa_sched_out_pro, sort_flow_by_pkt_num);
        static_schedule_restart(sched_all_flow_with_adjust_offset_density, sa_sched_out_pro, sort_flow_by_pkt_num);
        printf("\n");

        printf("sched all flow with adjust offset density, sa_sched_offset, sort flow by pkt num\n");
        static_schedule_increment(sa_sched_offset_random, NULL, NULL);
        static_schedule_restart(sa_sched_offset_random, NULL, NULL);
        static_schedule_increment(sched_all_flow_with_adjust_offset_density, sa_sched_offset_pro, sort_flow_by_pkt_num);
        static_schedule_restart(sched_all_flow_with_adjust_offset_density, sa_sched_offset_pro, sort_flow_by_pkt_num);
        printf("\n");

        static_schedule_increment(tabu_sched_out_tradeoff, NULL, NULL);
        static_schedule_restart(tabu_sched_out_tradeoff, NULL, NULL);
        static_schedule_increment(tabu_sched_offset_tradeoff, NULL, NULL);
        static_schedule_restart(tabu_sched_offset_tradeoff, NULL, NULL);
        static_schedule_increment(sa_sched_out_tradeoff, NULL, NULL);
        static_schedule_restart(sa_sched_out_tradeoff, NULL, NULL);
        static_schedule_increment(sa_sched_offset_tradeoff, NULL, NULL);
        static_schedule_restart(sa_sched_offset_tradeoff, NULL, NULL);
        printf("\n");
#endif
#if 0
        static_schedule_increment(tabu_sched_out_tradeoff, NULL, NULL);
        static_schedule_restart(tabu_sched_out_tradeoff, NULL, NULL);
        printf("\n");
        static_schedule_increment(tabu_sched_offset_tradeoff, NULL, NULL);
        static_schedule_restart(tabu_sched_offset_tradeoff, NULL, NULL);
        printf("\n");

#else
		static_schedule_increment(sa_sched_out_tradeoff, NULL, NULL);
		static_schedule_restart(sa_sched_out_tradeoff, NULL, NULL);
        printf("\n");
        static_schedule_increment(sa_sched_offset_tradeoff, NULL, NULL);
        static_schedule_restart(sa_sched_offset_tradeoff, NULL, NULL);
        printf("\n");
#endif
/*        
        printf("sched all flow without adjust offset, sort flow by deadline\n");
        static_schedule_increment(sched_all_flow_without_adjust_offset, NULL, sort_flow_by_deadline);
        static_schedule_restart(sched_all_flow_without_adjust_offset, NULL, sort_flow_by_deadline);
        printf("\n");

        printf("sched all flow with adjust offset ascend, sort flow by deadline\n");
        static_schedule_increment(sched_all_flow_with_adjust_offset_ascend, NULL, sort_flow_by_deadline);
        static_schedule_restart(sched_all_flow_with_adjust_offset_ascend, NULL, sort_flow_by_deadline);
        printf("\n");

        printf("sched all flow with adjust offset descend, sort flow by deadline\n");
        static_schedule_increment(sched_all_flow_with_adjust_offset_descend, NULL, sort_flow_by_deadline);
        static_schedule_restart(sched_all_flow_with_adjust_offset_descend, NULL, sort_flow_by_deadline);
        printf("\n");

        printf("sched all flow with adjust offset density, sort flow by deadline\n");
        static_schedule_increment(sched_all_flow_with_adjust_offset_density, NULL, sort_flow_by_deadline);
        static_schedule_restart(sched_all_flow_with_adjust_offset_density, NULL, sort_flow_by_deadline);
        printf("\n");

        printf("global sched all flow with adjust offset density, sort flow by deadline\n");
        static_schedule_increment(global_sched_all_flow_with_adjust_offset_density, NULL, sort_flow_by_deadline);
        static_schedule_restart(global_sched_all_flow_with_adjust_offset_density, NULL, sort_flow_by_deadline);
        printf("\n");

        printf("sched all flow with adjust offset density, tabu_sched_out, sort flow by deadline\n");
        static_schedule_increment(sched_all_flow_with_adjust_offset_density, tabu_sched_out, sort_flow_by_deadline);
        static_schedule_restart(sched_all_flow_with_adjust_offset_density, tabu_sched_out, sort_flow_by_deadline);
        printf("\n");

        printf("sched all flow with adjust offset density, tabu_sched_offset, sort flow by deadline\n");
        static_schedule_increment(sched_all_flow_with_adjust_offset_density, tabu_sched_offset, sort_flow_by_deadline);
        static_schedule_restart(sched_all_flow_with_adjust_offset_density, tabu_sched_offset, sort_flow_by_deadline);
        printf("\n");
 
        printf("sched all flow with adjust offset density, sa_sched_out, sort flow by deadline\n");
        static_schedule_increment(sched_all_flow_with_adjust_offset_density, sa_sched_out, sort_flow_by_deadline);
        static_schedule_restart(sched_all_flow_with_adjust_offset_density, sa_sched_out, sort_flow_by_deadline);

        printf("sched all flow with adjust offset density, sa_sched_offset, sort flow by deadline\n");
        static_schedule_increment(sched_all_flow_with_adjust_offset_density, sa_sched_offset, sort_flow_by_deadline);
        static_schedule_restart(sched_all_flow_with_adjust_offset_density, sa_sched_offset, sort_flow_by_deadline);
*/
        flow_num += 200;
    }

    return 1;
}
