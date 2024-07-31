#include "unit/predictor.h"
namespace Czar{
    Predictor::Predictor(){
        for(int i=0;i<32;i++)
        {
            predictor_mem[i][0]=0;
            predictor_mem[i][1]=0;
        }
    }
    int Predictor::Hash(int ins_addr){
        int index = (ins_addr/4) % 32;
        return index;
    }
    bool Predictor::GetPridictor(int ins_addr){
        int index = Hash(ins_addr);
        return predictor_mem[index][1];
    }
    void Predictor::step(int ins_addr,int branch_match){
        int index = Hash(ins_addr);
        if(predictor_mem[index][1]!=branch_match)
        {
            match_rate.second=(match_rate.second*match_rate.first)/(match_rate.first+1);
            ++match_rate.first;
        }
        else{
            match_rate.second=(match_rate.second*match_rate.first+1)/(match_rate.first+1);
            ++match_rate.first;
        }
        if(predictor_mem[index][1]==1)
        {
            if(branch_match==0 && predictor_mem[index][0]>0)
            {
                predictor_mem[index][1]=0;
                predictor_mem[index][0]=0;
            }
            else if(branch_match==0){
                predictor_mem[index][0]=1;
            }
            else{
                predictor_mem[index][0]=0;
            }
        }
        else{
            if(branch_match==1 && predictor_mem[index][0]>0)
            {
                predictor_mem[index][1]=1;
                predictor_mem[index][0]=0;
            }
            else if(branch_match==1){
                predictor_mem[index][0]=1;
            }
            else{
                predictor_mem[index][0]=0;
            }
        }
    }
    void Predictor::get_match_rate(){
        std::cout<<"total_num:"<<match_rate.first<<std::endl;
        std::cout<<"match_rate:"<<match_rate.second*100<<std::endl;
    }
} //namespace Czar