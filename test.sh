#!/bin/sh
# expr言語は現在の言語仕様上、
# 標準出力に出力する手段がないため、計算結果を戻り値として返している

EXEFILE="exprc"
TESTDIR="test"

test -f ./${EXEFILE} || { echo "not found './${EXEFILE}'"; exit 1; }

testcaselist=$(ls ${TESTDIR}/*.expr)
success_num=0
for testcase in ${testcaselist}; do
	expected=$(grep "^##return " ${testcase} | sed "s/^##return //")

	./${EXEFILE} ${testcase} -print-llvm | lli -force-interpreter
	ret=$?

	if [ ${ret} -eq ${expected} ]; then
		printf "Success:"
		success_num=$((success_num + 1))
	else
		printf "Failure (ret=${ret} exp=${expected}):"
	fi
	echo "  ${testcase}"
done

test_num=$(echo ${testcaselist} | wc -w)
echo "${success_num} / ${test_num}"

