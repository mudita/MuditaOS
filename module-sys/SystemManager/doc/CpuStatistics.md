CPU statistics
==============

We are able to gather CPU statistics and dump them wherever we want via printer.
Current CPU statistics gathering:
- measures each task in profiling pool data 
- prints last 100ms CPU usage data after frequency was changed

# preface

Our processor usage is limited to:
- Applications
- Services
- Workers
- Threads
- IRQ's

Applications and Services are registered on bus and are processing data on demand.
Workers should also process data on demand, but do not have bus - but communicate via custom bindings. Workers are bound to Services.
Threads are naked FreeRTOS threads, for our needs its: Idle task, Timer task - these do not have any rule how should be implemented.
IRQ's are very processing specific.

For Applications and Services we can measure:
- What request was approximately was handled and how long
For Workers and threads we can measure:
- How long was it processing.
For IRQs we do not have measuring capabilities right now.

# data gathering

defined in: `module-os/FreeRTOS/perf.h`, on each context switch saves data in memory pool for each task.
used in:
- CpuStatistics.cpp
- tasks.c

Two approaches were tested:
1. approach where profiling data was stored separately on each context switch
2. approach where profiling data is a sum of time for all context switches
profiling gathering is meant to have O0 complexity to not add load to the core processing

The first approach resulted in quite a lot of data, especially in the moments when the processor was fully loaded.
With filtering out useless information it resulted in ~60 samples per 100ms when in idle, while having thousands when 
actual processing was happening (i.e. rendering). This resulted in pretty graphing capabilities, but was not useable
for storage, as we could easily gather thousands kB per 100ms
The second approach sums time and context switches per task from the last request.
This way we do not know what exact tasks fight for the CPU attention, but have overall **smoothened** characteristics.

The second approach is more convenient to both:
- storage: with samples limiting we can save data only when frequency is changed, which depending on time span should take up to hundreds of bytes. normally tens right now.
- ram usage: we can limit buffer to predicted count od threads, therefore whole performance gathering buffer shall take around 600 bytes.

# data printing

There is `sys::cpu::Printer` class in CpuStatistics for use. Depending of needs curently it can either:
- not print at all
- print CPU usage as formatted csv string to std::out
- print CPU usage as message pack csv to std::out
Formatted message pack can be modified to store data in file to minimize it's size. This way we will have small footprint, easily accessible statistics without logging.
Any other printing capabilities could be added.

# IRQ time

Currently we do not measure time spent in IRQs, this could be easily achieved by:
- funneling all IVT calls to single IRQ receptor function
- calling IVT calls on demand form them, while saving GPT time before and after the call
