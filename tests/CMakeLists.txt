function(smw_create_test name path)
	add_executable(${name} ${path})
	target_link_libraries(${name} PRIVATE doctest CommonFiles)
	doctest_discover_tests(${name})
endfunction()


smw_create_test(test_vec2 common/math/test_vec2.cpp)
