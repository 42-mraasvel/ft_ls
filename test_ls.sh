#!/bin/bash
FT_LS_PATH="./debug/src/ft_ls"
ft_ls="${FT_LS_PATH}"

test_option_combinations() {
	OPTIONS=("-r" "-R" "-t" "-a" "-rR" "-rt" "-ra" "-Rt" "-Ra" "-ta" "-rRt" "-rRa" "-rta" "-Rta" "-rRta")
	for option in "${OPTIONS[@]}"
	do
		test_case "${option}" $@
	done
}

FAIL='\033[91m'
ENDC='\033[0m'

test_case() {
	ls -1 $@ &> ls_output
	"${ft_ls}" $@ &> ft_ls_output
	cmp -s ls_output ft_ls_output
	if [ "$?" -eq 1 ];
	then
		echo -e '[' $FAIL "Failed" $ENDC "]: 'ls $@'"
		echo "[ERROR]: 'ls $@'" >> results.log
		echo "[ERROR]: $(cmp ls_output ft_ls_output)" >> results.log
		echo "[INFO]: 'ls -1 $@'" >> results.log
		cat ls_output >> results.log
		echo "[INFO]: '${ft_ls} $@'" >> results.log
		cat ft_ls_output >> results.log
		echo "[INFO]: DIFF" >> results.log
		diff ls_output ft_ls_output >> results.log
		echo >> results.log
		return 1
	fi
	return 0
}

make build
> results.log
test_option_combinations src Makefile
test_option_combinations src
