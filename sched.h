#include<stdio.h>
#include<stdlib.h>  
#include<string.h>
#include<math.h>
#include<time.h>
#include<libxml/xmlmemory.h>  
#include<libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include<memory.h>
#include<setjmp.h>
//#include<z3.h>

//#include<stdbool.h>
typedef enum {TRUE=1,FALSE=0} bool;
#define MAX_PATH_LEN 20//最大路径长
#define MAX_FLOW_NUM 100//最大流数目
#define NODE_NUM 4//最大交换机节点数目
#define PORT_NUM 4//交换机端口数目
#define MAX_SCHED_SLOT 1024//最大时间槽数目
#define MAX_map_NUM 1024
#define DEFAULT_TIME_SLOT 100 //单位us
#define BACK_NUM 10000//最大回溯次数
/*队列长度*/
#define CQF_QUEUE_LEN 400
//RC阈值
#define RC_threshold 0
//BE阈值 
#define BE_threshold 0
//表项深度
#define MAX_CLASS_FWD_NUM 1024
#define MAX_GATE_NUM 1024

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct flow_set flow_set_init;
struct global_resource global_resource_init;
struct sched_set sched_set_init;
struct end end_init;
struct sw sw_init;
int max_slot_cycle;
u16 period_slot_set[5];

//回溯次数
u32 back_num=0;
//回溯记录表()
u16 record[MAX_FLOW_NUM][BACK_NUM];
//应用到网络的注入延迟
u16 delay_inject_network;
//网络到应用的提交延迟
u16 delay_submit_app;
//CQF队列长度
u16 CQF_queue_size;
//每个终端的注入地址数目
u8 inject_buffer_num = 32;
//每个终端的注入地址数目
u8 submit_buffer_num = 32;
//时间槽尺寸，单位us
u16 time_slot = 10;

//路径上对应交换机的路径节点信息
struct path_node 
{ 
    u8 sw_id; //交换节点的id 
    u8 inport; //输入端口号
    u8 outport; //输出端口号
};
//对应交换机的路径节点
struct path_info 
{ 
    u8 path_len;//路径的长度(指的是经过的交换机数目) 
    struct path_node node[MAX_PATH_LEN];//经过交换机的路径表 
}; 

//终端信息
struct end
{
    u16 imac;
    u8 end_id;
};
// struct end_set
// {
//     u32 end_num;
//     struct end end_sets[NODE_NUM];
// };
//交换机信息
struct sw
{
    u16 imac;
    u8 sw_id;
};
// struct sw_set
// {
//     u32 sw_num;
//     struct sw sw_sets[NODE_NUM];
// }

//流特征信息
struct tsn_flow_feature
{
    u16 flow_id;//唯一标识的流ID号
    u8 smac[6];
    u8 dmac[6];
    u8 src_ip[4];//五元组信息
    u8 dst_ip[4];   
    u16 src_port;   
    u16 dst_port;
    u16 protocol_type;
    struct path_info path;//流的路径信息
    u16 period;//发送周期
    u16 dispatch_pit;//从应用到网络的注入时刻
    u16 receive_pit;//从网络到应用的提交时刻
    u16 pkt_size;//报文的尺寸大小
    u32 deadline;//截止时间
};

//管理所有流的流特征的结构体
struct flow_set
{
    u32 cur_flow_num;
    struct tsn_flow_feature tsn_set[MAX_FLOW_NUM];
};

//记录调度过程中CQF队列资源的使用信息
struct cqf_resource
{
    int total_len;
    int free_len;//used<time_slot*1Gbps
};
//交换机上所有端口上的资源信息
struct share_cqf_resource
{
    int total_len;
    int free_len;//free_len=total_len-lastslot_used_len-RC阈值-BE阈值;
};
//全局资源信息
struct global_resource
{
    u32 cur_sched_slot_num;
    struct cqf_resource cqf_queue[NODE_NUM][PORT_NUM][MAX_SCHED_SLOT];
    struct share_cqf_resource cqf_sw[NODE_NUM][MAX_SCHED_SLOT];
};


