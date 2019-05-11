#!/bin/sh
# 最適化オプションの確認
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
	# 試験用ソースコード生成
	srcfname="test.ea"
	printf "a<-4;\nb<-5;\nc<-a+b;" > ${srcfname}

	# 実行
	${TEST_EXE} -O ${srcfname}
	ret=$?

	# 戻り値が0でない場合、失敗
	if [ "${ret}" -ne 0 ]; then
		error_msg "bad return (${ret})"
		return 1
	fi

	# ファイルを生成していない場合、失敗
	if [ ! -f "a.s" ]; then
		error_msg "no asm file"
		return 1
	fi

	# 生成ファイルの実行
	gcc a.s -o output
	./output
	output_ret=$?

	# 生成ファイルをコンパイルしてできた実行ファイルの戻り値
	if [ "${output_ret}" -ne 9 ]; then
		error_msg "bad generated exefile return (${output_ret})"
		return 1
	fi

	return 0
}


######################### 実行 #########################

run_test testcase

