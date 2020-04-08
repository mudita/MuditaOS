# 1. File system
 - Currently used FAT is obsolete and prone to failures which will lead to phone failure
 - When only 1 partition is used for all storage (including OS failsafe) when it'll become corrupt failsafe OS is unbootable
 - Frequent writes to filesystem within the same location (any modification of data base related to eg. call made) will lead to eMMC degradation and in short time result in failure
 - Proper filesystem shall be used (eg. LittleFS)
