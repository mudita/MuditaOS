#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "mtp_container.h"

static const uint8_t open_session_request[] = {
    0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x10, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
};

static const uint8_t open_session_response[] = {
    0x0c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00
};

static mtp_op_cntr_t* smap;

Describe(mtp_container);

BeforeEach(mtp_container)
{
    smap = (mtp_op_cntr_t*)open_session_request;
}

AfterEach(mtp_container)
{
}

Ensure(mtp_container, length)
{
    assert_that(smap->header.length, is_equal_to(16));
}

Ensure(mtp_container, type)
{
    assert_that(smap->header.type, is_equal_to(MTP_CONTAINER_TYPE_COMMAND));
}

Ensure(mtp_container, opcode_is_open_session)
{
    assert_that(smap->header.operation_code, is_equal_to(MTP_OPERATION_OPEN_SESSION));
}

Ensure(mtp_container, code_union)
{
    assert_that(smap->header.operation_code, is_equal_to(smap->header.response_code));
    assert_that(smap->header.response_code, is_equal_to(smap->header.event_code));
}

Ensure(mtp_container, transaction_id)
{
    assert_that(smap->header.transaction_id, is_equal_to(1));
}

Ensure(mtp_container, get_param_count_is_ok)
{
    mtp_op_cntr_t *no_param_container = (mtp_op_cntr_t*)&open_session_response;
    assert_that(mtp_container_get_param_count((mtp_op_cntr_t*)smap), is_equal_to(1));
    assert_that(mtp_container_get_param_count((mtp_op_cntr_t*)no_param_container), is_equal_to(0));
}

