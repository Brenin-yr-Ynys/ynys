#!/bin/bash

rm src/styleguide.cpp
$RACK_DIR/helper.py createmodule StyleGuide res/styleguide.svg src/styleguide.cpp
make install
