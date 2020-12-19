

#if NDN_HASH_BASED_FIB_DEBUG_ON
#define fib_debug(M, ...) printf(M, __VA_ARGS__)
#else
#define fib_debug(M, ...)
#endif //!NDN_HASH_BASED_FIB_DEBUG_ON

#include "NDN-HashBasedFIB.hpp"

int HashBasedFIB::lookup_times = 0;
int HashBasedFIB::no_matchs = 0;
//输入应形如"/s1/s2/s3/s4..."
ForwardInfo* HashBasedFIB::LPM_search(const string& input) const {
    //统计输入字串的组件数目 以及各组件的开始下标
    fib_debug("[LPM BINARY] To search prefix %s:\n", input.c_str())
    int slash_pos[2 + MAX_COMP_NUM];
    int comp_num = 0;
    for (unsigned int i = 0; i < input.length(); i++) {
        if (input[i] == '/') {
            slash_pos[comp_num] = i;
            comp_num++;
        }
    }
    slash_pos[comp_num] = input.length();
    if (comp_num == 0) { //输入错误
        printf("Wrong Input\n"); system("pause");
        exit(-1);
    }
    //二分查找
    _FIB_HT::const_iterator lastHit = mMap.end();
    string lastHitStr;
    _FIB_HT::const_iterator res;
    int high = comp_num; int low = 1;
    while (low <= high) {
        lookup_times++;
        int mid = (high + low) / 2;
        res = mMap.find(input.substr(0, slash_pos[mid]));
        fib_debug("    L = %d, Try prefix %s: ", mid, input.substr(0, slash_pos[mid]).c_str());
        if (res != mMap.end()) { //命中
            fib_debug("HIT.\n")
            lastHit = res;
            lastHitStr = input.substr(0, slash_pos[mid]);
            low = mid + 1;
        }
        else {
            fib_debug("MISS.\n")
            high = mid - 1;
        }
    }
    //检验查询结果
    if (lastHit != mMap.end()) {
        fib_debug("    Last hit %s ", lastHitStr.c_str());
        if (lastHit->second->type == AuxTreeNode::Real) {
            //搜索结果为实前缀
            //printf("match!\n");
            fib_debug("is real.\n", );
            ForwardInfo* resInfo = new ForwardInfo(*(lastHit->second->info));
            return resInfo;
        }
        else if (lastHit->second->type == AuxTreeNode::SemiVirtual) {
            //搜索结果为半虚前缀 则向上回溯直到找到一个实前缀 (理论上一定存在)
            fib_debug("is semi-virtual.\n")
            AuxTreeNode* ptr = lastHit->second;
            int end_pos = lastHitStr.length() + 1;
            while (ptr->type != AuxTreeNode::Real) {
                lookup_times++;
                ptr = ptr->parent;
                end_pos--;
                while (lastHitStr[end_pos] != '/')
                    end_pos--;
            }
            //printf("match!\n");
            fib_debug("    Forward info is copied from %s.\n",
                      lastHitStr.substr(0, end_pos).c_str());
            ForwardInfo* resInfo = new ForwardInfo(*(ptr->info));
            return resInfo;
        }
        else {
            //搜索结果为虚前缀
            //printf("No name prefix match!\n");
            no_matchs++;
            fib_debug("is virtual.\n");
        }
    }
    else {
        //搜索失败
        //printf("No name prefix match!\n");
        no_matchs++;
        fib_debug("    No prefix found.\n")
    }
    return NULL;
}

ForwardInfo * HashBasedFIB::LPM_search_no_bt(const string & input) const
{
    //统计输入字串的组件数目 以及各组件的开始下标
    fib_debug("[LPM BINARY] To search prefix %s:\n", input.c_str())
    int slash_pos[2 + MAX_COMP_NUM];
    int comp_num = 0;
    for (unsigned int i = 0; i < input.length(); i++) {
        if (input[i] == '/') {
            slash_pos[comp_num] = i;
            comp_num++;
        }
    }
    slash_pos[comp_num] = input.length();
    if (comp_num == 0) { //输入错误
        printf("Wrong Input\n"); system("pause");
        exit(-1);
    }
    //二分查找
    _FIB_HT::const_iterator lastHit = mMap.end();
    string lastHitStr;
    _FIB_HT::const_iterator res;
    int high = comp_num; int low = 1;
    while (low <= high) {
        lookup_times++;
        int mid = (high + low) / 2;
        res = mMap.find(input.substr(0, slash_pos[mid]));
        fib_debug("    L = %d, Try prefix %s: ", mid, input.substr(0, slash_pos[mid]).c_str());
        if (res != mMap.end()) { //命中
            fib_debug("HIT.\n")
            lastHit = res;
            lastHitStr = input.substr(0, slash_pos[mid]);
            low = mid + 1;
        }
        else {
            fib_debug("MISS.\n")
            high = mid - 1;
        }
    }
    //检验查询结果
    if (lastHit != mMap.end()) {
        fib_debug("    Last hit %s ", lastHitStr.c_str());
        if (lastHit->second->type == AuxTreeNode::Real) {
            //搜索结果为实前缀
            //printf("match!\n");
            fib_debug("is real.\n", );
            ForwardInfo* resInfo = new ForwardInfo(*(lastHit->second->info));
            return resInfo;
        }
        else {
            //搜索结果为非实前缀
            //printf("No name prefix match!\n");
            no_matchs++;
            fib_debug("is non-real.\n");
        }
    }
    else{
        //搜索失败
        //printf("No name prefix match!\n");
        no_matchs++;
        fib_debug("    No prefix found.\n")
    }
    return NULL;
}


