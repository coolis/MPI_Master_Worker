#include "Master_Worker.h"
#include "mpi.h"
#include "gmp.h"
#include <vector>
#include <cmath>

using namespace std;

#define NUM_SIZE 256 
#define GRAIN 500
struct work_t {
    char start[NUM_SIZE]; 
    char end[NUM_SIZE];
};

struct result_t {
    char digits[GRAIN][NUM_SIZE] = {0};
};

class MW : public Master_Worker {
public:
    MW(int wk_sz, int rs_sz, char *n, int g, int m) : Master_Worker(wk_sz, rs_sz, m) {
        memcpy(this->n, n, sizeof(n)); 
        this->g = g;
    };

    void create() {
        mpz_t n, r;
        mpz_set_str(n, this->n, 10);
        mpz_sqrt(r, n); 


         

        mpz_t r, index, work_num;
        mpz_sqrt(r, n);
        mpz_fdiv_q(work_num, r, g);
        mpz_init(index); 
        while (mpz_cmp(work_num, index)) {
            work_t* w = new work_t();
            mpz_t tmp;
            mpz_mul(tmp, index, g);
            mpz_add_ui(tmp, tmp, 1);
            mpz_set(w->start, tmp);
            mpz_add_ui(tmp, index, 1);
            mpz_mul(tmp, tmp, g);
            mpz_set(w->end, tmp);
            wPool.push_back(w);
            mpz_add_ui(index, index, 1);
        }
        mpz_t remainder;
        mpz_mod(remainder, r, g); 
        if (mpz_sgn(remainder) == 1) {
            work_t* w = new work_t();
            mpz_t tmp;
            mpz_sub(tmp, r, remainder);
            mpz_add_ui(tmp, tmp, 1);
            mpz_set(w->start, tmp);
            mpz_set(w->end, r);
            wPool.push_back(w);
        }
        cout << wPool.size() << " works created" << endl;
    };

    int result(vector<result_t*> &res, result_t* &tmpR) {
        try {
            tmpR = new result_t();
            int index=0;
            for (int i=0; i<res.size(); i++) {
                for (int j=0; j<RESULT_SIZE; j++) {
                    if (mpz_sgn(res[i]->digits[j]) == 0)
                        break;
                    mpz_set(tmpR->digits[index], res[i]->digits[j]);
                    index++;
                }
            }
        } catch (int e) {
            return 0;
        }
        return 1; 
    }

    result_t* compute(work_t* &work) {
        result_t *res = new result_t();
        mpz_t i;
        int index;
        mpz_set(i, work->start);
        while(mpz_cmp(work->end, i)) {
            mpz_t remainder;
            mpz_mod(remainder, n, i);
            if (mpz_sgn(remainder) == 0) {
                mpz_t tmp;
                mpz_fdiv_q(tmp, n, i);
                mpz_set(res->digits[index++], i);
                mpz_set(res->digits[index++], tmp); 
            }
            mpz_add_ui(i, i, 1);
        }
        return res;
    }
private:
    char *n;
    int g;
};

int main(int argc, char *argv[]) {

    char *n;
    memcpy(n, argv[argc-3], sizeof(argv[argc-3]));
    int g = atoi(argv[argc-2]);
    int mode = atoi(argv[argc-1]);

    MPI::Init (argc, argv);

    Master_Worker *mw = new MW(sizeof(work_t), sizeof(result_t), n, g, mode);

    mw->Run();

    if (mw->isMaster()) {
        result_t *r = mw->getResult();
        cout << "The result:" << endl;
        for (int i=0; i<100; i++) {
            if (r->digits[i] == 0)
                break;
            cout << r->digits[i] << " ";
        }
        cout << endl;
    }

    MPI::Finalize ();

    return 0;
}
