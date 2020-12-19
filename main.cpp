#include <iostream>
#include "NDN-HashBasedFIB.hpp"
#include "NDN-PrefixGenerator.hpp"
#include "BinarySearch.hpp"
#include "CBF-HT.h"
#include <sys/time.h>
#include <fstream>
using namespace std;


#define MAX 1e8  //整数范围 1 ～ MAX
#define N 1

struct timeval	gStartTime,gEndTime;
struct timeval	startTime,endTime;
struct timeval	st,et;

int seed = time(0);

HashBasedFIB HPT;
//BinarySearch mFIB(MAX*10);
CBF_HT mFIB(MAX*1);
int operation_time = 1e6;

ForwardInfo info;
uint64 hM = 1000;

PrefixGenerator gen(seed);
typedef unordered_map<string, uint64> prefixs;
prefixs baseFIB;
vector<string> vec;
double temp;

void generate_names(){
    gettimeofday(&st,NULL);
    for (uint64 i = 0; i < MAX; i++){
        while(1){
            string name = gen.generate_shorter_prefix();
            prefixs::iterator it = baseFIB.find(name);
            if (it == baseFIB.end()){
                baseFIB.insert(make_pair(name, i));
                //cout << name << endl;
                break;
            }
        }
    }
    printf("The actual size of baseFIB is %lld\n", baseFIB.size());
    printf("generate_baseFIB finished\n");
    gettimeofday(&et, NULL);
    temp = et.tv_sec - st.tv_sec + (double) (et.tv_usec - st.tv_usec) / 1000000;
    printf("Use Time: %f(s)\n", temp);

    printf("write names into file\n");
    ofstream out("./names_dataset.txt");
    if(out.is_open())
    {
        gettimeofday(&st,NULL);
        for (int i = 0; i < N; i++) {
            for (auto &x: baseFIB){
                //cout << " " << x.first << ":" << x.second << endl;
                std::string ht = "/ht" + std::to_string(i) + x.first;
                //cout << ht << endl;
                out << ht << endl;
            }
            gettimeofday(&et, NULL);
            temp = et.tv_sec - st.tv_sec + (double) (et.tv_usec - st.tv_usec) / 1000000;
            printf("%de8 names have been written into file\n",i+1);
            printf("Use Time: %f(s)\n", temp);
        }
        gettimeofday(&et, NULL);
        temp = et.tv_sec - st.tv_sec + (double) (et.tv_usec - st.tv_usec) / 1000000;
        printf("write_names_into_file finished\n");
        printf("Total Use Time: %f(s)\n", temp);

        out.close();
    }
}

//插入名字前缀
void insert_prefixes(){
    char buffer[1024];
    ifstream in("./test.txt");
    if (!in.is_open()){
        cout << "Error opening name file"; exit(1);
    }
    uint64 cnt = 0;
    gettimeofday(&st,NULL);
    while (!in.eof())
    {
        in.getline(buffer,100);
        string name = buffer;
        //cout << name << endl;
        mFIB.insert(name, info);
        cnt++;
        if ((cnt % hM) == 0) {
            gettimeofday(&et, NULL);
            temp = et.tv_sec - st.tv_sec + (double) (et.tv_usec - st.tv_usec) / 1000000;
            printf("Use Time: %f(s)\n", temp);

            uint64 fibSize = getContentSize(mFIB.mMap);
            printf("The actual size of FIB is %lld\n", fibSize);
            cout << "冲突数目：" << getConflict(mFIB.mMap) << endl;
            cout << "桶数量：" << gettableSize(mFIB.mMap) << endl;
            cout << "负载因子：" << gethtLF(mFIB.mMap) << endl;
            string getMem = "ps -aux | grep fib > " + std::to_string(cnt/hM) + "_HPT.txt";
            cout << getMem << endl;
            system(getMem.data());
        }
        //if(cnt>=10) break;
    }
}

void generate_insert_names(){
    gettimeofday(&st,NULL);
    for (uint64 i = 0; i < MAX; i++){
        while(1){
            string name = gen.generate_shorter_prefix();
            prefixs::iterator it = baseFIB.find(name);
            if (it == baseFIB.end()){
                baseFIB.insert(make_pair(name, i));
                //cout << name << endl;
                break;
            }
        }
    }
    printf("The actual size of baseFIB is %lld\n", baseFIB.size());
    printf("generate_baseFIB finished\n");
    gettimeofday(&et, NULL);
    temp = et.tv_sec - st.tv_sec + (double) (et.tv_usec - st.tv_usec) / 1000000;
    printf("Use Time: %f(s)\n", temp);

    printf("insert names into FIB\n");

    //gettimeofday(&st,NULL);
    for (int i = 0; i < N; i++) {
            for (auto &x: baseFIB){
                //cout << " " << x.first << ":" << x.second << endl;
                std::string ht = "/ht" + std::to_string(i) + x.first;
                mFIB.insert(ht, info);
                //cout << ht << endl;
            }
            gettimeofday(&et, NULL);
            temp = et.tv_sec - st.tv_sec + (double) (et.tv_usec - st.tv_usec) / 1000000;

            printf(">>>>>>>>Use Time: %f(s)\n", temp);
            printf("%de8 names have been inserted into FIB\n",i+1);
            uint64 fibSize = getContentSize(mFIB.mMap);
            printf("The actual size of FIB is %lld\n", fibSize);
            cout << "冲突数目：" << getConflict(mFIB.mMap) << endl;
            cout << "桶数量：" << gettableSize(mFIB.mMap) << endl;
            cout << "负载因子：" << gethtLF(mFIB.mMap) << endl;
            string getMem = "ps -aux | grep fib > " + std::to_string(i) + "_CBF-HT.txt";
            cout << getMem << endl;
            system(getMem.data());
    }
    gettimeofday(&et, NULL);
    temp = et.tv_sec - st.tv_sec + (double) (et.tv_usec - st.tv_usec) / 1000000;
    printf("insert_names_into_FIB finished\n");
    printf("Total Use Time: %f(s)\n", temp);

}

