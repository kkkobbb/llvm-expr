#!/bin/sh
# アセンブリ出力の確認
#
# 成功時、0を返す
#

######################### 準備 #########################

EXEFILE="./exparrc"
TESTDIR="$(dirname $0)"

RUN_DIR=${TESTDIR}/TD

# 実行ファイルの指定
test $# -eq 1 && EXEFILE="$1"
# 実行ファイルの存在確認
test -f ${EXEFILE} || { echo "not found '${EXEFILE}'"; exit 1; }


# テストとして実行するコマンド
run_exefile()
{
	AB_EXEFILE=$(cd $(dirname ${EXEFILE}); pwd)/$(basename ${EXEFILE})

	# テスト用のディレクトリ生成、移動
	mkdir -p ${RUN_DIR}
	cd ${RUN_DIR}

	# 実行
	printf "12;\n" | ${AB_EXEFILE} /dev/stdin
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
	gcc a.s -o aaa
	./aaa
	aaa_ret=$?

	# 元のディレクトリに戻る
	cd - > /dev/null 2> /dev/null

	# 生成ファイルをコンパイルしてできた実行ファイルの戻り値
	if [ "${aaa_ret}" -ne 12 ]; then
		echo "bad generated exefile return (${aaa_ret})"
		return 1
	fi

	return 0
}


######################### 実行 #########################

# 実行
run_exefile
ret=$?


######################### 確認 #########################

result=0

# 戻り値が0であること
if [ ${ret} -ne 0 ]; then
	result=1
fi


######################### 掃除 #########################

# 一時ファイルの削除
rm -rf ${RUN_DIR}

exit ${result}

