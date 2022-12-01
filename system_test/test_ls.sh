#!/bin/bash
ROOT_PATH='..'
if [ -z "$FT_LS_PATH" ];
then
	FT_LS_PATH="$ROOT_PATH/debug/src/ft_ls"
else
	FT_LS_PATH="$ROOT_PATH/$FT_LS_PATH"
fi

TEST_COUNT='0'
TEST_PASSED='0'
TEST_FAILED='0'

ft_ls="${FT_LS_PATH}"
LS_OUTFILE='ls_out.txt'
FT_LS_OUTFILE='ft_ls_out.txt'
RESULT_OUTFILE='result.log'

test_option_combinations() {
	OPTIONS=("-l" "-r" "-R" "-t" "-a" "-lr" "-lR" "-lt" "-la" "-rR" "-rt" "-ra" "-Rt" "-Ra" "-ta" "-lrR" "-lrt" "-lra" "-lRt" "-lRa" "-lta" "-rRt" "-rRa" "-rta" "-Rta" "-lrRt" "-lrRa" "-lrta" "-lRta" "-rRta" "-lrRta")
	for option in "${OPTIONS[@]}"
	do
		test_case "${option}" $@
	done
}

FAIL='\033[91m'
PASS='\033[32m'
ENDC='\033[0m'

test_case() {
	TEST_COUNT=$((TEST_COUNT+1))
	ls -1 $@ &> $LS_OUTFILE
	"${ft_ls}" $@ &> $FT_LS_OUTFILE
	cmp -s $LS_OUTFILE $FT_LS_OUTFILE
	if [ "$?" -eq 1 ];
	then
		TEST_FAILED=$((TEST_FAILED+1))
		echo -e '[' $FAIL "Failed" $ENDC "]: 'ls $@'"
		echo "[ERROR]: 'ls $@'" >> $RESULT_OUTFILE
		echo "[ERROR]: $(cmp $LS_OUTFILE $FT_LS_OUTFILE)" >> $RESULT_OUTFILE
		# echo "[INFO]: 'ls -1 $@'" >> $RESULT_OUTFILE
		# cat "$LS_OUTFILE" >> $RESULT_OUTFILE
		# echo "[INFO]: '${ft_ls} $@'" >> $RESULT_OUTFILE
		# cat "$FT_LS_OUTFILE" >> $RESULT_OUTFILE
		echo "[INFO]: DIFF" >> $RESULT_OUTFILE
		diff $LS_OUTFILE $FT_LS_OUTFILE >> $RESULT_OUTFILE
		echo >> $RESULT_OUTFILE
		return 1
	else
		echo -e '[' $PASS "Passed" $ENDC "]: 'ls $@'"
		TEST_PASSED=$((TEST_PASSED+1))
	fi
	return 0
}

make -C "$ROOT_PATH" build
> $RESULT_OUTFILE

test_option_combinations recursive
test_option_combinations basic
test_option_combinations basic recursive somefile
test_option_combinations -z
test_option_combinations this_is_a_symlink
test_option_combinations this_is_a_symlink ./basic ./recursive

echo "Passed: $TEST_PASSED / $TEST_COUNT"
echo "Failed: $TEST_FAILED / $TEST_COUNT"