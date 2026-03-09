add_executable(smw-tests
    runner.cpp
    common/test_version.cpp
    common/test_tokenize.cpp
    common/math/test_vec2.cpp
    common/util/test_DirIterator.cpp
)

if(MSVC)
    target_compile_definitions(smw-tests PRIVATE DOCTEST_CONFIG_USE_STD_HEADERS)
endif()

target_link_libraries(smw-tests PRIVATE doctest CommonFiles)
doctest_discover_tests(smw-tests)
