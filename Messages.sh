#! /usr/bin/env bash

$EXTRACTRC `find src -name \*.rc -o -name \*.ui -o -name \*.kcfg` >> muonrc.cpp
$XGETTEXT rc.cpp muonrc.cpp `find src -name \*.cpp` -o $podir/muon.pot
rm -f muonrc.cpp
