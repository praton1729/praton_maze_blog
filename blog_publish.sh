#! /bin/bash

echo "Building production website...."

JEKYLL_ENV=production bundle exec jekyll build

echo "Publishing production website on praton.me"

rsync -r --info=progress2 _site/* blog_machine:/home/tnn1sqgvuaxk/public_html

echo "Publishing production website on blog.praton.xyz"
rsync -r _site/* blog_vultr:/var/www/blog/
