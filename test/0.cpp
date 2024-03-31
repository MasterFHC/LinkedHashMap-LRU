#include <bits/stdc++.h>
#include "lru.hpp"
using namespace std;
void get_info(auto it){
    cout<<"pre="<<it.get()->pre<<";now="<<it.get()<<";nxt="<<it.get()->nxt<<endl;
}
void travel(sjtu::double_list<int> &test){
    auto it = test.end();
    if(it == test.begin()){
        cout<<"empty!"<<endl;
        return;
    }
    it--;
    for(;;){
        cout<<(*it)<<" ";
        if(it == test.begin()) break;
        it--;
    }
    cout<<endl;
}
int main(){
    sjtu::double_list<int> test;
    try{
        cout<<test.empty()<<endl;
        travel(test);
        for(int i=1;i<=5;i++){
            test.insert_tail(i);
        }
        travel(test);
        for(int i=6;i<=10;i++){
            test.insert_head(i);
        }
        travel(test);
        auto it = test.begin();
        ++it, it--, it++, it++, --it;
        cout<<(*it)<<endl;
        for(int i=1;i<=3;i++){
            test.erase(it);
            travel(test);
        }
        cout<<test.empty()<<endl;
        sjtu::double_list<int> test2(test);
        travel(test2);
        test2.delete_head();
        travel(test);
        travel(test2);
        
    } catch(const char* &e){
        cout<<e<<endl;
    }
    return 0;
}