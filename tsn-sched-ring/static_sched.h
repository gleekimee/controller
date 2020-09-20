#ifndef __STATIC_SCHED__
#define __STATIC_SCHED__


#include "tsn_sched.h"

typedef int(*user_defined_flow_sched)();
typedef int(*user_defined_flow_sort)();

int init_all_flow_feature(u32 flow_num);
int static_schedule_increment(user_defined_flow_sched sched_func1, \
                    user_defined_flow_sched sched_func2, user_defined_flow_sort sort_func, u32 flow_num);
int static_schedule_restart(user_defined_flow_sched sched_func1, \
                    user_defined_flow_sched sched_func2, user_defined_flow_sort sort_func, u32 flow_num);

#endif
