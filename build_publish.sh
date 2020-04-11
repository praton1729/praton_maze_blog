#! /bin/bash

echo "Building website...."

jekyll build

echo "Publishing website"

rsync -r _site/* blog_machine:/home/yynj07hrhveh/public_html
