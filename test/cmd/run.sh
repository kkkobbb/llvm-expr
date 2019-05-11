#!/bin/sh
# コマンド実行、ファイル出力の動作確認
#
# ./run.sh [options] [exefile]
#
#   [options]
#     -x   xmlとして出力する
#
# run.shと同じディレクトリで拡張子が${TESTEXT}のファイルがテストケースとなる
# コマンドライン引数でexefileの指定があればその実行ファイルでテストする
#
# exefile
#     テストするコマンド 指定がなければ同じ階層のコマンド
#
# 戻り値
#     成功時 0
EXEFILE="./exparrc"
TESTDIR="$(cd $(dirname $0); pwd)"
TESTEXT="test.sh"

. ${TESTDIR}/../run_output_xml.sh

COLOR_SUCCESS="\e[32m"
COLOR_FAILURE="\e[31m"
COLOR_RESET="\e[m"


# -x オプション
output_xml=false
if [ "$1" = "-x" ]; then
	output_xml=true
	shift
fi

# 実行ファイルの指定
test $# -ne 0 && EXEFILE="$1"
# 実行ファイルの存在確認
test -f ${EXEFILE} || { echo "not found '${EXEFILE}'"; exit 1; }


if ${output_xml}; then
	XML_TMP=$(mktemp)
	classname=$(basename ${TESTDIR})
fi

# 出力保存用一時ファイル
out=$(mktemp)
err=$(mktemp)
msg=$(mktemp)

testcaselist=$(ls ${TESTDIR}/*.${TESTEXT})
test_num=0
success_num=0
for testcase in ${testcaselist}; do
	test_num=$((test_num + 1))

	(sh ${testcase} ${EXEFILE}) > ${out} 2> ${err} 3> ${msg}
	ret=$?

	test "${ret}" -eq 0 && success_num=$((success_num + 1))

	if ${output_xml}; then
		name=$(basename ${testcase})
		(add_testcase ${ret} ${name} ${classname} ${out} ${err} ${msg}) >> ${XML_TMP}
		continue
	fi

	if [ "${ret}" -eq 0 ]; then
		printf "${COLOR_SUCCESS}Success:${COLOR_RESET}"
	else
		cat ${out} ${err} ${msg}
		printf "${COLOR_FAILURE}Failure:${COLOR_RESET}"
	fi

	echo "  [$(basename ${testcase})]"
done

# 一時ファイル削除
rm -f ${out} ${err} ${msg}

# 全体の成否
all_result=0
if [ "${success_num}" -ne "${test_num}" ]; then
	all_result=1
fi

# xml出力
if ${output_xml}; then
	failures=$((test_num - success_num))

	(add_xmlhead ;
	add_testsuite_start ${test_num} ${failures} ${classname} ;
	cat ${XML_TMP} ;
	rm -f ${XML_TMP} ;
	add_testsuite_end)
	exit ${all_result}
fi

# 全体の成否で色を変える
color="${COLOR_SUCCESS}"
if [ "${all_result}" -ne 0 ]; then
	color="${COLOR_FAILURE}"
fi
printf "\n${color}${success_num} / ${test_num}${COLOR_RESET}\n"

exit ${all_result}

