#! /bin/bash

echo "Building website...."

bundle exec jekyll build

echo "Publishing website"

bundle exec jekyll serve
