#include"sched.h" 
#include<libxml/xmlmemory.h>  
#include<libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

u16 period_slot_set[5] = {20, 40, 60, 80, 100};

/* 解析flow_feature节点，打印节点的内容 */  
void parse_flow_feature(xmlDocPtr doc,xmlNodePtr cur){ 
    xmlNodePtr entry;
    xmlChar *flow_id, *smac, *dmac, *src_ip, *dst_ip, *src_port, *dst_port, *protocol_type;
    xmlChar *period, *dispatch_pit, *receive_pit, *pkt_size, *deadline;
    int i = 0;//flow_id从1开始，这里的i表示第几条流
    for(entry=cur->children;entry;entry=entry->next)
    {
        if(xmlStrcmp(entry->name,(const xmlChar *)"entry")==0)
        {
            flow_id = xmlGetProp(entry,(const xmlChar *)"flow_id");
            smac = xmlGetProp(entry,(const xmlChar *)"smac");
            dmac = xmlGetProp(entry,(const xmlChar *)"dmac");
            src_ip = xmlGetProp(entry,(const xmlChar *)"src_ip");
            dst_ip = xmlGetProp(entry,(const xmlChar *)"dst_ip");
            src_port = xmlGetProp(entry,(const xmlChar *)"src_port");
            dst_port = xmlGetProp(entry,(const xmlChar *)"dst_port");
            protocol_type = xmlGetProp(entry,(const xmlChar *)"protocol_type");
            period = xmlGetProp(entry,(const xmlChar *)"period");
            dispatch_pit = xmlGetProp(entry,(const xmlChar *)"dispatch_pit");
            receive_pit = xmlGetProp(entry,(const xmlChar *)"receive_pit");
            pkt_size = xmlGetProp(entry,(const xmlChar *)"pkt_size");
            deadline = xmlGetProp(entry,(const xmlChar *)"deadline");
            
            flow_set_init.tsn_set[i].flow_id = atoi(flow_id);
            sscanf(smac,"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                &flow_set_init.tsn_set[i].smac[0],&flow_set_init.tsn_set[i].smac[1],
                &flow_set_init.tsn_set[i].smac[2],&flow_set_init.tsn_set[i].smac[3],
                &flow_set_init.tsn_set[i].smac[4],&flow_set_init.tsn_set[i].smac[5]);
            sscanf(dmac,"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                &flow_set_init.tsn_set[i].dmac[0],&flow_set_init.tsn_set[i].dmac[1],
                &flow_set_init.tsn_set[i].dmac[2],&flow_set_init.tsn_set[i].dmac[3],
                &flow_set_init.tsn_set[i].dmac[4],&flow_set_init.tsn_set[i].dmac[5]);
            sscanf(src_ip,"%hhd.%hhd.%hhd.%hhd.",
            &flow_set_init.tsn_set[i].src_ip[0],&flow_set_init.tsn_set[i].src_ip[1],
            &flow_set_init.tsn_set[i].src_ip[2],&flow_set_init.tsn_set[i].src_ip[3]);
            sscanf(dst_ip,"%hhd.%hhd.%hhd.%hhd.",
            &flow_set_init.tsn_set[i].dst_ip[0],&flow_set_init.tsn_set[i].dst_ip[1],
            &flow_set_init.tsn_set[i].dst_ip[2],&flow_set_init.tsn_set[i].dst_ip[3]);
            flow_set_init.tsn_set[i].src_port = atoi(src_port);
            flow_set_init.tsn_set[i].dst_port = atoi(dst_port);
            flow_set_init.tsn_set[i].protocol_type = atoi(protocol_type);
            flow_set_init.tsn_set[i].period = atoi(period);
            flow_set_init.tsn_set[i].dispatch_pit = atoi(dispatch_pit);
            flow_set_init.tsn_set[i].receive_pit = atoi(receive_pit);
            flow_set_init.tsn_set[i].pkt_size = atoi(pkt_size);
            flow_set_init.tsn_set[i].deadline = atoi(deadline);
            i++;
        }      
    }
    flow_set_init.cur_flow_num = i;
    return ;  
}  
/* 解析path节点，打印节点的内容 */  
void parse_path(xmlDocPtr doc, xmlNodePtr cur){  
    xmlNodePtr flow,node;
    xmlChar *flow_id,*id,*inport,*outport;
    int path_len = 0;
    int i=0;
    for(flow=cur->children;flow;flow=flow->next)
    {
        if(xmlStrcmp(flow->name,(const xmlChar *)"flow") == 0)
        {
            flow_id = xmlGetProp(flow,(const xmlChar *)"id");
            for(node=flow->children;node;node=node->next)
            {
                if(xmlStrcmp(node->name,(const xmlChar *)"sw")==0)
                {
                    id = xmlGetProp(node,(const xmlChar *)"id");
                    inport = xmlGetProp(node,(const xmlChar *)"inport");
                    outport = xmlGetProp(node,(const xmlChar *)"outport");

                    flow_set_init.tsn_set[i].path.node[path_len].sw_id = atoi(id);
                    flow_set_init.tsn_set[i].path.node[path_len].inport = atoi(inport);
                    flow_set_init.tsn_set[i].path.node[path_len].outport = atoi(outport);
                    path_len++; 
                }
            }
            flow_set_init.tsn_set[i].path.path_len = path_len;
            path_len = 0;
            //printf("i=%d----flow_id=%d------path_len=%d-----sw_id=%d\n",i,flow_set_init.tsn_set[i].flow_id,flow_set_init.tsn_set[i].path.path_len,flow_set_init.tsn_set[i].path.node[0].sw_id);
            i++;
        }     
    }

    return ;  
}
/* 解析switch节点，打印节点的内容 */  
void parse_switch(xmlDocPtr doc, xmlNodePtr cur){  
    xmlNodePtr entry;
    xmlChar* imac;
    xmlChar *key; 
    for(entry=cur->children;entry;entry=entry->next)
    {
        if(xmlStrcmp(entry->name,(const xmlChar *)"entry")==0)
        {
            imac = xmlGetProp(entry,(const xmlChar *)"imac");
            key = xmlNodeListGetString(doc, entry->xmlChildrenNode, 1);
            sscanf(imac,"14'h%hx",&sw_init.imac);
            sw_init.sw_id = atoi(key);
            //printf("imac:%s\n",imac);
            //printf("sw_id:%s\n",key);
            //printf("sw_init.sw_id:%x\n",sw_init.sw_id);
        }
    } 
    return;  
}
/* 解析end_station节点，打印节点的内容 */  
void parse_end_station(xmlDocPtr doc, xmlNodePtr cur){  
    xmlNodePtr entry;
    xmlChar* imac;
    xmlChar *key; 
    for(entry=cur->children; entry; entry=entry->next)
    {
        if(xmlStrcmp(entry->name,(const xmlChar *)"entry")==0)
        {
            imac = xmlGetProp(entry,(const xmlChar *)"imac");
            key = xmlNodeListGetString(doc, entry->xmlChildrenNode, 1);
            sscanf(imac,"14'h%hx",&end_init.imac);
            end_init.end_id = atoi(key);
            //printf("imac:%s\n",imac);
            //printf("end_id:%s\n",key);
            //printf("end_init.end_id:%x\n",end_init.end_id);
        }
    } 
    return;   
}
/* 解析flow_feature节点，打印节点的内容 */  
void parse_resource(xmlDocPtr doc, xmlNodePtr cur){  
    xmlChar* key;  
    for(cur=cur->xmlChildrenNode;cur;cur=cur->next)
    {  
        if(!xmlStrcmp(cur->name, (const xmlChar *)"queue_size"))
        {  
            key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);  
            //printf("queue_size: %s\n", key);  
            CQF_queue_size = atoi(key);
            //printf("queue_size:%s\n",key);
            xmlFree(key);  
        }
        if(!xmlStrcmp(cur->name, (const xmlChar *)"inject_buffer_num"))
        {  
            key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            inject_buffer_num = atoi(key);
            //printf("inject_buffer_num: %s\n", key);  
            xmlFree(key);  
        }  
        if(!xmlStrcmp(cur->name, (const xmlChar *)"submit_buffer_num"))
        {  
            key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            submit_buffer_num = atoi(key);  
            //printf("submit_buffer_num: %s\n", key);  
            xmlFree(key);  
        }
    }  
    return;  
}  

