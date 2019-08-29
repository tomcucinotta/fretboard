#!/bin/bash

echo "Fretboard - visual chords for guitar or ukulele on an ANSI terminal"
echo "Copyright (c) 2019 by Tommaso Cucinotta"
mode="-g"
echo
fretboard $mode

while true; do
    echo
    echo "Type a chord name (Am, G, C7, Fmaj7, ...), or any of the commands:"
    echo "  exit (or x) ... exit the program"
    echo "  guit (or g) ... guitar mode"
    echo "  ukul (or u) ... ukulele mode"
    echo -n "your choice: "
    read crd

    if [ "$crd" == "exit" -o "$crd" == "x" ]; then
        exit
    elif [ "$crd" == "ukul" -o "$crd" == "u" ]; then
        mode="-u"
        echo "Switched to ukulele mode"
    elif [ "$crd" == "guit" -o "$crd" == "g" ]; then
        mode="-g"
        echo "Switched to guitar mode"
    else
        echo
        fretboard $mode -c $crd
    fi
done