ForwardInfo * HashBasedFIB::linear_search(const string & input) const {
    fib_debug("[LINEAR] To search prefix %s:\n", input.c_str())
    int slash_pos[MAX_COMP_NUM + 1];
    int comp_num = 0;
    for (unsigned int i = 0; i < input.length(); i++) {
        if (input[i] == '/') {
            slash_pos[comp_num] = i;
            comp_num++;
        }
    }
    slash_pos[comp_num] = input.length();
    if (comp_num == 0) { //输入错误
        printf("Wrong Input\n"); system("pause");
        exit(-1);
    }
    _FIB_HT::const_iterator res;
    for (int i = comp_num; i > 0; i--) {
        lookup_times++;
        res = mMap.find(input.substr(0, slash_pos[i]));
        fib_debug("    L = %d, Try prefix %s: ", i, input.substr(0, slash_pos[i]).c_str());
        if ((res != mMap.end()) &&( res->second->type == AuxTreeNode::Real)) { //命中
            //printf("match!\n");
            fib_debug("HIT.\n")
            return res->second->info;
        }
        else {
            fib_debug("MISS.\n")
        }
    }
    //printf("No name prefix match!\n");
    no_matchs++;
    return NULL;
}

//树操作 添加子节点
void HashBasedFIB::AuxTreeNode::addChild(AuxTreeNode* nChild) {
    if (nChild == NULL){
        printf("null\n");
        return;
    }
    if (this == NULL){
        printf("null\n");
        return;
    }
    nChild->nextSibling = this->firstChild;
    nChild->parent = this;
    this->firstChild = nChild;
}

//树操作 删除子节点
void HashBasedFIB::AuxTreeNode::removeChild(AuxTreeNode* nChild) {
    if (nChild == NULL){
        printf("null\n");
        return;
    }
    if (this == NULL){
        printf("null\n");
        return;
    }
    if (this != nChild->parent)
        return;
    if (this->firstChild == nChild)
        this->firstChild = nChild->nextSibling;
    else {
        AuxTreeNode* pre = this->firstChild;
        while (pre->nextSibling != NULL) {
            if (pre->nextSibling == nChild) {
                pre->nextSibling = nChild->nextSibling;
                break;
            }
            pre = pre->nextSibling;
        }
    }
    nChild->nextSibling = NULL;
    nChild->parent = NULL;
}

//树操作 将子树中截至实前缀的所有节点设置为虚/半虚
void HashBasedFIB::AuxTreeNode::setNonrealNodesInSubtreeTo(
        AuxTreeNode::NodeType type) {
    if (this == NULL){
        printf("null\n");
        return;
    }
    if(this->type != Real) this->type = type;
    AuxTreeNode* ptr = this->firstChild;
    while (ptr != NULL) {
        if (ptr->type != Real)
            ptr->setNonrealNodesInSubtreeTo(type);
        ptr = ptr->nextSibling;
    }
}

//树操作 删除所有子节点
void HashBasedFIB::AuxTreeNode::removeAllChild() {
    if (this == NULL){
        printf("null\n");
        return;
    }
    AuxTreeNode* temp;
    while (this->firstChild != NULL) {
        temp = this->firstChild->nextSibling;
        this->firstChild->removeAllChild();
        delete this->firstChild;
        this->firstChild = temp;
    }
}

