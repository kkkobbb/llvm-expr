#!/bin/sh
# コンパイル時の動作確認用
#
# ./run.sh [exefile]
#
# run.shと同じディレクトリで拡張子が${TESTEXT}のファイルがテストケースとなる
# コマンドライン引数でexefileの指定があればその実行ファイルでテストする
# TESTOPT="-O" ./run.sh とかでテスト時の追加のオプションを指定可能
#
# exefile
#     テストするコマンド 指定がなければ同じ階層のコマンド
#
# 戻り値
#     成功時 0
#
#
# テスト用の入力ファイルについて
# 期待値の指定方法
# テストケースとなるファイルに以下のコメントを付ける
#     * `##return 0`       戻り値の期待値
#     * `##printn aaa`     標準出力への出力の期待値(改行付き)
#     * `##output_stderr`  標準エラー出力への出力があること (内容は問わない)
DEFAULT_EXE="./exparrc"
TESTDIR="$(cd $(dirname $0); pwd)"
. ${TESTDIR}/../run_init.sh

TESTEXT="test.ea"

COLOR_SUCCESS="\e[32m"
COLOR_FAILURE="\e[31m"
COLOR_RESET="\e[m"


print_result()
{
	result=$1
	name=$2
	msg=$3

	if [ "${result}" -eq 0 ]; then
		printf "${COLOR_SUCCESS}Success:${COLOR_RESET}"
	else
		printf "${COLOR_FAILURE}Failure: (${msg})${COLOR_RESET}"
	fi
	echo "  [${name}]"
}

testcase_lang()
{
	err="output_err"
	testfile=$1
	expected_output_stderr=$(grep "^##output_stderr" ${testfile})
	expected_r=$(grep "^\s*##return " ${testfile} | sed "s/^\s*##return //")
	expected_p=$(grep "^\s*##printn " ${testfile} | sed "s/^\s*##printn //")

	result_pn=$(${TEST_EXE} -f -filetype=bc ${testfile} ${TESTOPT} 2> ${err} | \
		lli -force-interpreter 2>> ${err})
	result_ret=$?

	err_size=$(cat ${err} | wc -c)

	# エラーがあったかのテスト
	# (標準エラー出力に出力があったか確認する、内容は確認しない)
	if [ -n "${expected_output_stderr}" ]; then
		test_num=$((test_num + 1))
		result=1
		if [ "${err_size}" -ne 0 ]; then
			success_num=$((success_num + 1))
			# 標準エラー出力に出力しない
			err_size=0
			result=0
		fi
		name="$(basename ${testfile}) (output_stderr)"
		print_result ${result} "${name}"
	fi

	# 標準エラー出力に出力があれば表示する
	if [ "${err_size}" -ne 0 ]; then
		cat ${err}
		printf "\n"
	fi

	# 戻り値のテスト
	if [ -n "${expected_r}" ]; then
		test_num=$((test_num + 1))
		result=1
		msg=""
		if [ ${result_ret} -eq ${expected_r} ]; then
			success_num=$((success_num + 1))
			result=0
		else
			msg="${result_ret}:${expected_r}"
		fi
		name="$(basename ${testfile}) (return)"
		print_result ${result} "${name}" "${msg}"
	fi

	# 標準出力への出力のテスト
	if [ -n "${expected_p}" ]; then
		test_num=$((test_num + 1))
		result=1
		msg=""
		if [ "${result_pn}" = "${expected_p}" ]; then
			success_num=$((success_num + 1))
			result=0
		else
			msg="\n${result_pn}\n<--- result : expected --->\n${expected_p}\n"
		fi
		name="$(basename ${testfile}) (printn)"
		print_result ${result} "${name}" "${msg}"
	fi

	return ${result}
}

testcaselist=$(ls ${TESTDIR}/*.${TESTEXT})
test_num=0
success_num=0
for testcase in ${testcaselist}; do
	run_test testcase_lang ${testcase}
	ret=$?
	test_num=$((test_num + 1))
	if [ "${ret}" -eq 0 ]; then
		success_num=$((success_num + 1))
	fi
done

# 全体の成否
all_result=0
if [ "${success_num}" -ne "${test_num}" ]; then
	all_result=1
fi

# 全体の成否で色を変える
color="${COLOR_SUCCESS}"
if [ "${all_result}" -ne 0 ]; then
	color="${COLOR_FAILURE}"
fi
printf "\n${color}${success_num} / ${test_num}${COLOR_RESET} (files)\n"

exit ${all_result}

