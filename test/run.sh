#!/bin/sh
# 動作確認用
#
# ./run.sh [exefile]
#
# run.shと同じディレクトリで拡張子が${TESTEXT}のファイルがテストケースとなる
# コマンドライン引数でexefileの指定があればその実行ファイルでテストする
#
# TESTOPT="-O" ./run.sh とかでテスト時の追加のオプションを指定可能
#
# 期待値の指定方法
# テストケースとなるファイルに以下のコメントを付ける
#     * `##return 0`   戻り値の期待値
#     * `##printn aaa` 標準出力への出力の期待値(改行付き) (エスケープシーケンスは使えない)
#

EXEFILE="exparrc"
TESTDIR="$(dirname $0)"
TESTEXT="ea"

# 実行ファイルの指定
test $# -eq 1 && EXEFILE="$1"

test -f ./${EXEFILE} || { echo "not found './${EXEFILE}'"; exit 1; }


run_exefile()
{
	./${EXEFILE} -f -output-bc $* ${TESTOPT}
}


testcaselist=$(ls ${TESTDIR}/*.${TESTEXT})
test_num=0
success_num=0
for testcase in ${testcaselist}; do
	expected_r=$(grep "^##return " ${testcase} | sed "s/^##return //")
	expected_p=$(grep "^##printn " ${testcase} | sed "s/^##printn //")

	result_pn=$(run_exefile ${testcase} | lli -force-interpreter)
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

