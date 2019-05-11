#!/bin/sh
# helpオプション実行の確認
#
# 成功時、0を返す
#

######################### 準備 #########################
DEFAULT_EXE="./exparrc"
TESTDIR="$(dirname $0)"
. ${TESTDIR}/run_init.sh


# テストとして実行するコマンド
testcase()
{
	# 標準出力 取得用
	out="test_stdout"
	# 標準エラー出力 取得用
	err="test_stderr"

	${TEST_EXE} --help > ${out} 2> ${err}
	ret=$?

	result=0
	# 戻り値が0であること
	if [ ${ret} -ne 0 ]; then
		error_msg "bad return (${ret})"
		result=1
	fi
	# 標準出力に出力されていること
	if [ -z "$(cat ${out})" ]; then
		error_msg "no help output"
		result=1
	fi
	# 標準エラー出力に出力されていないこと
	if [ -n "$(cat ${err})" ]; then
		error_msg "output stderr"
		result=1
	fi

	# 出力を表示する
	cat ${out}
	cat ${err}

	return ${result}
}


######################### 実行 #########################

run_test testcase