void generate_insert_names_and_query(){
    gettimeofday(&st,NULL);
    for (uint64 i = 0; i < MAX; i++){
        while(1){
            string name = gen.generate_shorter_prefix();
            prefixs::iterator it = baseFIB.find(name);
            if (it == baseFIB.end()){
                baseFIB.insert(make_pair(name, i));
                //cout << name << endl;
                break;
            }
        }
    }
    printf("The actual size of baseFIB is %lld\n", baseFIB.size());
    printf("generate_baseFIB finished\n");
    gettimeofday(&et, NULL);
    temp = et.tv_sec - st.tv_sec + (double) (et.tv_usec - st.tv_usec) / 1000000;
    printf("Use Time: %f(s)\n", temp);

    int count = 0;
    for (auto& x: baseFIB) {
        //cout << " " << x.first << ":" << x.second << endl;
        string ht = x.first + "/ht1/ht2/ht3/ht4/ht5/ht6";  //全hit
        //cout << ht << endl;
        vec.push_back(ht);
        count++;
        if(count == operation_time) break;
    }


    printf("insert names into FIB\n");

    //gettimeofday(&st,NULL);
    for (int i = 0; i < N; i++) {
        for (auto &x: baseFIB){
            //cout << " " << x.first << ":" << x.second << endl;
            //std::string ht = "/ht" + std::to_string(i) + x.first;
            std::string ht = x.first;
            //cout << ht << endl;
            mFIB.insert(ht, info);
            HPT.insert(ht, info);
        }
        gettimeofday(&et, NULL);
        temp = et.tv_sec - st.tv_sec + (double) (et.tv_usec - st.tv_usec) / 1000000;

        printf(">>>>>>>>Use Time: %f(s)\n", temp);
        //printf("%de8 names have been inserted into FIB\n",i+1);
        printf("\n%de8 names have been inserted into FIB\n",2);

        //HPT（有回溯
        HPT.reset_lookup_times();
        HPT.reset_no_matchs();
        gettimeofday(&gStartTime,NULL);
        for (int j = 0; j < operation_time; j++) {
            HPT.LPM_search(vec[j]); //printf("\n");
        }
        gettimeofday(&gEndTime,NULL);
        double a1 = 1000*(gEndTime.tv_sec - gStartTime.tv_sec) + (double)(gEndTime.tv_usec - gStartTime.tv_usec)/1000;
        printf(">>>>>>>>HPT With BT Use Time: %f(ms)\n", a1);
        printf("Average Lookup Times: %f\n", HPT.get_lookup_times() / (double)operation_time); //平均查找次数：访存次数
        printf("no match times: %d\n", HPT.get_no_matchs()); //没有匹配到名字的查找次数
        printf("\n");

        //CBF-HT（有回溯
        mFIB.reset_lookup_times();
        mFIB.reset_no_matchs();
        gettimeofday(&gStartTime,NULL);
        for (int j = 0; j < operation_time; j++) {
            mFIB.LPM_search(vec[j]); //printf("\n");
        }
        gettimeofday(&gEndTime,NULL);
        double a2 = 1000*(gEndTime.tv_sec - gStartTime.tv_sec) + (double)(gEndTime.tv_usec - gStartTime.tv_usec)/1000;
        printf(">>>>>>>>CBF-HT With BT Use Time: %f(ms)\n", a2);
        printf("Average Lookup Times: %f\n", mFIB.get_lookup_times() / (double)operation_time); //平均查找次数：访存次数
        printf("no match times: %d\n", mFIB.get_no_matchs()); //没有匹配到名字的查找次数
        printf("\n");
    }
    gettimeofday(&et, NULL);
    temp = et.tv_sec - st.tv_sec + (double) (et.tv_usec - st.tv_usec) / 1000000;
    printf("insert_names_into_FIB_and_query finished\n");
    printf("Total Use Time: %f(s)\n", temp);

}


int main()
{
//    cout << "ps -aux | grep fib > HPT.txt" << endl;
//    system("ps -aux | grep fib > HPT.txt");
//    cout << "ps -aux | grep fib > CBF-HT.txt" << endl;
//    system("ps -aux | grep fib > CBF-HT.txt");
    //generate_names();
    //insert_prefixes();
    //generate_insert_names();
    generate_insert_names_and_query();
    return 0;

}