#!/bin/sh
# コンパイル時の動作確認用
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
#     * `##return 0`       戻り値の期待値
#     * `##printn aaa`     標準出力への出力の期待値(改行付き)
#     * `##output_stderr`  標準エラー出力への出力があること (内容は問わない)
#
EXEFILE="./exparrc"
TESTDIR="$(dirname $0)"
TESTEXT="ea"

COLOR_SUCCESS="\e[32m"
COLOR_FAILURE="\e[31m"
COLOR_RESET="\e[m"


# 実行ファイルの指定
test $# -eq 1 && EXEFILE="$1"
# 実行ファイルの存在確認
test -f ${EXEFILE} || { echo "not found '${EXEFILE}'"; exit 1; }


run_exefile()
{
	${EXEFILE} -f -filetype=bc $* ${TESTOPT}
}

# 標準エラー出力取得用の一時ファイル生成
err=$(mktemp)

testcaselist=$(ls ${TESTDIR}/*.${TESTEXT})
test_num=0
success_num=0
for testcase in ${testcaselist}; do
	expected_output_stderr=$(grep "^##output_stderr" ${testcase})
	expected_r=$(grep "^##return " ${testcase} | sed "s/^##return //")
	expected_p=$(grep "^##printn " ${testcase} | sed "s/^##printn //")

	result_pn=$(run_exefile ${testcase} | lli -force-interpreter) 2> ${err}
	result_ret=$?

	err_size=$(cat ${err} | wc -c)

	# エラーがあったかのテスト
	# (標準エラー出力に出力があったか確認する、内容は確認しない)
	if [ -n "${expected_output_stderr}" ]; then
		test_num=$((test_num + 1))
		if [ "${err_size}" -ne 0 ]; then
			success_num=$((success_num + 1))
			printf "${COLOR_SUCCESS}Success:  (output_stderr)${COLOR_RESET}"
			# 標準エラー出力に出力しない
			err_size=0
		else
			printf "${COLOR_FAILURE}Failure:  (no output_stderr)${COLOR_RESET}"
		fi
		echo "  [${testcase}]"
	fi

	# 標準エラー出力に出力があれば表示する
	if [ "${err_size}" -ne 0 ]; then
		cat ${err}
		printf "\n"
	fi

	# 戻り値のテスト
	if [ -n "${expected_r}" ]; then
		test_num=$((test_num + 1))
		if [ ${result_ret} -eq ${expected_r} ]; then
			success_num=$((success_num + 1))
			printf "${COLOR_SUCCESS}Success:  (return)\e${COLOR_RESET}"
		else
			printf "${COLOR_FAILURE}Failure:  (return ${result_ret}:${expected_r})${COLOR_RESET}"
		fi
		echo "  [${testcase}]"
	fi

	# 標準出力への出力のテスト
	if [ -n "${expected_p}" ]; then
		test_num=$((test_num + 1))
		if [ "${result_pn}" = "${expected_p}" ]; then
			success_num=$((success_num + 1))
			printf "${COLOR_SUCCESS}Success:  (printn)${COLOR_RESET}"
		else
			printf "${COLOR_FAILURE}Failure:  (printn\n${result_pn}\n:\n${expected_p}\n)${COLOR_RESET}"
		fi
		echo "  [${testcase}]"
	fi
done

# 一時ファイルの削除
rm -f ${err}

# 全体の成否で色を変える
color="${COLOR_SUCCESS}"
if [ "${success_num}" -ne "${test_num}" ]; then
	color="${COLOR_FAILURE}"
fi
printf "\n${color}${success_num} / ${test_num}${COLOR_RESET}\n"

