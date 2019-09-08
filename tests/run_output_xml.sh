# テスト結果出力
#
# JUnitのxml形式で出力する
#
# 参照する環境変数
# TESTSUITE_NAME : testsuite name属性
# TESTSUITE_TESTS : testsuite テスト総数
# TESTSUITE_FAILURES : testsuite 失敗数
# TODO 環境変数は使用しない？

# testcase要素追加
# 
# $1 - $2 まで必須
# system-out要素、system-err要素はtestcase単位で出力する
# 失敗時、failure_txtがあればfailure要素のテキストとして出力する
add_testcase() {
	# 成否 0:成功 0以外:失敗
	result=$1
	# テスト名
	name=$2
	# テストクラス名
	classname=${3:-${TESTSUITE_NAME}}
	# 標準出力保存ファイル名
	out=$4
	# 標準エラー出力保存ファイル名
	err=$5
	# 失敗時用
	# 失敗時の
	failure_txt=$6
	# message属性用
	msg=$7
	# type属性用
	fail_type=$8

	TESTSUITE_TESTS=$((TESTSUITE_TESTS + 1))

	printf '<testcase classname="'${classname:-"Test"}'" name="'${name}'">\n'

	# 失敗時はfailure要素を追加する
	if [ "${result}" -ne 0 ]; then
		TESTSUITE_FAILURES=$((TESTSUITE_FAILURES + 1))
		printf '<failure type="'${fail_type:-"Error"}'" message="'${message:-"test failed"}'">'
		test -n "${failure_txt}" -a -f "${failure_txt}" && cat ${failure_txt} | sed "s/]]>/]] >/g"
		printf '</failure>\n'
	fi

	# 標準出力の内容を出力
	if [ -n "${out}" -a -f "${out}" ]; then
		printf '<system-out><![CDATA['
		cat ${out} | sed "s/]]>/]] >/g"
		printf ']]></system-out>\n'
	fi

	# 標準エラー出力の内容を出力
	if [ -n "${err}" -a -f "${err}" ]; then
		printf '<system-err><![CDATA['
		cat ${err} | sed "s/]]>/]] >/g"
		printf ']]></system-err>\n'
	fi

	printf '</testcase>\n'
}

add_xmlhead() {
	printf '<?xml version="1.0" ?>\n'
}

# testsuite要素の開始部分を出力する
#
# 全てのテストケースを実行後に呼び出すとテスト数、失敗数も出力する
add_testsuite_start() {
	# テスト総数
	tests=${1-${TESTSUITE_TESTS}}
	# 失敗数
	failures=${2:-${TESTSUITE_FAILURES}}
	# testsuite名
	name=${3:-${TESTSUITE_NAME}}

	printf '<testsuite name="'${name:-"Test"}'" tests="'${tests}'" failures="'${failures}'">\n'
}

# testsuite要素の終了部分を出力する
add_testsuite_end() {
	printf '</testsuite>\n'
}

# testsuites要素の開始部分を出力する
add_testsuites_start() {
	printf '<testsuites>\n'
}

# testsuites要素の終了部分を出力する
add_testsuites_end() {
	printf '</testsuites>\n'
}