/* 解析文档 */  
static void parseDoc(char *docname){  
    /* 定义文档和节点指针 */  
    xmlDocPtr doc;  
    xmlNodePtr cur;  
    xmlChar *value;
    /* 进行解析，如果没成功，显示一个错误并停止 */  
    doc = xmlParseFile(docname);  
    if(doc == NULL){  
        fprintf(stderr, "Document not parse successfully. \n");  
        return;  
    }  
  
    /* 获取文档根节点，若无内容则释放文档树并返回 */  
    cur = xmlDocGetRootElement(doc);  
    if(cur == NULL){  
        fprintf(stderr, "empty document\n");  
        xmlFreeDoc(doc);  
        return;  
    }  
  
    /* 确定根节点名是否为input_file，不是则返回 */  
    if(xmlStrcmp(cur->name, (const xmlChar *)"input_file")){  
        fprintf(stderr, "document of the wrong type, root node != story");  
        xmlFreeDoc(doc);  
        return;  
    }  
  
    /* 遍历文档树 */  
    cur = cur->xmlChildrenNode;  
    while(cur != NULL){  
        /* 找到storyinfo子节点 */  
        if(!xmlStrcmp(cur->name, (const xmlChar *)"flow_feature")){ 
             //printf("开始解析flow_feature!\n");
             parse_flow_feature(doc, cur); /* 解析flow_feature子节点 */  
         }
        if(!xmlStrcmp(cur->name, (const xmlChar *)"path")){ 
            //printf("开始解析path!\n"); 
            parse_path(doc, cur); /* 解析path子节点 */  
         }
        if(!xmlStrcmp(cur->name, (const xmlChar *)"switch")){  
            //printf("开始解析switch!\n"); 
            parse_switch(doc, cur); /* 解析switch子节点 */  
        }
        if(!xmlStrcmp(cur->name, (const xmlChar *)"end_station")){  
            //printf("开始解析end!\n"); 
            parse_end_station(doc, cur); /* 解析end_station子节点 */  
        }
        if(!xmlStrcmp(cur->name, (const xmlChar *)"resource")){  
            //printf("开始解析resource!\n");
            parse_resource(doc, cur); /* 解析resource子节点 */  
        }
        cur = cur->next; /* 下一个子节点 */  
    }
    //printf("解析完毕，关闭输入文本！！！\n");  
    xmlFreeDoc(doc); /* 释放文档树 */  
    return;   
}


