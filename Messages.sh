#! /usr/bin/env bash

$EXTRACTRC `find src -name \*.rc -o -name \*.ui -o -name \*.kcfg` >> rc.cpp
$XGETTEXT rc.cpp `find src -name \*.cpp` -o $podir/muon.pot
