# テストの準備用
#
# $1 テスト用のコマンド (default a.out)
#
# testcase を定義してからrun_test を実行する
#   テスト用のディレクトリで実行する
#
# DEFAULT_EXE: テスト用の実行ファイルのデフォルト
# run_test $1: テストコード
#     テスト成功時0、失敗時非0を返す
#     テストするコマンドは ${TEST_EXE} で指定すること
#     (実行時のディレクトリにテストしたいコマンドがコピーされている)
#     実行時のディレクトリにはテスト用の実行ファイルのみ存在する
#     なるべくディレクトリ移動はしないこと
#     実行時のディレクトリ内に生成したファイルは全てテスト後に削除される

# 実行ファイル
REAL_EXE=${DEFAULT_EXE:-"./a.out"}
# 引数があれば、最初の引数を使用する
test $# -ne 0 && REAL_EXE=$1
# 実行ファイルの存在確認
test -f ${REAL_EXE} || { echo "not found '${REAL_EXE}'"; exit 1; }


# テストの実施
# $1 を実行する
run_test() {
	if [ $# -eq 0 ];then
		return 255
	fi
	testcase=$1
	shift ; testcase_arg="$@"
	# テスト用のディレクトリ生成
	RUN_DIR=$(mktemp -d)
	cp ${REAL_EXE} ${RUN_DIR}
	# テスト実行時に実行ファイルを参照する場合の名前
	TEST_EXE="./$(basename ${REAL_EXE})"

	# テスト用のディレクトリに移動
	base_dir=$(pwd)
	cd ${RUN_DIR}

	# 実行
	# 環境変数を隠すため、別プロセスで実行
	(unset REAL_EXE RUN_DIR base_dir ; \
		${testcase} ${testcase_arg} ; exit $? )
	ret=$?

	# 元のディレクトリに戻る
	cd ${base_dir}

	# テスト用のディレクトリの削除
	rm -rf ${RUN_DIR}

	return ${ret}
}

# エラー時のメッセージ出力
#
# ファイルディスクリプタ3番がオープンしていればそこへ出力する
# xml用に'<'、'>'はそれぞれ'('、')'に変換する
error_msg() {
	if [ -f /dev/fd/3 ]; then
		printf "Error: %s;\n" "$1" | tr '<>' '()' >&3
	else
		printf "Error: %s;\n" "$1" | tr '<>' '()'
	fi
}