/* 计算调度周期 */
int gcd(int x,int y){
    int a = 1;
    if(x<y)
    {
        a = x;
        x = y;
        y = a;
    }
    while(x%y != 0)
    {
        a = x%y;
        x = y;
        y = a;
    }
    return y;
}

int lcm(struct flow_set before_sched_set){
    int result = before_sched_set.tsn_set[0].period/time_slot;
    int i=0;
    for(i=1;i< before_sched_set.cur_flow_num;i++)
    {
        result = ((before_sched_set.tsn_set[i].period/time_slot) * result)/gcd(before_sched_set.tsn_set[i].period/time_slot,result);
    }
    return result;
}

/*初始化资源*/
int init_global_resource(){
    int i = 0, j = 0, k = 0;
    memset(&global_resource_init, 0, sizeof(struct global_resource));
    global_resource_init.cur_sched_slot_num = lcm(flow_set_init);
    printf("调度周期是%d\n",global_resource_init.cur_sched_slot_num);
    /*初始化交换机上队列资源*/
    for(i = 0; i < NODE_NUM; i++)
    {
        for(j = 0; j < PORT_NUM; j++)
        {
            for(k = 0; k < MAX_SCHED_SLOT; k++)
            {
                global_resource_init.cqf_queue[i][j][k].total_len = CQF_QUEUE_LEN;
                global_resource_init.cqf_queue[i][j][k].free_len = CQF_QUEUE_LEN;
            }
        }
    }
    /*初始化交换机所有端口的共享资源*/
    for(i = 0; i < NODE_NUM; i++)
    {
        for(k = 0; k < MAX_SCHED_SLOT; k++)
        {
            global_resource_init.cqf_sw[i][k].total_len = 512 - RC_threshold - BE_threshold;
            global_resource_init.cqf_sw[i][k].free_len = 512 - RC_threshold - BE_threshold;
        }
    }
}

u8 init_inject_addr(u32 flow_id){
    return (flow_id%32);    
}
u8 init_submit_addr(u32 flow_id){
     return (flow_id%32);
}
void init_flow_feature()
{
    int i=0;
    int j=0;
    sched_set_init.cur_flow_num = flow_set_init.cur_flow_num;
    for(i=0;i<sched_set_init.cur_flow_num;i++)
    {
        sched_set_init.sched[i].deadline = flow_set_init.tsn_set[i].deadline;
        sched_set_init.sched[i].flow_id = flow_set_init.tsn_set[i].flow_id;
        sched_set_init.sched[i].path.path_len = flow_set_init.tsn_set[i].path.path_len;
        for(j=0;j<flow_set_init.tsn_set[i].path.path_len;j++)
        {
            sched_set_init.sched[i].path.node[j].inport = flow_set_init.tsn_set[i].path.node[j].inport;
            sched_set_init.sched[i].path.node[j].outport = flow_set_init.tsn_set[i].path.node[j].outport;
            sched_set_init.sched[i].path.node[j].sw_id = flow_set_init.tsn_set[i].path.node[j].sw_id;
        }
        sched_set_init.sched[i].period = flow_set_init.tsn_set[i].period;
        sched_set_init.sched[i].pkt_size = flow_set_init.tsn_set[i].pkt_size;
    }
    return ;
}
void init_random_flow_feature(){//初始化流特征信息
    int i=0;
    int j=0;
    sched_set_init.cur_flow_num = MAX_FLOW_NUM;
    for(i=0;i<MAX_FLOW_NUM;i++)
    {
        sched_set_init.sched[i].deadline = (random()%10+10)*1000;//单位us
        sched_set_init.sched[i].flow_id = i;
        sched_set_init.sched[i].path.path_len = flow_set_init.tsn_set[i].path.path_len;
        for(j=0;j<flow_set_init.tsn_set[i].path.path_len;j++)
        {
            sched_set_init.sched[i].path.node[j].inport = flow_set_init.tsn_set[i].path.node[j].inport;
            sched_set_init.sched[i].path.node[j].outport = flow_set_init.tsn_set[i].path.node[j].outport;
            sched_set_init.sched[i].path.node[j].sw_id = flow_set_init.tsn_set[i].path.node[j].sw_id;
        }
        j = random()%5;
        sched_set_init.sched[i].period = period_slot_set[j] * time_slot;
        sched_set_init.sched[i].pkt_size = (random()%12+1); //(128 1500)     
    }
    return ;
}

