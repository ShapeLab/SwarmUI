#!/bin/bash

for d in example_*/ ; do
echo "$d"
cd $d
xcodebuild
done