#ifndef ARRAY_H
#define ARRAY_H

#include <utility>
namespace Czar{
template<class T>
class Array {
public:
    Array(int _size):size(_size){}
    void push(T value){
        for(int i=0;i<size;i++){
            if(data[i].first==false){
                data[i].second=value;
                data[i].first=true;
                ++pointer;
                return;
            }
        }
        // if(pointer+1<size && data[pointer+1].first==false){
        //     pointer++;
        //     data[pointer].second=value;
        //     data[pointer].first=true;
        //     return;
        // }
        throw ("array full");
    }
    void pop(){
        if(pointer==-1){
            throw ("empty");
            return;
        }
        pointer--;
    }
    bool& busy(int pos)
    {
        return data[pos].first;
    }
    T& operator[](int pos){
        data[pos].first=true;
        return data[pos].second;
    }
    void clean() { 
        for(int i=0;i<=size-1;i++){
            data[i].first=false;
        }
        pointer = -1; 
    }
    void take_out(int pos) { data[pos].first = false; pointer--;}
    // int get_pointer() { return pointer; }
    bool full() { return pointer == size - 1; }

private:
    std::pair<bool,T> data[1024]{{false,T()}};
    int pointer=-1;
    const int size=0;
};
}
#endif // ARRAY_H