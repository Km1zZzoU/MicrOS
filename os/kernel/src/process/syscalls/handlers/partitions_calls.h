#ifndef PARTITIONS_CALLS_H
#define PARTITIONS_CALLS_H

#include "filesystems/partitions/partitions.h"

void syscall_partitions_get_count(interrupt_state *state);

#endif