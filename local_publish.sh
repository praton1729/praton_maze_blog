#! /bin/bash

echo "Building website...."

jekyll build

echo "Publishing website"

jekyll serve
