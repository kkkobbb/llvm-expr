#!/bin/sh
# アセンブリ出力の確認
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
	# 実行
	printf "12;\n" | ${TEST_EXE} -filetype=asm /dev/stdin
	ret=$?

	# 戻り値が0でない場合、失敗
	if [ "${ret}" -ne 0 ]; then
		echo "bad return (${ret})"
		return 1
	fi

	# ファイルを生成していない場合、失敗
	if [ ! -f "a.s" ]; then
		echo "no asm file"
		return 1
	fi

	# 生成ファイルの実行
	gcc a.s -o output
	./output
	output_ret=$?

	# 生成ファイルをコンパイルしてできた実行ファイルの戻り値
	if [ "${output_ret}" -ne 12 ]; then
		echo "bad generated exefile return (${output_ret})"
		return 1
	fi

	return 0
}


######################### 実行 #########################

run_test testcase

