# add re2 library sources
set(RE2_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/re2)
set(RE2_SOURCES
        ${RE2_SRCDIR}/re2/bitstate.cc
        ${RE2_SRCDIR}/re2/compile.cc
        ${RE2_SRCDIR}/re2/dfa.cc
        ${RE2_SRCDIR}/re2/filtered_re2.cc
        ${RE2_SRCDIR}/re2/mimics_pcre.cc
        ${RE2_SRCDIR}/re2/nfa.cc
        ${RE2_SRCDIR}/re2/onepass.cc
        ${RE2_SRCDIR}/re2/parse.cc
        ${RE2_SRCDIR}/re2/perl_groups.cc
        ${RE2_SRCDIR}/re2/prefilter.cc
        ${RE2_SRCDIR}/re2/prefilter_tree.cc
        ${RE2_SRCDIR}/re2/prog.cc
        ${RE2_SRCDIR}/re2/re2.cc
        ${RE2_SRCDIR}/re2/regexp.cc
        ${RE2_SRCDIR}/re2/set.cc
        ${RE2_SRCDIR}/re2/simplify.cc
        ${RE2_SRCDIR}/re2/stringpiece.cc
        ${RE2_SRCDIR}/re2/tostring.cc
        ${RE2_SRCDIR}/re2/unicode_casefold.cc
        ${RE2_SRCDIR}/re2/unicode_groups.cc
        ${RE2_SRCDIR}/util/rune.cc
        ${RE2_SRCDIR}/util/strutil.cc
)
target_sources(${PROJECT_NAME} PRIVATE ${RE2_SOURCES})

# turn off thread safety
target_compile_definitions(${PROJECT_NAME} PUBLIC RE2_NO_THREAD_SAFETY)

# suppress warning for RE2 to avoid big changes in the original library
set_source_files_properties(${RE2_SRCDIR}/re2/perl_groups.cc
        PROPERTIES COMPILE_FLAGS
        -Wno-missing-field-initializers
)


target_include_directories(${PROJECT_NAME} PUBLIC ${RE2_SRCDIR})
