#!/bin/sh
# コマンド実行、ファイル出力の動作確認
#
#
EXEFILE="./exparrc"
TESTDIR="$(dirname $0)"
TESTEXT="test.sh"

COLOR_SUCCESS="\e[32m"
COLOR_FAILURE="\e[31m"
COLOR_RESET="\e[m"


# 実行ファイルの指定
test $# -eq 1 && EXEFILE="$1"
# 実行ファイルの存在確認
test -f ${EXEFILE} || { echo "not found '${EXEFILE}'"; exit 1; }


testcaselist=$(ls ${TESTDIR}/*.${TESTEXT})
test_num=0
success_num=0
for testcase in ${testcaselist}; do
	test_num=$((test_num + 1))

	sh ${testcase} ${EXEFILE}
	ret=$(echo $?)

	if [ "${ret}" -eq 0 ]; then
		success_num=$((success_num + 1))
		printf "${COLOR_SUCCESS}Success:${COLOR_RESET}"
	else
		printf "${COLOR_FAILURE}Failure:${COLOR_RESET}"
	fi

	echo "  [${testcase}]"
done

# 全体の成否で色を変える
color="${COLOR_SUCCESS}"
if [ "${success_num}" -ne "${test_num}" ]; then
	color="${COLOR_FAILURE}"
fi
printf "\n${color}${success_num} / ${test_num}${COLOR_RESET}\n"

