#include "cmar.h"
#include <check.h>

char* error;
void setup(void) {
	error = malloc(256*sizeof(char));
}
void teardown(void) {
	free(error);
}
START_TEST(test_cmar_fake) {
	fail_unless(true, "You can't fail that one !");
}
END_TEST

Suite* array_suite(void) {
	Suite* s = suite_create("CMAR");
	TCase* tc_tree = tcase_create("FP-Growth tree");
	tcase_add_checked_fixture(tc_tree, setup, teardown);
	tcase_add_test(tc_tree, test_cmar_fake);
	suite_add_tcase(s, tc_tree);
	return s;
}

int main(void) {
	int number_failed;
	Suite* s = array_suite();
	SRunner* sr = srunner_create(s);
	srunner_run_all(sr, CK_ENV);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
