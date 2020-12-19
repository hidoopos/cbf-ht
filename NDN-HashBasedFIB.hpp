#ifndef H_NDN_HASH_BASED_FIB
#define H_NDN_HASH_BASED_FIB

#include "NDN-Footprint.hpp"

using namespace std;


class HashBasedFIB{
public:
    //�ӿ�

    //�ǰ׺ƥ��(ͨ�����ֲ���)
    ForwardInfo* LPM_search(const string& input) const;

    //�Ա���
    ForwardInfo* LPM_search_no_bt(const string& input) const;

    //�ǰ׺ƥ��(ͨ�����Բ���)
    ForwardInfo* linear_search(const string& input) const;

    //ǰ׺����
    void insert(const string& input, const ForwardInfo& info);

    //ǰ׺ɾ��
    bool erase(const string& input);


private:
    struct AuxTreeNode {
        //ת����Ϣ
        ForwardInfo* info;

        //�ýڵ�����ԣ�ʵ�����飬��
        enum NodeType { Real, SemiVirtual, Virtual };
        NodeType type;

        //��ָ�룺���ڵ㣬�ֵܽڵ㣬�ӽڵ�
        AuxTreeNode* parent;
        AuxTreeNode* nextSibling;
        AuxTreeNode* firstChild;
        AuxTreeNode():
                type(Virtual), parent(NULL), nextSibling(NULL),
                firstChild(NULL), info(NULL) {}
        ~AuxTreeNode();

        //������
        void addChild(AuxTreeNode* nChild);
        void removeChild(AuxTreeNode* nChild);
        void setNonrealNodesInSubtreeTo(NodeType type);
        void removeAllChild();
    };
    AuxTreeNode* root; //�������ĸ�

public:

#if FOOTPRINT_AS_KEY_ON //������footprint��Ϊ��ϣ���key
    typedef unordered_map<Footprint, AuxTreeNode*, hash_FP, cmp_FP> _FIB_HT;
#else //����ʹ��ȫ������Ϊkey
    typedef unordered_map<string, AuxTreeNode*> _FIB_HT;
#endif
    _FIB_HT mMap;


    //debug ��ʾȫ������
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

