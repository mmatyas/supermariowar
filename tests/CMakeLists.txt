add_executable(smw-tests
	runner.cpp
	common/test_version.cpp
	common/test_tokenize.cpp
	common/math/test_vec2.cpp
	common/util/test_DirIterator.cpp
)
target_link_libraries(smw-tests PRIVATE doctest CommonFiles)
doctest_discover_tests(smw-tests)
