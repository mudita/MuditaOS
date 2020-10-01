#pragma once

#if defined(TARGET_Linux) && not defined(SERIAL_PORT)
#define ENABLE_GSM (0)
#else
#define ENABLE_GSM (1)
#endif
