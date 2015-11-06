#! /usr/bin/env bash

$EXTRACTRC `find muon/ -name \*.rc -o -name \*.ui -o -name \*.kcfg` >> muonrc.cpp
$XGETTEXT rc.cpp muonrc.cpp `find muon -name \*.cpp` -o $podir/muon.pot
rm -f muonrc.cpp
