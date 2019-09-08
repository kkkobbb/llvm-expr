#!/bin/sh
# テストの実行
#
# ./run.sh [exefile]
#
# このシェルスクリプトと同じ階層のディレクトリごとにテストを実行する
# テストについて
#     直下のディレクトリにある${SH_BASENAME}ファイルが実行対象となる
#     ${SH_BASENAME}
#         成功時 0、失敗時0以外を返すこと
#         結果は標準出力に出力する
#
# exefile
#     テストするコマンド 指定がなければ各テストのデフォルトになる
#     そのまま各テスト用のコマンドに渡される
#
# 戻り値
#     全てのテストが成功した場合、0
TESTDIR=$(cd $(dirname $0); pwd)

COLOR_SUCCESS="\e[32m"
COLOR_FAILURE="\e[31m"
COLOR_RESET="\e[m"

# テスト用のシェルスクリプト名
SH_BASENAME="run.sh"


# 成否を表す文字列を返す
# $1が0なら成功
get_result_str() {
	ret=$1
	sh_name=$2

	result_str="${COLOR_SUCCESS}Success"
	if [ "${ret}" -ne 0 ]; then
		result_str="${COLOR_FAILURE}Failure"
	fi
	printf "${result_str}:${COLOR_RESET} [${sh_name}]"
}

# テスト用のシェルスクリプトがあるディレクトリを検索
# このシェルスクリプトと同じ階層のディレクトリを探す
test_list=$(cd ${TESTDIR}; find -type f -name ${SH_BASENAME} | \
	grep '\./[^/]*/'${SH_BASENAME} | sed -e "s/${SH_BASENAME}//")

result_msg=""
result=0
for test_dir in ${test_list}; do
	test_fullpath=$(cd "${TESTDIR}/${test_dir}"; pwd)
	test_basedir=$(basename ${test_fullpath})
	echo "${test_basedir}:"
	${test_fullpath}/${SH_BASENAME} "$@"
	ret=$?
	if [ "${ret}" -ne 0 ]; then
		result=1
	fi
	result_msg="${result_msg}$(get_result_str ${ret} ${test_basedir})\n"
	printf "\n\n"
done

printf "Summary:\n"
printf "${result_msg}"

exit ${result}

