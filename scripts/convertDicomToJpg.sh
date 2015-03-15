#!/bin/bash

for i in *.dcm;
do
  begin="raw-"
  number=${i//[^[:digit:]]/}
  if [ "$number" -lt 100 ]; then
    begin=${begin}0
    if [ "$number" -lt 10 ]; then
      begin=${begin}0
    fi
  fi
  convert  $i ${begin}${number}.jpg
done
