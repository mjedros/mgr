#!/bin/bash

for i in *.jpg;
do
  begin="converted-"
  number=${i//[^[:digit:]]/}
  if [ "$number" -lt 100 ]; then
    begin=${begin}0
    if [ "$number" -lt 10 ]; then
      begin=${begin}0
    fi
  fi
  mv $i ${begin}${number}.jpg
done
