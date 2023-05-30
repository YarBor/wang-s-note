#!/bin/bash

count=$(git log --since=midnight --count | wc --chars)

if [[ $count -eq 0 ]]; then
  echo "rubbish" > a.rubbish
  git add a.rubbish
  git commit -m '1'
  git push
  rm a.rubbish
fi


