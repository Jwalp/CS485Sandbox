cd "cs485_sandbox_test_suite/bin/public"
verbose=0
if [ "$2" == "verbose" ]; then
	verbose=1
fi
case "$1" in
	"milestone1")
		echo "Running Test Suite for milestone 1"
		cd "milestone1"
		binaries=(*)
		echo -e "${binaries[@]}\n"
		for binary in "${binaries[@]}"; do
			stdout=$("./$binary")
			if echo "$stdout" | grep -q "FAIL"; then
				echo "FAILED $binary"
			elif echo "$stdout" | grep -q "PASS"; then
				echo "PASSED $binary"
			else
				echo "ERROR ENCOUNTERED IN $binary"
			fi
			if [ $verbose == 1 ]; then
				echo "$stdout"
			fi
			echo -e "\n"
		done
		;;
	"milestone2")
		echo "Running Test Suite for milestone 2"
		cd "milestone2"
		binaries=(*)
		echo -e "${binaries[@]}\n"
		for binary in "${binaries[@]}"; do
			stdout=$("./$binary")
			if echo "$stdout" | grep -q "FAIL"; then
				echo "FAILED $binary"
				echo "$stdout"
			elif echo "$stdout" | grep -q "PASS"; then
				echo "PASSED $binary"
			else
				echo "ERROR ENCOUNTERED IN $binary"
			fi
			echo -e "\n"
		done
		;;
	"final")
		echo "Running Test Suite for final"
                cd "milestone2"
                binaries=(*)
                echo -e "${binaries[@]}\n"
                for binary in "${binaries[@]}"; do
                        stdout=$("./$binary")
                        if echo "$stdout" | grep -q "FAIL"; then
                                echo "FAILED $binary"
                                echo "$stdout"
                        elif echo "$stdout" | grep -q "PASS"; then
                                echo "PASSED $binary"
                        else
                                echo "ERROR ENCOUNTERED IN $binary"
                        fi
                        echo -e "\n"
		done
		;;
	"")
		echo "INFO: Usage: ./testsuite.sh (milestone1|milestone2|final)"
		echo "INFO: Ensure that you are running this within your sandbox"
		echo "INFO: Ensure that have precompiled the binaries before running"
		;;
	*)
		echo "Could not find test suite"
		;;
esac
echo "Exiting"
