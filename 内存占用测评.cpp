#include <iostream>
#include "NDN-HashBasedFIB.hpp"
#include "NDN-PrefixGenerator.hpp"
#include <sys/time.h>
#include <pthread.h>
using namespace std;


#define MAX 1e8    //整数范围 1 ～ MAX
#define N 1           //创建N 个子线程生成FIB表
#define AVE (MAX/N)       //每个子线程生成的前缀个数
#define base 1e8          //baseFIB大小
#define M (AVE/base)

struct timeval	gStartTime,gEndTime;
struct timeval	startTime,endTime;
struct timeval	st,et;

int seed = 242;
HashBasedFIB mFIB;
ForwardInfo info;
PrefixGenerator gen(seed);
typedef unordered_map<string, uint64> prefixs;
prefixs baseFIB;
pthread_spinlock_t glock;


void generate_baseFIB(){
    for (int i = 0; i < base; i++){
        while(1){
            string s = gen.generate_shorter_prefix();
            prefixs::iterator it = baseFIB.find(s);
            if (it == baseFIB.end()){
                baseFIB.insert(make_pair(s, i));
                break;
            }
        }
    }
    printf("The actual size of baseFIB is %lld\n", baseFIB.size());
    printf("generate_baseFIB finished\n");
}


//子线程：生成前缀
void* generate_prefixs(int index)
{
    pthread_spin_lock(&glock);
    double temp;
    string ht;
    generate_baseFIB();
    gettimeofday(&st,NULL);
    long long fibSize = mFIB.mMap.size();
    printf("The actual size of FIB is %lld\n", fibSize);
    for (int i = 0; i < M; i++) {
        for (auto& x: baseFIB) {
            //cout << " " << x.first << ":" << x.second << endl;
            ht = x.first + "/ht" + std::to_string(i);
            //cout << ht << endl;
            mFIB.insert(ht,info);
        }
        printf("i = %d\n",i);
        gettimeofday(&et,NULL);
        temp = et.tv_sec - st.tv_sec + (double)(et.tv_usec - st.tv_usec)/1000000;
        printf("Use Time: %f(s)\n", temp);
        long long fibSize = mFIB.mMap.size();
        printf("The actual size of FIB is %lld\n", fibSize);
        cout<<"桶数量："<<mFIB.mMap.bucket_count()<<endl;
        cout<<"负载因子："<<mFIB.mMap.load_factor()<<endl;

        string getMem = "ps -aux | grep fib > "+ std::to_string(i) + "_10.txt";
        cout << getMem << endl;
        system(getMem.data());

    }
    pthread_spin_unlock(&glock);
    pthread_exit(0);
}

