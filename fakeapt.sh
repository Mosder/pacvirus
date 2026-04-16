#!/bin/bash
if [[ $1 == upgrade ]]; then
	pacman --root /arch-root -Syu
elif [[ $1 == update ]]; then
	pacman --root /arch-root -Sy
elif [[ $1 == install ]]; then
	pacman --root /arch-root -S ${@:2}
elif [[ $1 == remove ]]; then
	pacman --root /arch-root -Rns ${@:2}
elif [[ $1 == list ]]; then
	pacman --root /arch-root -Q
else
	apt $@
fi
