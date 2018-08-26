#!/bin/sh

EXEFILE="exprc"
if [ ! -f ${EXEFILE} ]; then
	echo "not found '${EXEFILE}'"
	exit 1
fi

TESTDIR="test"
testlist=$(ls ${TESTDIR}/*.expr)

testnum=$(echo ${testlist} | wc -w)
snum=0
for src in ${testlist}; do
	expected=$(grep "^##return " ${src} | sed "s/^##return //")

	./${EXEFILE} ${src} -print-llvm | lli
	ret=$?

	if [ ${ret} -eq ${expected} ]; then
		printf "Success:"
		snum=$((snum + 1))
	else
		printf "Failure (ret=${ret} exp=${expected}):"
	fi
	echo "  ${src}"
done

echo "${snum} / ${testnum}"


