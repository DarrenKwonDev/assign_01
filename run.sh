#!/bin/bash

while true; do
  clear
  make main -B && ./main < input.txt
  sleep 1
done
