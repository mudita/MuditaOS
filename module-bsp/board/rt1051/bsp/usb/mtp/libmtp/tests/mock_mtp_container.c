#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "mtp_container.h"

int mtp_container_get_param_count(const mtp_op_cntr_t *c)
{
    return (int)mock(c);
}

