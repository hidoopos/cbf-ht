#ifndef H_NDN_HASH_BASED_FIB
#define H_NDN_HASH_BASED_FIB

#include "NDN-Footprint.hpp"

using namespace std;


class HashBasedFIB{
public:
    //接口

    //最长前缀匹配(通过二分查找)
    ForwardInfo* LPM_search(const string& input) const;

    //对比用
    ForwardInfo* LPM_search_no_bt(const string& input) const;

    //最长前缀匹配(通过线性查找)
    ForwardInfo* linear_search(const string& input) const;

    //前缀插入
    void insert(const string& input, const ForwardInfo& info);

    //前缀删除
    bool erase(const string& input);


private:
    struct AuxTreeNode {
        //转发信息
        ForwardInfo* info;

        //该节点的属性：实，半虚，虚
        enum NodeType { Real, SemiVirtual, Virtual };
        NodeType type;

        //树指针：父节点，兄弟节点，子节点
        AuxTreeNode* parent;
        AuxTreeNode* nextSibling;
        AuxTreeNode* firstChild;
        AuxTreeNode():
                type(Virtual), parent(NULL), nextSibling(NULL),
                firstChild(NULL), info(NULL) {}
        ~AuxTreeNode();

        //树操作
        void addChild(AuxTreeNode* nChild);
        void removeChild(AuxTreeNode* nChild);
        void setNonrealNodesInSubtreeTo(NodeType type);
        void removeAllChild();
    };
    AuxTreeNode* root; //辅助树的根

public:

#if FOOTPRINT_AS_KEY_ON //若采用footprint作为哈希表的key
    typedef unordered_map<Footprint, AuxTreeNode*, hash_FP, cmp_FP> _FIB_HT;
#else //否则使用全名字作为key
    typedef unordered_map<string, AuxTreeNode*> _FIB_HT;
#endif
    _FIB_HT mMap;


    //debug 显示全部表项
    void printAllEntry() const;

    HashBasedFIB() { root = new AuxTreeNode; }
    ~HashBasedFIB();

private:
    static int lookup_times;
    static int no_matchs;
public:
    static int get_lookup_times() { return lookup_times; }
    static void reset_lookup_times() { lookup_times = 0; }
    static int get_no_matchs() { return no_matchs; }
    static void reset_no_matchs() { no_matchs = 0; }
    int getNodeSize() const{
        return sizeof(AuxTreeNode);
    }
}; // !class HashBasedFIB


#endif // !H_NDN_HASH_BASED_FIB

