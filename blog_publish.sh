#! /bin/bash

echo "Building production website...."

JEKYLL_ENV=production bundle exec jekyll build

echo "Publishing production website"

rsync -r --info=progress2 _site/* blog_machine:/home/tnn1sqgvuaxk/public_html
