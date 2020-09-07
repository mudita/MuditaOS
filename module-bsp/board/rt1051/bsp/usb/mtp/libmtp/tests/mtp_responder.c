#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "mtp_responder.h"
#include "mtp_container.h"
#include "mtp_storage.h"
#include "mtp_util.h"

#include "mock_mtp_storage_api.h"

static mtp_responder_t *mtp = NULL;
static uint16_t error;

Describe(mtp_responder);

BeforeEach(mtp_responder)
{
    mtp = mtp_responder_alloc();
    mtp_responder_init(mtp);
    error = 0xaa;
}

AfterEach(mtp_responder)
{
    mtp_responder_free(mtp);
}

Ensure(mtp_responder, allocated)
{
    assert_that(mtp, is_not_equal_to(NULL));
}

Ensure(mtp_responder, add_storage_fails_if_id_is_zero)
{
    error = mtp_responder_set_storage(mtp, 0, NULL, NULL);
    assert_that(error, is_not_equal_to(0));
}

Ensure(mtp_responder, add_storage_fails_if_id_is_ffffffff)
{
    error = mtp_responder_set_storage(mtp, 0xFFFFFFFF, NULL, NULL);
    assert_that(error, is_not_equal_to(0));
}

Ensure(mtp_responder, add_storage_fails_if_api_not_provided)
{
    error = mtp_responder_set_storage(mtp, 0x00010001, NULL, NULL);
    assert_that(error, is_not_equal_to(0));
}

