#!/bin/sh
# expr言語は現在の言語仕様上、
# 標準出力に出力する手段がないため、計算結果を戻り値として返している
# *出力可能になったため、順次標準出力の確認に変更する*
#
# TESTOPT="-O" ./test.sh とかでテスト時の追加のオプションを指定可能
#
# `##return 0`   戻り値の期待値
# `##printn aaa` 標準出力への出力の期待値(改行付き) エスケープシーケンスは使えない

EXEFILE="exparrc"
TESTDIR="test"

test -f ./${EXEFILE} || { echo "not found './${EXEFILE}'"; exit 1; }

testcaselist=$(ls ${TESTDIR}/*.ea)
test_num=0
success_num=0
for testcase in ${testcaselist}; do
	expected_r=$(grep "^##return " ${testcase} | sed "s/^##return //")
	expected_p=$(grep "^##printn " ${testcase} | sed "s/^##printn //")

	result_pn=$(./${EXEFILE} ${testcase} -f ${TESTOPT} | lli -force-interpreter)
	result_ret=$?

	# 戻り値のテスト
	if [ -n "${expected_r}" ]; then
		test_num=$((test_num + 1))
		if [ ${result_ret} -eq ${expected_r} ]; then
			success_num=$((success_num + 1))
			printf "\e[32mSuccess:  (return)\e[m"
		else
			printf "\e[31mFailure:  (return ${result_ret}:${expected_r})\e[m"
		fi
		echo "  [${testcase}]"
	fi

	# 標準出力への出力のテスト
	if [ -n "${expected_p}" ]; then
		test_num=$((test_num + 1))
		if [ "${result_pn}" = "${expected_p}" ]; then
			success_num=$((success_num + 1))
			printf "\e[32mSuccess:  (printn)\e[m"
		else
			printf "\e[31mFailure:  (printn ${result_pn}:${expected_p})\e[m"
		fi
		echo "  [${testcase}]"
	fi
done

echo "${success_num} / ${test_num}"

