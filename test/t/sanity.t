# vi:filetype=perl

use lib 'lib';
use Test::Nginx::Socket;

repeat_each(1);

plan tests => repeat_each() * 3 * blocks();

no_long_string();

run_tests();

#no_diff();

__DATA__

=== TEST 1: set if empty
--- config
    location /foo {
        set $a 32;
        set_if_empty $a 56;
        echo $a;

        set_if_empty $b 72;
        echo $b;
    }
--- request
GET /foo
--- response_body
32
72



=== TEST 2: set if empty
--- config
    location /foo {
        set $bar $arg_bar;
        set_if_empty $bar 15;
        echo $bar;

        set $bah $arg_bah;
        set_if_empty $bah 25;
        echo $bah;
    }
--- request
GET /foo?bar=71
--- response_body
71
25

