#include <iostream>
#include "NDN-HashBasedFIB.hpp"
#include "NDN-PrefixGenerator.hpp"
#include "BinarySearch.hpp"
#include "CBF-HT.h"
#include <sys/time.h>
#include <pthread.h>
using namespace std;


#define MAX 1e7  //整数范围 1 ～ MAX
#define N 1           //创建N 个子线程生成FIB表
#define AVE (MAX/N)       //每个子线程生成的前缀个数
#define base 1e7        //baseFIB大小
#define M (AVE/base)

struct timeval	gStartTime,gEndTime;
struct timeval	startTime,endTime;
struct timeval	st,et;

int seed = 242;

//HashBasedFIB mFIB;
BinarySearch mFIB(MAX*2);
//CBF_HT mFIB(MAX);

ForwardInfo info;

std::string ht;
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

    generate_baseFIB();
    //mFIB.mMap = hash_table_new(MAX);
    int k = 0;
    gettimeofday(&st,NULL);
    for (int i = 0; i < M; i++) {
        for (auto& x: baseFIB) {
            //cout << " " << x.first << ":" << x.second << endl;
            //std::string ht = x.first + "/ht" + std::to_string(i);
            std::string ht = x.first;
            //cout << ht << endl;
            mFIB.insert(ht,info);
            k++;
            if((k%1000000) == 0) {
                gettimeofday(&et,NULL);
                temp = et.tv_sec - st.tv_sec + (double)(et.tv_usec - st.tv_usec)/1000000;
                printf("Use Time: %f(s)\n", temp);

                uint64 fibSize = getContentSize(mFIB.mMap);
                printf("The actual size of FIB is %d\n", fibSize);
                cout<<"冲突数目："<<getConflict(mFIB.mMap)<<endl;
                cout<<"桶数量："<<gettableSize(mFIB.mMap)<<endl;
                cout<<"负载因子："<<gethtLF(mFIB.mMap)<<endl;
                string getMem = "ps -aux | grep fib > "+ std::to_string(k/1000000) + "_10.txt";
                cout << getMem << endl;
                system(getMem.data());
            }
        }
//        for (int j=0;j<base;j++) {
//            //cout << " " << x.first << ":" << x.second << endl;
//            //std::string ht = x.first + "/ht" + std::to_string(i);
//            ht="/ht0123456789/ht0123456789/ht0123456789/ht0123456789"+std::to_string(j);
//            //cout << ht << endl;
//            mFIB.insert(ht,info);
//        }
        //printf("i = %d\n",i);
//        gettimeofday(&et,NULL);
//        temp = et.tv_sec - st.tv_sec + (double)(et.tv_usec - st.tv_usec)/1000000;
//        printf("Use Time: %f(s)\n", temp);
//
//        uint64 fibSize = getContentSize(mFIB.mMap);
//        printf("The actual size of FIB is %d\n", fibSize);
//        cout<<"冲突数目："<<getConflict(mFIB.mMap)<<endl;
//        cout<<"桶数量："<<gettableSize(mFIB.mMap)<<endl;
//        cout<<"负载因子："<<gethtLF(mFIB.mMap)<<endl;

//        long long fibSize = mFIB.mMap.size();
//        printf("The actual size of FIB is %lld\n", fibSize);
//        cout<<"桶数量："<<mFIB.mMap.bucket_count()<<endl;
//        cout<<"负载因子："<<mFIB.mMap.load_factor()<<endl;

//        string getMem = "ps -aux | grep fib > "+ std::to_string(i) + "_10.txt";
//        cout << getMem << endl;
//        system(getMem.data());

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


    string getMem = "ps -aux | grep fib > BF.txt";
    cout << getMem << endl;
    system(getMem.data());


    //cout<<sizeof(size_t) + sizeof(void*)<<endl;  //每个桶结构的大致消耗


//    cout<<"最大桶数量："<<mFIB.mMap.max_bucket_count()<<endl;
//    cout<<"桶数量："<<mFIB.mMap.bucket_count()<<endl;
//    cout<<"最大负载因子："<<mFIB.mMap.max_load_factor()<<endl;
//    cout<<"负载因子："<<mFIB.mMap.load_factor()<<endl;
//    mFIB.mMap.rehash(MAX); //通过rehash设置哈希桶数量，进一步提高效率
//    cout<<"桶数量："<<mFIB.mMap.bucket_count()<<endl;
//    cout<<"负载因子："<<mFIB.mMap.load_factor()<<endl;


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

    gettimeofday(&endTime,NULL);
    a5 = endTime.tv_sec - startTime.tv_sec + (double)(endTime.tv_usec - startTime.tv_usec)/1e6;
    printf("Total Time: %f(s)\n", a5);


//    cout<<"最大桶数量："<<mFIB.mMap.max_bucket_count()<<endl;
//    cout<<"桶数量："<<mFIB.mMap.bucket_count()<<endl;
//    cout<<"最大负载因子："<<mFIB.mMap.max_load_factor()<<endl;
//    cout<<"负载因子："<<mFIB.mMap.load_factor()<<endl;

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