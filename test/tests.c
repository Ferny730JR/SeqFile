#include <stdio.h>

#include "minunit.h"

#include "seqf_core.h"

static UTEST_TYPE
test_seqfopen(void)
{
	init_unit_tests("Testing seqfopen");
	SeqFile file;

#define assert_file(file, exp_compression, exp_type) \
	file != NULL && \
	((seqf_statep)file)->compression == exp_compression && \
	((seqf_statep)file)->type == exp_type && \
	exp_compression == PLAIN ? !((seqf_statep)file)->stream_is_init : ((seqf_statep)file)->stream_is_init && \
	((seqf_statep)file)->mutex_is_init

	file = seqfopen("example_files/example.fasta", "a");
	mu_assert("Open fasta file", assert_file(file, PLAIN, 'a'));
	seqfclose(file);

	file = seqfopen("example_files/example.fasta.gz", "a");
	mu_assert("Open compressed fasta file", assert_file(file, GZIP, 'a'));
	seqfclose(file);

	file = seqfopen("example_files/example.fastq", "q");
	mu_assert("Open fastq file", assert_file(file, PLAIN, 'q'));
	seqfclose(file);

	file = seqfopen("example_files/example.fastq.gz", "q");
	mu_assert("Open compressed fastq file", assert_file(file, GZIP, 'q'));
	seqfclose(file);

	file = seqfopen("example_files/example.reads", "s");
	mu_assert("Open sequences file", assert_file(file, PLAIN, 's'));
	seqfclose(file);

	file = seqfopen("example_files/example.reads.gz", "s");
	mu_assert("Open compressed sequences file", assert_file(file, GZIP, 's'));
	seqfclose(file);

	/* Check that return NULL on files that don't exist */
	file = seqfopen("non-existent-dir/non-existent-file", NULL);
	mu_assert("Opening non-existent file", file == NULL);
	seqfclose(file);

	file = seqfopen("example_files/example.reads", "wrong!");
	mu_assert("Open file with unsupported mode", file == NULL);

#undef assert_file
	unit_tests_end;
}

static UTEST_TYPE
test_seqfclose(void)
{
	init_unit_tests("Testing seqfclose");
	mu_assert("Close null file", seqfclose(NULL) == 1);
	mu_assert("Close opened file", seqfclose(seqfopen("example_files/example.reads",NULL))==0);

	unit_tests_end;
}

static UTEST_TYPE
test_seqferrno(void)
{
	init_unit_tests("Testing seqferrno");

	mu_assert("seqferrno has not been set", seqferrno == 0);

	seqfopen("non-existent-path/no-existent-file", NULL);
	mu_assert("Opening file that does not exist", seqferrno == 1);

	seqfopen("example_files/example.reads", "wrong mode!");
	mu_assert("Error code for wrong mode", seqferrno == 3);

	unit_tests_end;
}

static void all_tests() {
	init_run_test;

	/* Begin tests */
	mu_run_test(test_seqfopen);
	mu_run_test(test_seqfclose);
	mu_run_test(test_seqferrno);

	/* End of tests */
	run_test_end;
}

int main(void) {
	all_tests();
	return 0;
}