void offset_constraint_random(u32 flow_num){
    bool flag = FALSE;
    int i=0,j=0;
    u16 offset = 0;
    int hop_num = 0;
    u32 deadline = 0;
    time_t start,finish;
    double duration = 0;
    start = clock();
    while(1)
    {
        //为每一条流随机产生一个offset值
        for(i=0;i<flow_num;i++)
        {        
//NEXT:       sched_set_init.sched[i].offset = rand()%(sched_set_init.sched[i].period);
            sched_set_init.sched[i].offset = rand()%(sched_set_init.sched[i].period);
            for(j=0;j<flow_num;j++){
                if((sched_set_init.sched[i].offset == sched_set_init.sched[j].offset) && (i != j)){
                    sched_set_init.sched[i].offset = (sched_set_init.sched[i].offset+1)%(sched_set_init.sched[i].period);
                    //goto NEXT;
                }
            }
            record[i][back_num] = sched_set_init.sched[i].offset;
            printf("flow_id[%d]:record[%d][%d]=%d\n",i,i,back_num,sched_set_init.sched[i].offset);
        }
        //判定生成的这一组offset是否需要回溯 
        //deadline约束       
        for(i=0;i<flow_num;i++){
            offset = sched_set_init.sched[i].offset;
            printf("----back_num=%d----flow_id=%d----offset=%d----\n",back_num,sched_set_init.sched[i].flow_id,sched_set_init.sched[i].offset);
            hop_num = sched_set_init.sched[i].path.path_len;
            deadline = sched_set_init.sched[i].deadline;
            if(deadline_constraint(offset,hop_num,deadline) == TRUE){
                flag = TRUE;
            }
            else{
                flag = FALSE;
                sched_set_init.sched[i].flag = FALSE;
                record[i][back_num] = offset;//记录失败的那条流的offset
                break;
            }
        }
        //队列资源约束
        if((CQF_share_resource_constraint()==TRUE) && (CQF_resource_constraint()==TRUE) && (flag==TRUE))
        {
            flag = TRUE;
            printf("back_num=%d----success flow_id=%d----offset=%d\n\n",back_num,sched_set_init.sched[i].flow_id,sched_set_init.sched[i].offset);
        }
        else{
            flag = FALSE;
        }
        //当调度成功或者迭代次数超过一千次时
        if((flag == TRUE) || (back_num == BACK_NUM))
        {
            finish = clock();
            duration = ((double)(finish - start) / CLOCKS_PER_SEC);
            //printf("back_num=%d------time_consumed: %lf\n",back_num,duration);
            return ;
        }
        else
        {
            init_global_resource();
            back_num++;
        }
    }
}

//基于SMT求解器随机为每条流产生offset
// void offset_constraint_smt(u32 flow_num int lower_bound,int upper_bound)
// {
//     flag = FALSE;
//     Z3_model m = 0;
//     Z3_lbool result = 0;
//     char str[100] = {0};
//     Z3_context ctx[flow_num];
//     Z3_ast offset[flow_num],lower,upper;
//     Z3_ast c1[flow_num], c2[flow_num];
//     Z3_solver s;

//     printf("\noffset_constraint\n");
//     int i=0;
//     for(i=0;i<flow_num;i++)
//     {
//        ctx[i] = mk_context();
//        s = mk_solver(ctx[i]);
//        offset[i] = mk_int_var(ctx[i], "offset");
//        lower = mk_int(ctx[i], lower_bound);
//        upper = mk_int(ctx[i], upper_bound);

//        c1[i] = Z3_mk_lt(ctx[i], offset[i], upper);
//        c2[i] = Z3_mk_gt(ctx[i], offset[i], lower);
//        Z3_solver_assert(ctx[i], s, c1[i]);
//        Z3_solver_assert(ctx[i], s, c2[i]);
//        printf("model for: lower_bound< offset < upper_bound\n");
//        //check(ctx[i], s, Z3_L_TRUE);
//        result = Z3_solver_check(ctx[i], s);
//        if(result == Z3_L_TRUE)
//        {
//              m = Z3_solver_get_model(ctx[i], s);
//              strcpy(str,Z3_model_to_string(ctx[i], m));
//              sched_set_init.sched[i].offset = str[10]-'0';//填充生成的offset值
//              printf("offset---->%d\n",(str[10]-'0'));
//        }
//        memset(str, 0, sizeof(str));
//     }
//     //判定生成的这一组offset是否需要回溯
//     //判定deadline约束，队列资源约束
//     u16 offset =0；
//     int hop_num =0；
//     u32 deadline =0；
//     for(i=0;i<flow_num;i++)
//     {
//         offset = sched_set_init.sched[i].offset;
//         hop_num = sched_set_init.sched[i].path.path_len;
//         deadline = sched_set_init.sched[i].deadline;
//         if(deadline_constraint(offset,hop_num,deadline) == TRUE)
//         {
//             if((CQF_share_resource_constraint()==TRUE) &&(CQF_resource_constraint()))
//             {
//                 flag = TRUE;
//             }
//         }
//     }
//     if(flag == TRUE)
//     {
//         return ;
//     }
//     else
//     {
//         back_num++;
//         backtrack();
//     }
//     /* assert lower_bound< offset < upper_bound */
//     for(i=0;i<flow_num;i++)
//     {
//        del_solver(ctx[i], s);
//        Z3_del_context(ctx[i]);
//     }
// }

