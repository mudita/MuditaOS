#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <vfs.hpp>
#include <i18/i18.hpp>

class vfs vfs;       // needed for compilation, our vfs is global
utils::i18 localize; // needed to load any keymap - these are stored in i18
