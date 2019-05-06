#!/bin/sh
# 出力ファイル種類指定無し場合の確認
#   デフォルトはasmを出力する
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
	printf "12;\n" | ${TEST_EXE} /dev/stdin
	ret=$?

	# 戻り値が0出ない場合、失敗
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