bool deadline_constraint(u16 offset,int hop_num,u32 deadline){
    return ((offset+hop_num+1)<(deadline));
}
bool CQF_resource_constraint(){
    int i = 0, j = 0, k = 0;
    u16 offset = 0;
    u16 temp_slot = 0;
    u8 path_len = 0;
    bool flag = FALSE;
    /*更新队列资源*/
    u8 sw_id = 0;
    u8 outport = 0;
    for(i=0;i<sched_set_init.cur_flow_num;i++)
    {
        flag = TRUE;
        path_len = sched_set_init.sched[i].path.path_len;
        for(j = 0; j < path_len; j++){
            sw_id = sched_set_init.sched[i].path.node[j].sw_id;
            outport = sched_set_init.sched[i].path.node[j].outport;
            for(k = 0; k < global_resource_init.cur_sched_slot_num/(sched_set_init.sched[i].period); k++){
                temp_slot = offset + (j+1) + k*(sched_set_init.sched[i].period);//基于CQF模型

                global_resource_init.cqf_queue[sw_id][outport][temp_slot].free_len -= sched_set_init.sched[i].pkt_size;
                //printf("flow_id=%d---schedule_cycle=%d\n",i,k);
                //printf("队列资源约束：flow_id=%d---global_resource_init.cqf_sw[sw_id:%d][port_id:%d][temp_slot:%d].free_len=%d---hop_num=%d\n",i,sw_id,outport,temp_slot,global_resource_init.cqf_queue[sw_id][outport][temp_slot].free_len,j+1);
                if(global_resource_init.cqf_queue[sw_id][outport][temp_slot].free_len < 0){
                    sched_set_init.sched[i].flag = FALSE;                 
                    record[i][back_num] = offset;//记录失败的那条流的offset
                    flag = FALSE;
                    return flag;
                }                  
            }
        }
        //给每条调度流打上标记
        if(flag == TRUE){
            sched_set_init.sched[i].flag = TRUE;
        }
        else{
            sched_set_init.sched[i].flag = FALSE;
        }
    } 
    return flag;
}

bool CQF_share_resource_constraint(){
    int i = 0, j = 0, k = 0;
    bool flag = FALSE;//指示本次调度是否成功
    u16 offset = 0;
    u16 temp_slot = 0;
    u8 path_len = 0;
    u8 sw_id = 0;
    /*更新队列资源*/
    for(i=0; i<sched_set_init.cur_flow_num; i++)
    {
        flag = TRUE;
        path_len = sched_set_init.sched[i].path.path_len;
        offset = sched_set_init.sched[i].offset;
        for(j = 0; j < path_len; j++)
        {
            sw_id = sched_set_init.sched[i].path.node[j].sw_id;
            for(k = 0; k < global_resource_init.cur_sched_slot_num/(sched_set_init.sched[i].period); k++)
            {
                temp_slot = offset + (j+1) + k*(sched_set_init.sched[i].period);//基于CQF模型
                global_resource_init.cqf_sw[sw_id][temp_slot].free_len -= sched_set_init.sched[i].pkt_size;

                //printf("端口资源约束：flow_id=%d---back_num=%d--->global_resource_init.cqf_sw[sw_id:%d][temp_slot:%d].free_len=%d\n",start,back_num,sw_id,temp_slot,global_resource_init.cqf_sw[sw_id][temp_slot].free_len);
 
                if(global_resource_init.cqf_sw[sw_id][temp_slot].free_len < 0)
                {
                    sched_set_init.sched[i].flag = FALSE;
                    record[i][back_num] = offset;//记录失败的那条流的offset
                    flag = FALSE;
                    return flag;
                }               
            }
        }
        //给每条调度流打上标记
        if(flag == TRUE){
            sched_set_init.sched[i].flag = TRUE;
        }
        else{
            sched_set_init.sched[i].flag = FALSE;
        }
    }
    return flag;
}

