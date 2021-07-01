add_executable(cnode-test cnode-test.cpp test-helpers.cpp)
target_link_libraries(cnode-test gtest ctree)
add_test(cnode-test cnode-test)

add_executable(ctree-test ctree-test.cpp test-helpers.cpp)
target_link_libraries(ctree-test gtest ctree)
add_test(ctree-test ctree-test)

#
# Call valgrind if necessary, TODO: suppression file for those 2 false positives
#
#add_test(NAME cnode-find-leaks COMMAND valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --error-exitcode=1 $<TARGET_FILE:cnode-test>)
#add_test(NAME ctest-find-leaks COMMAND valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --error-exitcode=1 $<TARGET_FILE:ctree-test>)