void HashBasedFIB::insert(const string& input, const ForwardInfo& info) {
    fib_debug("[INSERT] To insert prefix %s:\n", input.c_str());
    _FIB_HT::iterator it = mMap.find(input);
    if (it != mMap.end()) {
        //已经存在相应表项
        fib_debug("    %s already exists in table, ", input.c_str())
        switch (it->second->type) {
            case AuxTreeNode::Virtual:
                //为虚表项，设置转发信息，修改本表项为实，并将子树中的虚修改为半虚
                it->second->type = AuxTreeNode::Real;
                it->second->setNonrealNodesInSubtreeTo(AuxTreeNode::SemiVirtual);
            case AuxTreeNode::SemiVirtual:
                //为半表项，设置转发信息，修改本表项为实
                it->second->type = AuxTreeNode::Real;
                it->second->info = new ForwardInfo(info);
                fib_debug("its entry has been modified to real.\n")
                break;
            case AuxTreeNode::Real:
                //为实表项，替换转发信息
                *(it->second->info) = info;
                fib_debug("its forward info has been updated.\n")
        }
    }
    else {
        //不存在相应表项，在原有基础上延长
        //建立实节点
        AuxTreeNode *nNode, *preNode;
        nNode = new AuxTreeNode;
        nNode->type = AuxTreeNode::Real;
        nNode->info = new ForwardInfo(info);
        mMap.insert(make_pair(input, nNode));
        fib_debug("    %s doesn't exist in table, insert real entry.\n", input.c_str())
        int end_pos = input.length();
        //向上递归地建立虚/半虚节点
        while (1) {
            while (input[end_pos] != '/')
                end_pos--;
            if (end_pos == 0) {
                //已遍历完该名字的前缀
                root->addChild(nNode);
                return;
            }
            //查找前一级前缀是否存在于表中
            it = mMap.find(input.substr(0, end_pos));
            if (it != mMap.end()) {
                //前一级前缀已存在，连接到该节点上
                it->second->addChild(nNode);
                //如果该节点为实或半虚，则将其子树的所有非实节点改为半虚
                if (it->second->type != AuxTreeNode::Virtual) {
                    nNode->setNonrealNodesInSubtreeTo(AuxTreeNode::SemiVirtual);
                }
                return;
            } else {
                //前一级前缀不存在于表中，创建虚节点
                preNode = nNode;
                nNode = new AuxTreeNode;
                nNode->type = AuxTreeNode::Virtual;
                nNode->addChild(preNode);
                mMap.insert(make_pair(input.substr(0, end_pos), nNode));
                fib_debug("    Insert non-real entry %s.\n", input.substr(0, end_pos).c_str());
                end_pos--;
            }
        }
    }

}

bool HashBasedFIB::erase(const string& input) {
    fib_debug("[ERASE] To erase prefix %s:\n", input.c_str());
    _FIB_HT::iterator it = mMap.find(input);
    if (it == mMap.end()) { //表中不存在待删除项
        fib_debug("    %s doesn't exists in table.\n", input.c_str());
        return false;
    }
    if (it->second->firstChild != NULL) {
        fib_debug("    %s has child nodes, thus modified to ", input.c_str());
        //若存在子女 则该节点不移除 而是作为虚/半虚节点存在
        delete it->second->info;
        it->second->info = NULL;
        if (it->second->parent->type == AuxTreeNode::Virtual) {
            //上级节点为虚，则设置为虚
            fib_debug("virtual, as well as its subtree.\n");
            it->second->setNonrealNodesInSubtreeTo(
                    AuxTreeNode::Virtual);
        }
        else {
            fib_debug("semi-virtual\n");
            //上级节点为实/半虚，则设置为半虚，子树不修改
            it->second->type = AuxTreeNode::SemiVirtual;
        }
    }
    else {
        fib_debug("    %s has no child nodes, thus removed.\n", input.c_str());
        //不存在子女 则向上删除
        //pre指待删除节点 cur指pre的父母
        AuxTreeNode* curNode, *preNode = it->second;
        mMap.erase(input);
        int end_pos = input.length() + 1;
        while (1) {
            curNode = preNode->parent;
            curNode->removeChild(preNode);
            //判断上一级节点是否需要删除
            if (curNode->firstChild == NULL &&
                curNode->type != AuxTreeNode::Real &&
                curNode != root) {
                //上级节点是没有子女的非实节点，故删除
                delete preNode;
                preNode = curNode;
                //在表中删除对应前缀
                end_pos--;
                while (input[end_pos] != '/')
                    end_pos--;
                fib_debug("    %s is non-real and has no child nodes, thus removed.\n", input.substr(0, end_pos).c_str());
                mMap.erase(input.substr(0, end_pos));
            }
            else
                //上级节点无需删除
                break;
        }
        delete preNode;
    }
    return true;
}

void HashBasedFIB::printAllEntry() const {
    printf("[PRINT ENTRY]\n");
    for (_FIB_HT::const_iterator it = mMap.begin();
         it != mMap.end(); it++) {
        switch (it->second->type) {
            case AuxTreeNode::Real:
                printf("     R: "); break;
            case AuxTreeNode::SemiVirtual:
                printf("     S: "); break;
            case AuxTreeNode::Virtual:
                printf("     V: "); break;
        }
#if FOOTPRINT_AS_KEY_ON
        it->first.print();
		printf("\n");
#else
        printf("%s\n", it->first.c_str());
#endif
    }
}

HashBasedFIB::AuxTreeNode::~AuxTreeNode() {
    delete info;
}

HashBasedFIB::~HashBasedFIB() {
    root->removeAllChild();
    delete root;
}
