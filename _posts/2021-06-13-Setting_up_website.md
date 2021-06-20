---
layout: post
title: "Setting up personal website with nginx"
---

Here in this post we will setup an nginx server on a Ubuntu/Debian machine and add a
simple page to it to see it in action. Next we will cover how to setup https for
your personally hosted webpages for free. Then at last we will cover setting up
multiple subdomains within a single domain name and host it on a single server.

*P.S: The instructions are specific to Debian/Ubuntu*

## Setting up webserver

- Install `nginx`.
- Check if you can see nginx homepage by browsing the domain.
- If it is not visible then check the ports open your server.
- You can open port 80 through `sudo ufw allow 80`.

### Setting up personal page

- Copy `/etc/nginx/sites-available/default` to
  `/etc/nginx/sites-available/<your-website>`
- Alter the file `/etc/nginx/sites-available/<your-website>` to look like below.

```bash
server {
        listen 80;
        listen [::]:80;

        root /var/www/<your-website>;

        index index.html index.htm index.nginx-debian.html;

        server_name <your-domain>; #Like praton.me

        location / {
                try_files $uri $uri/ =404;
        }
}

```
- Create a symlink between `/etc/nginx/sites-available/<your-website>` and
  `/etc/nginx/sites-enabled/<your-website>`
- Create a dir in `/var/www/home` and put some html in there.
- Reload nginx with `systemctl reload nginx`.
- Now when you browse to the domain you should see your html rendered.

## Setting up https

- Install `cerbot`.
- Install `python-certbot-nginx`
- Execute `certbot --nginx` and follow the instructions.
- Reload nginx.
- Now try browsing the `https` version of your webpage.
- If it is not visible then check the ports open your server.
- You can open port 443 through `sudo ufw allow 443`.
- 443 is needed by `ssl`.

## Setting up multiple subdomains

- You can add multiple subdomains from the domain vendor interface
like that of epik. 
- Otherwise the wildcard entry would just work.
- As shown in the below picture.
![some alt text](/pics/epik_dns.png) 
- Multiple dirs can be created under `/var/www/` to support multiple websites
  with one server and one domain.
	- Like `blog.praton.xyz` has a `var/www/blog` dir and one
	  `/etc/nginx/sites-available/blog` file.
