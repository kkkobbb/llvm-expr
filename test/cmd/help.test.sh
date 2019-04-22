#!/bin/sh
# helpオプション実行の確認
#
# 成功時、0を返す
#

######################### 準備 #########################

EXEFILE="./exparrc"
TESTDIR="$(dirname $0)"

# 実行ファイルの指定
test $# -eq 1 && EXEFILE="$1"
# 実行ファイルの存在確認
test -f ${EXEFILE} || { echo "not found '${EXEFILE}'"; exit 1; }


# テストとして実行するコマンド
run_exefile()
{
	${EXEFILE} -help
}


######################### 実行 #########################

# 標準出力 取得用
out=$(mktemp)
# 標準エラー出力 取得用
err=$(mktemp)

# 実行
run_exefile > ${out} 2> ${err}
ret=$(echo $?)


######################### 確認 #########################

result=0

# 戻り値が0であること
if [ ${ret} -ne 0 ]; then
	result=1
fi

# 標準出力に出力されていること
if [ -z "$(cat ${out})" ]; then
	result=1
fi

# 標準エラー出力に出力されていないこと
if [ -n "$(cat ${err})" ]; then
	result=1
fi


######################### 掃除 #########################

# テスト失敗時出力を表示する
if [ ${result} -ne 0 ]; then
	cat ${out}
	cat ${err}
fi

# 一時ファイルの削除
rm -f ${out}
rm -f ${err}

return ${result}

