echo 

case "$1" in
	"milestone1")
		echo "Running Test Suite for milestone 1"
		cd "cs485_sandbox_test_suite/bin/public/milestone1" # this may not be the correct relative root. test and change
		binaries=(*)
		echo "${binaries[@]}"
		for binary in "${binaries[@]}"; do
			stdout=$(./"$binary")
			if grep -q "FAIL" <<< "$stdout"; then
				echo "FAILED $binary"
				echo "$stdout"
			else
				echo "PASSED $binary"
			fi
			echo -e "\n"
		done
		;;
	"milestone2")
		echo "Compiling Test Suite for milestone 2"
		# WIP
		;;
	"final")
		echo "Compiling Test Suite for final"
		# WIP
		;;
	"")
		echo "Usage: ./testsuite.sh (milestone1|milestone2|final)"
		echo "Ensure that you are running this within your sandbox"
		;;
	*)
		echo "Could not find test suite"
		;;
esac
echo "Exiting"
