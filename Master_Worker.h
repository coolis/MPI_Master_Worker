#ifndef ____Master_Worker__
#define ____Master_Worker__

#include <vector>
#include <mpi.h>

using namespace std;

struct work_t;
struct result_t;

class Master_Worker{
public:
    Master_Worker();
    Master_Worker(int wk_sz, int rs_sz, int m=1);
    
    //Run Function, Master Worker implementation
    void Run();
    
    result_t* getResult();
    void setWorkSize(int sz);
    void setResultSize(int sz);
    bool isMaster();

protected:
    vector<work_t*> wPool;
    result_t* finalR;
    
    //construct pList and wPool based on the input value
    virtual void create() = 0;
    
    //master process the results after recieving from workers
    virtual int result(vector<result_t*> &res, result_t* &tmpR) = 0;
    
    //distribute the works, and compute each portion of the list based on the rank number
    virtual result_t* compute(work_t* work) = 0;

private:
    int rank, sz;
    int work_sz, result_sz;
    int mode;
    const int MASTER_RANK = 0;
    MPI::Status status;

    void directMode();
    void assignMode();
};

#endif
