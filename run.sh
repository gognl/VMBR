#!/bin/sh

victim=1
attacker=1

while getopts "avh" flag; do
    case $flag in
        a)
            victim=0
            ;;
        v)
            attacker=0
            ;;
        h)
            echo "\033[38;5;74mUsage: \033[38;5;69m$0 [-v] [-a] [-h]\033[38;5;74m"
            echo "  -v      Run only the victim"
            echo "  -a      Run only the attacker"
            echo "  -h      Display help\033[0m"
            exit 1
    esac
done

if [ "${victim}" -eq "${attacker}" ]; then
    echo "\033[38;5;208mRunning the VMBR attacker & victim\033[0m"
    cd victim
    python ../attacker/main.py & make
elif [ "${attacker}" -eq 1 ]; then
    echo "\033[38;5;208mRunning the VMBR attacker\033[0m"
    python ./attacker/main.py
elif [ "${victim}" -eq 1 ]; then
    echo "\033[38;5;208mRunning the VMBR victim\033[0m"
    cd victim
    make
fi
