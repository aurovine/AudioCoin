#!/bin/bash
# create multiresolution windows icon
ICON_DST=../../src/qt/res/icons/audiocoin.ico

convert ../../src/qt/res/icons/audiocoin-16.png ../../src/qt/res/icons/audiocoin-32.png ../../src/qt/res/icons/audiocoin-48.png ${ICON_DST}
