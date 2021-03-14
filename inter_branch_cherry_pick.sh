#!/bin/bash -x

SOURCE_BRANCH=$1
DEST_BRANCH=$2

# Collect changes from the source branch
git checkout $SOURCE_BRANCH
git log --oneline --reverse --author='@oneplus.com' > changes.txt

# Filter commit SHA from the file and cherry-pick the commits
git checkout $DEST_BRANCH
#cat changes.txt | awk '{print $1}' | xargs -I{} git cherry-pick {}
cat changes.txt | awk '{print $1}' | xargs -I{} echo {}