//调度后的流特征信息
struct sched_traffic_info
{
    u16 flow_id;//唯一标识的流ID号
    struct path_info path;//流的路径信息
    u16 period;//周期 
    u16 pkt_size;//报文的尺寸大小
    u32 deadline;//截止时间
    u16 offset;//终端发送时的偏移时间，单位为ns
    bool flag;//用于标识该条流是否调度成功,为1时调度成功；为0时调度失败
};
//管理所有流的流特征的结构体
struct sched_set
{
    u32 cur_flow_num;
    u32 cur_suc_num;
    struct sched_traffic_info sched[MAX_FLOW_NUM];
};

//下面对各类流表的数据结构进行定义
//分类映射表（用于流映射线程查表匹配，配置在第一跳）
struct five_tuple_info
{
        u8 src_ip[4]; //源端ip地址
        u8 dst_ip[4]; //目的端ip地址
        u16 src_port;//源端端口
        u16 dst_port; //目的端端口
        u8 protocol_type; //IP协议类型
};
struct TSNtag 
{
        u8 flow_type; //流类型，分为TS流：000；RC流：001；BE流：010
        u16 flow_id; //静态流量使用flowID，每条静态流分配一个唯一flowID
        u8 inject_addr; // TS流在源端等待发送调度时缓存地址
        u8 submit_addr;// TS流在终端等待接收调度时缓存地址
};
struct map_entry
{
    int ID;//表项ID
    struct five_tuple_info five_tuple;
    struct TSNtag tsntag;

};
struct remap_entry
{
    u16 flow_id; //静态流量使用flowID，每条静态流分配一个唯一flowID
    u8 dmac[6];//目的端的ma'c
    u8 outport; //输出端口号
};

//注入时间表
struct inject_time_entry
{
    int time_slot;//位于匹配域的时间槽号
    u8 inject_addr;// ST分组在源端等待发送调度时缓存地址
};
//提交时间表
struct submit_time_entry
{
    int time_slot;//位于匹配域的时间槽号
    u8 submit_addr;// ST分组在源端等待接收调度时缓存地址
};
//转发表
struct class_fwd_entry
{
    u16 flow_id; //静态流量使用flowID，每条静态流分配一个唯一flowID
    u8 sw_id; //交换节点的id 
    u16 outport; //输出端口号
};

//配置到交换机的流表信息
struct sw_tbl_set 
{ 
    //分类转发表(交换机上目前只需配置转发表)
	struct class_fwd_entry  class_fwd_tbl[MAX_CLASS_FWD_NUM]; 
    //注入时间表
    struct inject_time_entry   inject_time_tbl[MAX_GATE_NUM]; 
    //提交时间表
    struct submit_time_entry   submit_time_tbl[MAX_GATE_NUM];
}; 

//配置到终端的流表信息
struct endstation_tbl_set 
{ 
    //分类映射表
	struct map_entry     map_tbl[MAX_map_NUM]; 
    //注入时间表
    struct inject_time_entry   inject_time_tbl[MAX_GATE_NUM]; 
    //提交时间表
    struct submit_time_entry   submit_time_tbl[MAX_GATE_NUM];
}; 

/* 解析文档 */  
static void parseDoc(char *docname);
int gcd(int x,int y);
int lcm(struct flow_set before_sched_set);

int init_global_resource();
void init_flow_feature();
void init_random_flow_feature();
void parse_flow_feature(xmlDocPtr doc,xmlNodePtr cur);
void parse_path(xmlDocPtr doc, xmlNodePtr cur);
void parse_switch(xmlDocPtr doc, xmlNodePtr cur);
void parse_end_station(xmlDocPtr doc, xmlNodePtr cur);
void parse_resource(xmlDocPtr doc, xmlNodePtr cur);

u8 init_inject_addr(u32 flow_id);
u8 init_submit_addr(u32 flow_id);

void offset_constraint_random(u32 flow_num);
//void offset_constraint(struct flow_set before_sched_set);
bool deadline_constraint(u16 offset,int hop_num,u32 deadline);
bool CQF_resource_constraint();
bool CQF_share_resource_constraint();

//u8 dmac[6]转为字符串
static char* mac_t0_string(u8 mac[6]);
//整数转为字符串
static char* itoa1(u16 num);
//u8 ip[4]转为字符串
u8* transfun(u8* ptr);

//生成各类表项
void table_set();

