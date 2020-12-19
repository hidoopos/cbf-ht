#include <iostream>
#include "NDN-HashBasedFIB.hpp"
#include "NDN-PrefixGenerator.hpp"
#include <sys/time.h>
using namespace std;

#define MAX 1e6   //整数范围 1 ～ MAX
#define M 1
#define base MAX/M
struct timeval	gStartTime,gEndTime;
struct timeval	startTime,endTime;

int seed = 242;
HashBasedFIB mFIB;
ForwardInfo info;
PrefixGenerator gen(seed);
typedef unordered_map<string, uint64> prefixs;
prefixs baseFIB;
vector<string> vec;

void generate_baseFIB(){
    for (int i = 0; i < base; i++){
        while(1){
            string s = gen.generate_shorter_prefix();
            prefixs::iterator it = baseFIB.find(s);
            if (it == baseFIB.end()){
                baseFIB.insert(make_pair(s, i));
                //mFIB.insert(s,info);
                break;
            }
        }
    }
    string ht;
    for (int j = 0; j < M; j++) {
        for (auto& x: baseFIB) {
            //cout << " " << x.first << ":" << x.second << endl;
            ht = x.first + "/ht" + std::to_string(j);
            //cout << ht << endl;
            mFIB.insert(ht,info);
        }
    }
    //printf("The actual size of baseFIB is %lld\n", baseFIB.size());
    //printf("generate_baseFIB finished\n");
}

int main()
{
    double  a1, a2, a3, a4, a5;

    //配置随机FIB表
    long long fib_size = MAX;
    //配置随机前缀查询次数
    int operation_time = 1e6;
    printf("FIB_size: %lld  query_prefix_times: %d \n", fib_size, operation_time);

    gettimeofday(&startTime,NULL);
    generate_baseFIB();
    long long fibSize = mFIB.mMap.size();
    printf("The actual size of FIB is %lld\n", fibSize);
    //printf("average FIB prefix length: %f\n", (double)gen.prefix_num_sum / fib_size);

    //生成名字前缀用于查找
    gen.prefix_num_sum = 0;
    gettimeofday(&gStartTime,NULL);
    int count = 0;
    //gen.set_seed(seed);
    string ht;
    for (auto& x: baseFIB) {
        //cout << " " << x.first << ":" << x.second << endl;
        ht = x.first + "/ht0/ht1/ht2/ht3/ht4/ht5/ht6/ht7";  //全hit
        //ht = x.first + "/hy0/ht1/ht2/ht3/ht4/ht5/ht6/ht7";  //全miss
        //ht = "/ht0/ht1/ht2/ht3/ht4/ht5/ht6/ht7/ht8/ht9";  //全miss
        //cout << ht << endl;
        vec.push_back(ht);
        count++;
        if(count == operation_time) break;
    }
//    for (int i = 0; i < operation_time; i++) {
//        vec.push_back(gen.generate_prefix()); //printf("%d\n", i);
//    }
    gettimeofday(&gEndTime,NULL);
    printf("The actual size of vec is %d\n", vec.size());
    printf("average Lookup prefix length: %f\n", (double)gen.prefix_num_sum / operation_time);
    a1 = (gEndTime.tv_sec - gStartTime.tv_sec) + (double)(gEndTime.tv_usec - gStartTime.tv_usec)/1000000;
    printf("Generate Lookup prefix Use Time: %f(s)\n", a1);


    //二分查询（有回溯
    gen.set_seed(seed);
    HashBasedFIB::reset_lookup_times();
    mFIB.reset_no_matchs();
    gettimeofday(&gStartTime,NULL);
    for (int i = 0; i < operation_time; i++) {
        mFIB.LPM_search(vec[i]); //printf("\n");
    }
    gettimeofday(&gEndTime,NULL);
    a2 = 1000*(gEndTime.tv_sec - gStartTime.tv_sec) + (double)(gEndTime.tv_usec - gStartTime.tv_usec)/1000;
    printf("Binary With BT Use Time: %f(ms)\n", a2);
    printf("Average Lookup Times: %f\n", HashBasedFIB::get_lookup_times() / (double)operation_time); //平均查找次数：访存次数
    printf("no match times: %d\n", mFIB.get_no_matchs()); //没有匹配到名字的查找次数

    //二分查询（无回溯
    gen.set_seed(seed);
    HashBasedFIB::reset_lookup_times();
    mFIB.reset_no_matchs();
    gettimeofday(&gStartTime,NULL);
    for (int i = 0; i < operation_time; i++) {
        mFIB.LPM_search_no_bt(vec[i]); //printf("\n");
    }
    gettimeofday(&gEndTime,NULL);
    a3 = 1000*(gEndTime.tv_sec - gStartTime.tv_sec) + (double)(gEndTime.tv_usec - gStartTime.tv_usec)/1000;
    printf("Binary Without BT Use Time: %f(ms)\n", a3);
    printf("Average Lookup Times: %f\n", HashBasedFIB::get_lookup_times() / (double)operation_time);
    printf("no match times: %d\n", mFIB.get_no_matchs()); //没有匹配到名字的查找次数


    //线性查询
    gen.set_seed(seed);
    HashBasedFIB::reset_lookup_times();
    mFIB.reset_no_matchs();
    gettimeofday(&gStartTime,NULL);
    for (int i = 0; i < operation_time; i++) {
        mFIB.linear_search(vec[i]); //printf("\n");
    }
    gettimeofday(&gEndTime,NULL);
    a4 = 1000*(gEndTime.tv_sec - gStartTime.tv_sec) + (double)(gEndTime.tv_usec - gStartTime.tv_usec)/1000;
    printf("Linear Use Time: %f(ms)\n", a4);
    printf("Average Lookup Times: %f\n", HashBasedFIB::get_lookup_times() / (double)operation_time);
    printf("no match times: %d\n", mFIB.get_no_matchs()); //没有匹配到名字的查找次数
    HashBasedFIB::reset_lookup_times();
    mFIB.reset_no_matchs();

    printf("Ratio of Linear/Binary: %f, Linear/Binary_no_bt:  %f\n", (a4) / (a2), (a4) / (a3));

    gettimeofday(&endTime,NULL);
    a5 = endTime.tv_sec - startTime.tv_sec + (double)(endTime.tv_usec - startTime.tv_usec)/1e6;
    printf("Total Time: %f(s)\n", a5);

    //mFIB.printAllEntry();

    /*mFIB.insert("/s1/s2/s3/s4/s5/s6", info);
    printf("\n");
    mFIB.insert("/s1/s2/s5/s6/s7/s8", info);
    printf("\n");
    mFIB.insert("/s1", info);
    printf("\n");

    mFIB.erase("/s1/s2/s3/s4/s5/s6");
    printf("\n");
    mFIB.printAllEntry();
    printf("\n");
    mFIB.erase("/s1");
    printf("\n");
    mFIB.printAllEntry();
    printf("\n");
    mFIB.erase("/s1/s2/s5/s6/s7/s8");
    printf("\n");
    mFIB.printAllEntry();
    mFIB.LPM_search("/s1/s2/s3/s5/s7/s8/s9");*/

}