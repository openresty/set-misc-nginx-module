# vi:filetype=perl

use lib 'lib';
use Test::Nginx::Socket;

#repeat_each(3);

plan tests => repeat_each() * 2 * blocks();

no_long_string();

run_tests();

#no_diff();

__DATA__

=== TEST 1: sha1 hello (copy)
--- config
    location /sha1 {
        set_sha1 $a hello;
        echo $a;
    }
--- request
GET /sha1
--- response_body
aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d



=== TEST 2: sha1 hello (in-place)
--- config
    location /sha1 {
        set $a hello;
        set_sha1 $a;
        echo $a;
    }
--- request
GET /sha1
--- response_body
aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d



=== TEST 3: sha1 (empty)
--- config
    location /sha1 {
        set_sha1 $a "";
        echo $a;
    }
--- request
GET /sha1
--- response_body
da39a3ee5e6b4b0d3255bfef95601890afd80709

