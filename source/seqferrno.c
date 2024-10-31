#include "seqf_core.h"

_Thread_local int seqferrno_;

int *
seqfgeterrno(void)
{
	return &seqferrno_;
}
