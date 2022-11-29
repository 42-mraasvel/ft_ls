#!/bin/bash
ROOT_PATH='..'
if [ -z "$FT_LS_PATH" ];
then
	FT_LS_PATH="$ROOT_PATH/debug/src/ft_ls"
else
	FT_LS_PATH="$ROOT_PATH/$FT_LS_PATH"
fi

ft_ls="${FT_LS_PATH}"
LS_OUTFILE='ls_out.txt'
FT_LS_OUTFILE='ft_ls_out.txt'
RESULT_OUTFILE='result.log'

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
	ls -1 $@ &> $LS_OUTFILE
	"${ft_ls}" $@ &> $FT_LS_OUTFILE
	cmp -s $LS_OUTFILE $FT_LS_OUTFILE
	if [ "$?" -eq 1 ];
	then
		echo -e '[' $FAIL "Failed" $ENDC "]: 'ls $@'"
		echo "[ERROR]: 'ls $@'" >> $RESULT_OUTFILE
		echo "[ERROR]: $(cmp $LS_OUTFILE $FT_LS_OUTFILE)" >> $RESULT_OUTFILE
		echo "[INFO]: 'ls -1 $@'" >> $RESULT_OUTFILE
		cat "$LS_OUTFILE" >> $RESULT_OUTFILE
		echo "[INFO]: '${ft_ls} $@'" >> $RESULT_OUTFILE
		cat "$FT_LS_OUTFILE" >> $RESULT_OUTFILE
		echo "[INFO]: DIFF" >> $RESULT_OUTFILE
		diff $LS_OUTFILE $FT_LS_OUTFILE >> $RESULT_OUTFILE
		echo >> $RESULT_OUTFILE
		return 1
	fi
	return 0
}

make -C "$ROOT_PATH" build
> $RESULT_OUTFILE
test_option_combinations src Makefile
test_option_combinations src
