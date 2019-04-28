#!/bin/sh

RUN_DIR=$(dirname $0)

COLOR_SUCCESS="\e[32m"
COLOR_FAILURE="\e[31m"
COLOR_RESET="\e[m"


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


result=""

lang_sh="${RUN_DIR}/lang/run.sh"
echo "${lang_sh}:"
${lang_sh} "$@"
ret=$?
result="${result}$(get_result_str ${ret} ${lang_sh})\n"
printf "\n\n"

cmd_sh="${RUN_DIR}/cmd/run.sh"
echo "${cmd_sh}:"
${cmd_sh} "$@"
ret=$?
result="${result}$(get_result_str ${ret} ${cmd_sh})\n"
printf "\n\n"


printf "All Result:\n"
printf "${result}"

