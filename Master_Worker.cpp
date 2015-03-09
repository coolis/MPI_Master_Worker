#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <mpi.h>
#include "Master_Worker.h"

using namespace std;

//implementation of MPI_Run

Master_Worker::Master_Worker() {}
Master_Worker::Master_Worker(int wk_sz, int rs_sz, int m) : work_sz(wk_sz), result_sz(rs_sz), mode(m) {}

void Master_Worker::Run(){
    //have the rank, have the size, devide into 0 and 1
    sz = MPI::COMM_WORLD.Get_size();
    if (sz < 2) {
        cout << "Please assign at least two processors." << endl;
        exit(0);
    }
    rank = MPI::COMM_WORLD.Get_rank();
    cout << "Processor: " << rank << " starts working." << endl;

    //choose the mode, 1 is assign mdoe, 0 is direct mode
    if (mode == 1) {
        assignMode();
    } else {
        directMode();
    }
}

void Master_Worker::directMode() {
    int tag;
    create();
    int n = wPool.size();
    int wNum = sz-1;
    vector<result_t*> rList;
    if(rank == MASTER_RANK){
        for(int i = 1; i < sz; i++){
            result_t* tmpR = (result_t*) malloc(result_sz);
            MPI::COMM_WORLD.Recv(tmpR, result_sz, MPI::INT, i, tag);
            rList.push_back(tmpR);
        }
        //get the final result
        result(rList, finalR);
    }
    else{
        int tmpSize = n/wNum;
        int tmpRem = n%wNum;
        result_t* tmpR;
        for(int i = (rank-1)*tmpSize; i < rank*tmpSize; i++){
            tmpR = compute(wPool[i]);
            rList.push_back(tmpR);
        }
        if(rank <= tmpRem){
            tmpR = compute(wPool[n-rank]);
            rList.push_back(tmpR);
        }
        result(rList, finalR);
        MPI::COMM_WORLD.Send(finalR, result_sz, MPI::INT, 0, tag);
    }
    
}

void Master_Worker::assignMode() {
    int tag;
    vector<result_t*> rList;
    if (rank == MASTER_RANK) {
        create();
        
    }

}


result_t* Master_Worker::getResult() {
    return finalR;
}


void Master_Worker::setWorkSize(int sz) {
    work_sz = sz;
}

void Master_Worker::setResultSize(int sz) {
    result_sz = sz;
}

bool Master_Worker::isMaster() {
    if (rank == MASTER_RANK)
        return true;
    else
        return false;
}
