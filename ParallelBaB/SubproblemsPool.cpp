//
//  SubproblemsPool.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/6/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "SubproblemsPool.hpp"

SubproblemsPool::SubproblemsPool():
size_emptying(4),
max_limit_size(40),
size(0) {

}

std::ostream &operator<<(std::ostream& stream, SubproblemsPool& shared_pool) {

    tbb::queuing_rw_mutex::scoped_lock m_lock(shared_pool.updating_lock, true);

    std::vector<Interval> to_restore;
    Interval interval;

    stream << "pool_size: " << shared_pool.unsafe_size();
    for (size_t element = 0; element < shared_pool.unsafe_size(); ++element)
        if(shared_pool.try_pop(interval)) {
            to_restore.push_back(interval);
            stream << interval;
        }

    /** This have to be used only when the progress is saved and then we have to recover the data to continue with the exploration. **/
    for (const auto& sub_problem : to_restore)
        shared_pool.push(sub_problem);

    stream << std::endl;

    return stream;
}

/** If the global pool reach this size then the B&B tasks starts sending part of their work to the global pool. **/
void SubproblemsPool::setSizeEmptying(unsigned long size) {
    size_emptying = size;
    max_limit_size = size_emptying * 10;
}

unsigned long SubproblemsPool::getSizeEmptying() const {
    return size_emptying;
}

unsigned long SubproblemsPool::unsafe_size() const {
    return size;
}

bool SubproblemsPool::isMaxLimitReached() const {
    return size > max_limit_size;
}

bool SubproblemsPool::empty() const {
    return size > 0 ? false : true;
}

bool SubproblemsPool::isEmptying() const {
    return size < size_emptying ? true : false;
}

void SubproblemsPool::push(const Interval & subproblem) {
    size.fetch_and_increment();
    if (isMaxLimitReached())
        std::cout << "SubProblemsPool: max_limit reached by " << size << std::endl;

    priority_queues[subproblem.getPriority()].push(subproblem);
}

bool SubproblemsPool::try_pop(Interval & interval) {
    for(int p_queue = P_High; p_queue <= P_Low; ++p_queue) // Scan queues in priority order for a subproblem.
        if(priority_queues[p_queue].try_pop(interval)) {
            size.fetch_and_decrement();
            return true;
        }
    return false;
}

void SubproblemsPool::print() const {
    unsigned long total_size = unsafe_size();
    printf("Size:%4lu\t[Hi:%4lu\tMe:%4lu\tLo:%4lu\t]\n", total_size, priority_queues[P_High].unsafe_size(), priority_queues[P_Normal].unsafe_size(), priority_queues[P_Low].unsafe_size());
}
