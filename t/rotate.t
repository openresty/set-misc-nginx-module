# vi:filetype=

use Test::Nginx::Socket;

repeat_each(2);

plan tests => repeat_each() * (3 * blocks());

no_long_string();

run_tests();

#no_diff();

__DATA__

=== TEST 1: sanity
--- config
    location /bar {
        set $a 1;
        set_rotate $a 1 3;

        set $b 2;
        set_rotate $b 1 3;

        set $c 3;
        set_rotate $c 1 3;

        set $d 0;
        set_rotate $d 1 3;

        set $e 1;
        set_rotate $e 3 5;

        echo "a = $a";
        echo "b = $b";
        echo "c = $c";
        echo "d = $d";
        echo "e = $e";
    }
--- request
    GET /bar
--- response_body
a = 2
b = 3
c = 1
d = 1
e = 3
--- no_error_log
[error]



=== TEST 2: bad current value
--- config
    location /bar {
        set $a abc;
        set_rotate $a 1 3;

        echo "a = $a";
    }
--- request
    GET /bar
--- response_body
a = 2
--- error_log
set_rotate: bad current value: "abc"



=== TEST 3: bad "from" value
--- config
    location /bar {
        set $a 2;
        set_rotate $a abc 3;

        echo "a = $a";
    }
--- request
    GET /bar
--- response_body_like: 500 Internal Server Error
--- error_code: 500
--- error_log
set_rotate: bad "from" argument value: "abc"



=== TEST 4: bad "to" argument value
--- config
    location /bar {
        set $a 2;
        set_rotate $a 1 abc;

        echo "a = $a";
    }
--- request
    GET /bar
--- response_body_like: 500 Internal Server Error
--- error_code: 500
--- error_log
set_rotate: bad "to" argument value: "abc"

