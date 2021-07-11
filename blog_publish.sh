#! /bin/bash

#echo "Publishing production website on praton.me"
#rsync -r --info=progress2 _site/* blog_machine:/home/tnn1sqgvuaxk/public_html

echo "Publishing production website on blog.praton.xyz"
rsync --info=progress2 -r _site/* blog_vultr:/var/www/blog/
