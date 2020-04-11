#! /bin/bash

echo "Building production website...."

JEKYLL_ENV=production bundle exec jekyll build

echo "Publishing production website"

rsync -r _site/* blog_machine:/home/yynj07hrhveh/public_html
