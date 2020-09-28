#include "FreeRTOS.h"
#include <string.h>
#include <assert.h>

#define MAX_HANDLES 512
#define MAX_FILENAME_LENGTH 64
#define NO_HANDLE (0)

#if DEBUG_DB
#   include <fsl_debug_console.h>
#   define LOG(format...) PRINTF("[MTPDB]: "format)
#else
#   define LOG(...) LOG_INFO(__VA_ARGS__)
#endif

typedef char db_entry[MAX_FILENAME_LENGTH];
typedef uint32_t handle_t;

struct mtp_db {
    db_entry map[MAX_HANDLES];
    handle_t highest;
    uint32_t count;
};

static handle_t db_alloc(struct mtp_db *db)
{
    handle_t i;
    for(i = 1; i < MAX_HANDLES; i++)  {
        if (db->map[i][0] == '\0') {
            if (db->highest < i)
                db->highest = i;
            db->count++;
            return i;
        }
    }
    return NO_HANDLE;
}

static void db_free(struct mtp_db *db, handle_t handle)
{
    db->map[handle][0] = '\0';
    db->count++;
}

static handle_t db_search(struct mtp_db *db, const char *key)
{
    handle_t i;
    for(i = db->highest; i > 1; i--)  {
        if (!strncmp(db->map[i], key, 64))
            return i;
    }
    return NO_HANDLE;
}

struct mtp_db* mtp_db_alloc(void)
{
    struct mtp_db *db = pvPortMalloc(sizeof(struct mtp_db));
    if (!db) {
        LOG("Not enough memory!\n");
    }
    return db;
}

uint32_t mtp_db_add(struct mtp_db *db, const char *key)
{
    assert(db && key);

    uint32_t handle = db_search(db, key);

    if (handle)
        return handle;

    handle = db_alloc(db);
    if (handle) {
        strncpy(db->map[handle], key, 64);
        LOG("add [%u]: %s\n", (unsigned int)handle, db->map[handle]);
        return handle;
    }
    return NO_HANDLE;
}

void mtp_db_update(struct mtp_db *db, uint32_t handle, const char *key)
{
    assert(db);
    if (handle > 0 && handle < MAX_HANDLES) {
        strncpy(db->map[handle], key, 64);
    }
}

const char *mtp_db_get(struct mtp_db *db, uint32_t handle)
{
    assert(db);
    if (handle > 0 && handle < MAX_HANDLES && db->map[handle][0] != '\0') {
        return db->map[handle];
    }
    return NULL;
}

#if 0
const char *mtp_db_get_handle(struct mtp_db *db, const char *key)
{
    assert(db);
    const char *result = NULL;
    handle_t handle = db_search(db, key);
    if (handle)
        result = db->map[handle];
    return result;
}
#endif

void mtp_db_del(struct mtp_db *db, uint32_t handle)
{
    assert(db);

    if (handle > 0 && handle < MAX_HANDLES) {
        LOG("drop [%u]: %s\n", (unsigned int)handle, db->map[handle]);
        db_free(db, handle);
    }
}