static char* mac_t0_string(u8 mac[6]){
    static char str[20] = {"0"};
    sprintf(str,"%02x:%02x:%02x:%02x:%02x:%02x",
			mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    return str;
}

static char* itoa1(u16 num){
     static char ptr[20] = {"0"};
    sprintf(ptr,"%d",num);
    return ptr;
}

u8* transfun(u8* ptr){
    static u8 ip_final[20] ;
    memset(ip_final,0,20);
    u8 ip_trans[20];
    char dianhao[] = {"."};
    sprintf(ip_trans,"%d",(u8)(*ptr));
   // printf("%s",ip_trans);
    strcat(ip_final,ip_trans);

    strcat(ip_final,dianhao);
    sprintf(ip_trans,"%d",(u8)*(ptr+1));
  //  printf("%s",ip_trans);
    strcat(ip_final,ip_trans);

    strcat(ip_final,dianhao);
    sprintf(ip_trans,"%d",(u8)*(ptr+2));
    //printf("%s",ip_trans);
    strcat(ip_final,ip_trans);

    strcat(ip_final,dianhao);
    sprintf(ip_trans,"%d",(u8)*(ptr+3));
    //printf("%s",ip_trans);
    strcat(ip_final,ip_trans);
    return ip_final;
}

void table_set(){
    int i = 0;
    int j = 0;
    //配置寄存器
    u16 time_slot_set = time_slot;
    u32 inject_slot_period_set = 0;//调度周期（所有流发送周期的最小公倍数）
	u32 submit_slot_period_set = 0;
	u8 qbv_or_qch_set = 1;
	u16 rc_regulation_value_set = 0;
	u16 be_regulation_value_set = 0;
	u16 unmap_regulation_value_set = 0;

    inject_slot_period_set = global_resource_init.cur_sched_slot_num;
	submit_slot_period_set = global_resource_init.cur_sched_slot_num;
    //配置转发表
    struct class_fwd_entry fwd_table_set[NODE_NUM][MAX_FLOW_NUM];
    int node_num = 0;
    for(i=0;i<sched_set_init.cur_flow_num;i++)
    {
        for(j=0;j<sched_set_init.sched[i].path.path_len;j++)
        {
            node_num = sched_set_init.sched[i].path.node[j].sw_id;
            fwd_table_set[node_num][i].flow_id = sched_set_init.sched[i].flow_id;
            fwd_table_set[node_num][i].sw_id = sched_set_init.sched[i].path.node[j].sw_id;
            fwd_table_set[node_num][i].outport = sched_set_init.sched[i].path.node[j].outport;
            if(j == sched_set_init.sched[i].path.path_len - 1){
                fwd_table_set[node_num][i].outport = 256;
            }
        }
    }
    //配置注入时间表
    struct inject_time_entry inject_table_set[NODE_NUM][MAX_FLOW_NUM];
    int first_node = 0;
    for(i=0;i<sched_set_init.cur_flow_num;i++)
    {
        first_node = sched_set_init.sched[i].path.node[0].sw_id;
        //printf("first_node=%d-----flow_id=%d----path_len=%d\n",first_node,sched_set_init.sched[i].flow_id,sched_set_init.sched[i].path.path_len);
        inject_table_set[first_node][i].time_slot = sched_set_init.sched[i].offset;
        inject_table_set[first_node][i].inject_addr = init_inject_addr(sched_set_init.sched[i].flow_id);
        //printf("inject_time_entry--first_node=%d----i=%d----flow_id=%d----time_slot=%d----inject_addr=%d!!!\n",first_node,i,sched_set_init.sched[i].flow_id,inject_table_set[first_node][i].time_slot,inject_table_set[first_node][i].inject_addr);
    }
    //配置提交时间表
    struct submit_time_entry submit_table_set[NODE_NUM][MAX_FLOW_NUM];
    int last_node = 0; 
    for(i=0;i<sched_set_init.cur_flow_num;i++)
    {
        last_node = sched_set_init.sched[i].path.node[sched_set_init.sched[i].path.path_len-1].sw_id;
        submit_table_set[last_node][i].time_slot = sched_set_init.sched[i].offset + sched_set_init.sched[i].path.path_len + 1;
        submit_table_set[last_node][i].submit_addr = init_submit_addr(sched_set_init.sched[i].flow_id);
        //printf("submit_time_entry--last_node=%d--flow_id=%d----time_slot=%d----submit_addr=%d!!!\n",last_node,sched_set_init.sched[i].flow_id,submit_table_set[last_node][i].time_slot,submit_table_set[last_node][i].submit_addr);
    }
    //配置映射表
    struct map_entry map_table_set[NODE_NUM][MAX_FLOW_NUM];
    for(i=0;i<sched_set_init.cur_flow_num;i++)
    {
        first_node = sched_set_init.sched[i].path.node[0].sw_id;
        for(j=0;j<4;j++)
        {
            map_table_set[first_node][i].five_tuple.src_ip[j] = flow_set_init.tsn_set[i].src_ip[j];
            map_table_set[first_node][i].five_tuple.dst_ip[j] = flow_set_init.tsn_set[i].dst_ip[j];
        }
        map_table_set[first_node][i].five_tuple.src_port = flow_set_init.tsn_set[i].src_port;
        map_table_set[first_node][i].five_tuple.dst_port = flow_set_init.tsn_set[i].dst_port;
        //printf("**********i=%d-----src_port=%d\n",i,map_table_set[first_node][i].five_tuple.src_port);
        //printf("**********i=%d-----dst_port=%d\n",i,map_table_set[first_node][i].five_tuple.dst_port);
        map_table_set[first_node][i].five_tuple.protocol_type = flow_set_init.tsn_set[i].protocol_type;
        map_table_set[first_node][i].ID = flow_set_init.tsn_set[i].flow_id;
        map_table_set[first_node][i].tsntag.flow_type = 0;
        map_table_set[first_node][i].tsntag.flow_id = sched_set_init.sched[i].flow_id;
        map_table_set[first_node][i].tsntag.inject_addr = init_inject_addr(sched_set_init.sched[i].flow_id);
        map_table_set[first_node][i].tsntag.submit_addr = init_submit_addr(sched_set_init.sched[i].flow_id);
    }
    //配置逆映射表
    int path_len;
    struct remap_entry remap_table_set[NODE_NUM][MAX_FLOW_NUM];
    for(i=0;i<sched_set_init.cur_flow_num;i++)
    {
        //memset(&remap_table_set[i],0,sizeof(remap_table_set[i]));
        path_len = sched_set_init.sched[i].path.path_len;
        last_node = sched_set_init.sched[i].path.node[path_len-1].sw_id;
        remap_table_set[last_node][i].flow_id = sched_set_init.sched[i].flow_id;
        remap_table_set[last_node][i].outport = sched_set_init.sched[i].path.node[path_len-1].outport;
        for(j=0;j<6;j++)
        {
            remap_table_set[last_node][i].dmac[j] = flow_set_init.tsn_set[i].dmac[j];
        }
    }
    //生成XML文本
    char* s;
  //  struct sched_set set1;
    xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"nodes");
    //设置根节点
    xmlDocSetRootElement(doc,root_node);
    xmlNodePtr x_node[NODE_NUM+1];
    int k=0;
    for(k=1;k<=NODE_NUM;k++){
        //在根节点中直接创建node节点
        x_node[k] = xmlNewNode(NULL, BAD_CAST "node");
        xmlAddChild(root_node,x_node[k]);
        xmlNewProp(x_node[k],BAD_CAST"imac",BAD_CAST itoa1(k));

        xmlNodePtr gate_tb = xmlNewNode(NULL, BAD_CAST "gate_table");
        xmlAddChild(x_node[k],gate_tb);
        xmlNewProp(gate_tb,BAD_CAST"id",BAD_CAST "0");
        //printf("-----生成门控表-----\n");
        for (i=0;i<sched_set_init.cur_flow_num;i++){
            xmlNodePtr node = xmlNewNode(NULL,BAD_CAST"entry");
            s = itoa1(i);
            xmlAddChild(gate_tb,node);
            xmlNewProp(node,BAD_CAST"time_slot",BAD_CAST s);
            xmlNewProp(node,BAD_CAST"gate_state",BAD_CAST s);
        }
        xmlNodePtr reg = xmlNewNode(NULL, BAD_CAST "register");
        xmlAddChild(x_node[k],reg);

        xmlNodePtr time_sl = xmlNewNode(NULL, BAD_CAST "time_slot");
        xmlAddChild(reg,time_sl);

        xmlAddChild(time_sl, xmlNewText(BAD_CAST itoa1(time_slot_set)));            //对time_slot进行填写

        xmlNodePtr inj_sl = xmlNewNode(NULL, BAD_CAST "inject_slot_period");
        xmlAddChild(reg,inj_sl);
        xmlAddChild(inj_sl, xmlNewText(BAD_CAST itoa1(inject_slot_period_set)));    //对inject_slot_period进行填写

        xmlNodePtr sub_sl = xmlNewNode(NULL, BAD_CAST "submit_slot_period");
        xmlAddChild(reg,sub_sl);
        xmlAddChild(sub_sl, xmlNewText(BAD_CAST itoa1(submit_slot_period_set)));    //submit_slot_period

        xmlNodePtr qbv_or_qch_p = xmlNewNode(NULL, BAD_CAST "qbv_or_qch");
        xmlAddChild(reg,qbv_or_qch_p);
        xmlAddChild(qbv_or_qch_p, xmlNewText(BAD_CAST itoa1(qbv_or_qch_set)));      //qbv_or_qch

        xmlNodePtr rc_thr = xmlNewNode(NULL, BAD_CAST "rc_regulation_value");
        xmlAddChild(reg,rc_thr);
        xmlAddChild(rc_thr, xmlNewText(BAD_CAST itoa1(rc_regulation_value_set)));   //rc_regulation_value

        xmlNodePtr be_thr = xmlNewNode(NULL, BAD_CAST "be_regulation_value");
        xmlAddChild(reg,be_thr);
        xmlAddChild(be_thr, xmlNewText(BAD_CAST itoa1(be_regulation_value_set)));   //be_regulation_value

        xmlNodePtr umap_thr = xmlNewNode(NULL, BAD_CAST "unmap_regulation_value");
        xmlAddChild(reg,umap_thr);
        xmlAddChild(umap_thr, xmlNewText(BAD_CAST itoa1(unmap_regulation_value_set)));      //unmap_regulation_value
        

        xmlNodePtr for_tb = xmlNewNode(NULL, BAD_CAST "forward_table");
        xmlAddChild(x_node[k],for_tb);

    //  printf("ok--------------------------------------------------\n");
        for (i=0;i<sched_set_init.cur_flow_num;i++){                                                            //进行循环赋值,i为索引
            xmlNodePtr node = xmlNewNode(NULL,BAD_CAST"entry");
            xmlAddChild(for_tb,node);
            xmlNewProp(node,BAD_CAST"flowid",BAD_CAST itoa1(fwd_table_set[k][i].flow_id));
            xmlNewProp(node,BAD_CAST"outport",BAD_CAST itoa1(fwd_table_set[k][i].outport));
        }
        //printf("ok\n");

        xmlNodePtr inj_tb = xmlNewNode(NULL, BAD_CAST "inject_time_table");
        xmlAddChild(x_node[k],inj_tb);

        //printf("-----生成注入表-----\n");
        for (i=0;i<sched_set_init.cur_flow_num;i++){
                xmlNodePtr node = xmlNewNode(NULL,BAD_CAST"entry");

                xmlAddChild(inj_tb,node);
                xmlNewProp(node,BAD_CAST"time_slot",BAD_CAST  itoa1(inject_table_set[k][i].time_slot));
                xmlNewProp(node,BAD_CAST"inject_addr",BAD_CAST itoa1(inject_table_set[k][i].inject_addr));
                //printf("k=%d----i=%d----time_slot=%s---->inject_addr=%s\n",k,i,itoa1(inject_table_set[k][i].time_slot),itoa1(inject_table_set[k][i].inject_addr));
                //????????????
        }

        xmlNodePtr sub_tb = xmlNewNode(NULL, BAD_CAST "submit_time_table");
        xmlAddChild(x_node[k],sub_tb);

        //printf("-----生成提交表-----\n");
        for (i=0;i<sched_set_init.cur_flow_num;i++){
                xmlNodePtr node = xmlNewNode(NULL,BAD_CAST"entry");
                xmlAddChild(sub_tb,node);
                xmlNewProp(node,BAD_CAST"time_slot",BAD_CAST itoa1(submit_table_set[k][i].time_slot));
                xmlNewProp(node,BAD_CAST"submit_addr",BAD_CAST itoa1(submit_table_set[k][i].submit_addr));
        }

        xmlNodePtr map_tb = xmlNewNode(NULL, BAD_CAST "map_table");
        xmlAddChild(x_node[k],map_tb);

        //printf("-----生成映射表-----\n");
        for (i=0;i<sched_set_init.cur_flow_num;i++){
                xmlNodePtr node = xmlNewNode(NULL,BAD_CAST"entry");
                xmlAddChild(map_tb,node);
                xmlNewProp(node,BAD_CAST"id",BAD_CAST itoa1(i));

                xmlNodePtr src_ip = xmlNewNode(NULL, BAD_CAST "src_ip");
                xmlAddChild(node,src_ip);
                xmlAddChild(src_ip, xmlNewText(BAD_CAST transfun(map_table_set[k][i].five_tuple.src_ip)));

                xmlNodePtr dst_ip = xmlNewNode(NULL, BAD_CAST "dst_ip");
                xmlAddChild(node,dst_ip);
                xmlAddChild(dst_ip, xmlNewText(BAD_CAST transfun(map_table_set[k][i].five_tuple.dst_ip)));

                xmlNodePtr src_port = xmlNewNode(NULL, BAD_CAST "src_port");
                xmlAddChild(node,src_port);
                xmlAddChild(src_port,xmlNewText(BAD_CAST itoa1(map_table_set[k][i].five_tuple.src_port)));

                xmlNodePtr dst_port = xmlNewNode(NULL, BAD_CAST "dst_port");
                xmlAddChild(node,dst_port);
                xmlAddChild(dst_port,xmlNewText(BAD_CAST itoa1(map_table_set[k][i].five_tuple.dst_port)));

                xmlNodePtr ptc_type = xmlNewNode(NULL, BAD_CAST "protocol_type");
                xmlAddChild(node,ptc_type);
                xmlAddChild(ptc_type,xmlNewText(BAD_CAST itoa1(map_table_set[k][i].five_tuple.protocol_type)));

                xmlNodePtr flw_type = xmlNewNode(NULL, BAD_CAST "flow_type");
                xmlAddChild(node,flw_type);
                xmlAddChild(flw_type,xmlNewText(BAD_CAST itoa1(map_table_set[k][i].tsntag.flow_type)));

                xmlNodePtr flw_id = xmlNewNode(NULL, BAD_CAST "flow_id");
                xmlAddChild(node,flw_id);
                xmlAddChild(flw_id,xmlNewText(BAD_CAST itoa1(map_table_set[k][i].tsntag.flow_id)));

                xmlNodePtr inject_addr = xmlNewNode(NULL, BAD_CAST "inject_addr");
                xmlAddChild(node,inject_addr);

                xmlAddChild(inject_addr, xmlNewText(BAD_CAST itoa1(map_table_set[k][i].tsntag.inject_addr)));
                xmlNodePtr submit_addr = xmlNewNode(NULL, BAD_CAST "submit_addr");
                xmlAddChild(node,submit_addr);
                xmlAddChild(submit_addr, xmlNewText(BAD_CAST itoa1(map_table_set[k][i].tsntag.submit_addr)));
        }

        xmlNodePtr remap_tb = xmlNewNode(NULL, BAD_CAST "remap_table");
        xmlAddChild(x_node[k],remap_tb);

        //printf("-----生成逆映射表-----\n");
        for (i=0;i<sched_set_init.cur_flow_num;i++){
                xmlNodePtr node = xmlNewNode(NULL,BAD_CAST"entry");

                xmlAddChild(remap_tb,node);
                xmlNewProp(node,BAD_CAST"id",BAD_CAST itoa1(i));
                xmlNodePtr flw_id = xmlNewNode(NULL, BAD_CAST "flow_id");
                xmlAddChild(node,flw_id);
                xmlAddChild(flw_id, xmlNewText(BAD_CAST  itoa1(remap_table_set[k][i].flow_id)));

                xmlNodePtr dmac = xmlNewNode(NULL, BAD_CAST "dmac");
                xmlAddChild(node,dmac);
                xmlAddChild(dmac, xmlNewText(BAD_CAST mac_t0_string(remap_table_set[k][i].dmac)));

                xmlNodePtr outport = xmlNewNode(NULL, BAD_CAST "outport");
                xmlAddChild(node,outport);
                xmlAddChild(outport, xmlNewText(BAD_CAST itoa1(remap_table_set[k][i].outport)));
        }
    }
    //printf("-----生成各类流表完成!!!-----\n");
    xmlSaveFile("output.xml",doc);
    xmlFreeDoc(doc);
    //printf("-----关闭输出XML文本完成!!!-----\n");
    return ;
}

int main(int argc, char **argv){  
    char *docname;  
    if(argc <= 1){  
        printf("Usage: %s docname\n", argv[0]);  
        return 0;  
    }  
    docname=argv[1];
    time_t start,finish;
    double duration = 0;
    start = clock();

    parseDoc(docname);
    init_global_resource();
    init_flow_feature();
    //init_random_flow_feature(u32 flow_num)//初始化流特征信息
    offset_constraint_random(flow_set_init.cur_flow_num);
    table_set();

    finish = clock();
    duration = ((double)(finish - start) / CLOCKS_PER_SEC);
    printf("------generate schedule table------time_consumed: %lf\n",duration);
    return 0;  
}
