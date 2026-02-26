function(smw_create_test name path)
	add_executable(${name} ${path})
	target_link_libraries(${name} PRIVATE doctest CommonFiles)
	doctest_discover_tests(${name})
endfunction()


smw_create_test(test_vec2 common/math/test_vec2.cpp)
smw_create_test(test_version common/test_version.cpp)
smw_create_test(test_tokenize common/test_tokenize.cpp)
smw_create_test(test_DirIterator common/util/test_DirIterator.cpp)
