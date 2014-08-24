#ifndef R1H_RANDOM_H
#define R1H_RANDOM_H

#include <climits>

namespace r1h {

class XorShift {
    unsigned int seed_[4];
public:
    unsigned int next(void) {
        const unsigned int t = seed_[0] ^ (seed_[0] << 11);
        seed_[0] = seed_[1];
        seed_[1] = seed_[2];
        seed_[2] = seed_[3];
        return seed_[3] = (seed_[3] ^ (seed_[3] >> 19)) ^ (t ^ (t >> 8));
    }
    
    double next01(void) {
        return (double)next() / UINT_MAX;
    }
    
    void setSeed(const unsigned int iseed) {
        unsigned int s = iseed;
        for(int i = 1; i <= 4; i++) {
            seed_[i - 1] = s = 1812433253U * (s ^ (s >> 30)) + i;
        }
    }
    
    XorShift(const unsigned int iseed) {
        setSeed(iseed);
    }
};
typedef XorShift Random;

}

#endif