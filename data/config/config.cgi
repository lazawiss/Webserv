server {
    listen       0.0.0.0:8080; 
    server_name  localhost;

    client_max_body_size 10M;


    location /cgi-bin/python {
        root            /var/www/cgi-bin;
        methods         GET POST;
        cgi_extension   .py  /usr/bin/python3;
        index           index.py;
    }

    location /cgi-bin/php {
        root            /var/www/cgi-bin;
        methods         GET POST;
        cgi_extension   .php /usr/bin/php-cgi;
        index           index.php;
    }
}
