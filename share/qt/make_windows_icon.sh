#!/bin/bash
# create multiresolution windows icon
ICON_DST=../../src/qt/res/icons/growcoin.ico

convert ../../src/qt/res/icons/growcoin-16.png ../../src/qt/res/icons/growcoin-32.png ../../src/qt/res/icons/growcoin-48.png ${ICON_DST}