int main()
{
//    string test;
//    printf("%d\n", sizeof(test));
//    string test1 = "/s12345678/s12345678/s12345678/s1234/ht40";
//    printf("%d\n", sizeof(test1)+test1.length());
//    int index = 1;
//    cout<<mFIB.mMap.bucket_size(index)<<endl;


    cout<<sizeof(size_t) + sizeof(void*)<<endl;  //每个桶结构的大致消耗


    cout<<"最大桶数量："<<mFIB.mMap.max_bucket_count()<<endl;
    cout<<"桶数量："<<mFIB.mMap.bucket_count()<<endl;
    cout<<"最大负载因子："<<mFIB.mMap.max_load_factor()<<endl;
    cout<<"负载因子："<<mFIB.mMap.load_factor()<<endl;
    mFIB.mMap.rehash(MAX); //通过rehash设置哈希桶数量，进一步提高效率
    cout<<"桶数量："<<mFIB.mMap.bucket_count()<<endl;
    cout<<"负载因子："<<mFIB.mMap.load_factor()<<endl;

    printf("mFIB.getNodeSize() =  %d\n", mFIB.getNodeSize());
    double  a1, a2, a3, a4, a5;

    //配置随机FIB表
    long long fib_size = MAX;
    //配置随机前缀查询次数
    int operation_time = 1e6;
    printf("FIB_size: %lld  query_prefix_times: %d \n", fib_size, operation_time);

    pthread_t *pthread_id = NULL; //保存子线程id
    pthread_id = (pthread_t*)malloc(N * sizeof(pthread_t));

    gettimeofday(&startTime,NULL);

    gen.prefix_num_sum = 0;
    /********init the global spinlock***********/
    pthread_spin_init(&glock, PTHREAD_PROCESS_PRIVATE);
    //创建N个子线程
    for(int i=0;i<N;i++)
    {
        pthread_create(pthread_id+i,NULL,generate_prefixs,i);
    }

    for(int i=0;i<N;i++)
    {
        //等待子线程结束，如果该子线程已经结束，则立即返回
        pthread_join(pthread_id[i],NULL);
    }

    free(pthread_id);
    printf("average FIB prefix length: %f\n", (double)gen.prefix_num_sum / fib_size);

    //随机数生成器的时间  用于除去
//    gen.prefix_num_sum = 0;
//    gettimeofday(&gStartTime,NULL);
//    for (int i = 0; i < operation_time; i++) {
//        gen.generate_prefix(); //printf("%d\n", i);
//    }
//    gettimeofday(&gEndTime,NULL);
//    printf("average Lookup prefix length: %f\n", (double)gen.prefix_num_sum / operation_time);
//    a1 = 1000*(gEndTime.tv_sec - gStartTime.tv_sec) + (double)(gEndTime.tv_usec - gStartTime.tv_usec)/1000;
//    printf("Generate Lookup prefix Use Time: %f(ms)\n", a1);
//
//
//    //二分查询（有回溯
//    gen.set_seed(seed);
//    HashBasedFIB::reset_lookup_times();
//    gettimeofday(&gStartTime,NULL);
//    for (int i = 0; i < operation_time; i++) {
//        mFIB.LPM_search(gen.generate_prefix()); //printf("\n");
//    }
//    gettimeofday(&gEndTime,NULL);
//    a2 = 1000*(gEndTime.tv_sec - gStartTime.tv_sec) + (double)(gEndTime.tv_usec - gStartTime.tv_usec)/1000;
//    printf("Binary With BT Use Time: %f(ms)\n", a2);
//    printf("Average Lookup Times: %f\n", HashBasedFIB::get_lookup_times() / (double)operation_time); //平均查找次数：访存次数
//
//
//    //二分查询（无回溯
//    gen.set_seed(seed);
//    HashBasedFIB::reset_lookup_times();
//    gettimeofday(&gStartTime,NULL);
//    for (int i = 0; i < operation_time; i++) {
//        mFIB.LPM_search_no_bt(gen.generate_prefix()); //printf("\n");
//    }
//    gettimeofday(&gEndTime,NULL);
//    a3 = 1000*(gEndTime.tv_sec - gStartTime.tv_sec) + (double)(gEndTime.tv_usec - gStartTime.tv_usec)/1000;
//    printf("Binary Without BT Use Time: %f(ms)\n", a3);
//    printf("Average Lookup Times: %f\n", HashBasedFIB::get_lookup_times() / (double)operation_time);
//
//
//    //线性查询
//    gen.set_seed(seed);
//    HashBasedFIB::reset_lookup_times();
//    gettimeofday(&gStartTime,NULL);
//    for (int i = 0; i < operation_time; i++) {
//        mFIB.linear_search(gen.generate_prefix()); //printf("\n");
//    }
//    gettimeofday(&gEndTime,NULL);
//    a4 = 1000*(gEndTime.tv_sec - gStartTime.tv_sec) + (double)(gEndTime.tv_usec - gStartTime.tv_usec)/1000;
//    printf("Linear Use Time: %f(ms)\n", a4);
//    printf("Average Lookup Times: %f\n", HashBasedFIB::get_lookup_times() / (double)operation_time);
//    HashBasedFIB::reset_lookup_times();
//
//
//    printf("Ratio of Linear/Binary: %f, Linear/Binary_no_bt:  %f\n", (a4 - a1) / (a2 - a1), (a4 - a1) / (a3 - a1));

    gettimeofday(&endTime,NULL);
    a5 = endTime.tv_sec - startTime.tv_sec + (double)(endTime.tv_usec - startTime.tv_usec)/1e6;
    printf("Total Time: %f(s)\n", a5);

    long long fibSize = mFIB.mMap.size();
    printf("The actual size of FIB is %lld\n", fibSize);

    cout<<"最大桶数量："<<mFIB.mMap.max_bucket_count()<<endl;
    cout<<"桶数量："<<mFIB.mMap.bucket_count()<<endl;
    cout<<"最大负载因子："<<mFIB.mMap.max_load_factor()<<endl;
    cout<<"负载因子："<<mFIB.mMap.load_factor()<<endl;

    //mFIB.printAllEntry();

//    mFIB.insert("/s1/s2/s3/s4/s5/s6", info);
//    printf("\n");
//    mFIB.insert("/s1/s2/s5/s6/s7/s8", info);
//    printf("\n");
//    mFIB.insert("/s1", info);
//    printf("\n");
//
//    mFIB.erase("/s1/s2/s3/s4/s5/s6");
//    printf("\n");
//    mFIB.printAllEntry();
//    printf("\n");
//    mFIB.erase("/s1");
//    printf("\n");
//    mFIB.printAllEntry();
//    printf("\n");
//    mFIB.erase("/s1/s2/s5/s6/s7/s8");
//    printf("\n");
//    mFIB.printAllEntry();
//    mFIB.LPM_search("/s1/s2/s3/s5/s7/s8/s9");

    return 0;

}