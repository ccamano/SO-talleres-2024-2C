#ifndef LIGHTSWITCH_HPP
#define LIGHTSWITCH_HPP

#include <mutex>
#include <array>
#include <atomic>
#include <string>
#include <vector>

class Lightswitch {
 public:
   Lightswitch();
    void lock(mutex &m);
    void unlock(mutex &m);

 private:
   int counter;
   mutex l_mutex;
};

#endif  /* HMC_HPP */
