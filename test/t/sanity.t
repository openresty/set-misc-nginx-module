# vi:filetype=perl

use lib 'lib';
use Test::Nginx::Socket;

repeat_each(3);

plan tests => repeat_each() * 2 * blocks();

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



=== TEST 3: set if empty
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
GET /foo?bar=
--- response_body
15
25



=== TEST 4: set if empty (using arg_xxx directly)
buggy?
--- config
    location /foo {
        set_if_empty $arg_bar 15;
        echo $arg_bar;

        set_if_empty $arg_bah 25;
        echo $arg_bah;
    }
--- request
GET /foo?bar=71
--- response_body
15
25



=== TEST 5: set quote sql value
buggy?
--- config
    location /foo {
        set $foo "hello\n\r'\"\\";
        set_quote_sql_value $foo $foo;
        echo $foo;
    }
--- request
GET /foo
--- response_body
'hello\n\r\'\"\\'



=== TEST 6: set unescape uri
buggy?
--- config
    location /foo {
        set $foo "hello%20world";
        set_unescape_uri $foo $foo;
        echo $foo;
    }
--- request
GET /foo
--- response_body
hello world



=== TEST 7: set hashed upstream
buggy?
--- config
    upstream_list universe moon sun earth;
    location /foo {
        set_hashed_upstream $backend universe $arg_id;
        echo $backend;
    }
    location /main {
        echo_location_async /foo;
        echo_location_async /foo?id=hello;
        echo_location_async /foo?id=world;
        echo_location_async /foo?id=larry;
        echo_location_async /foo?id=audreyt;
    }
--- request
GET /main
--- response_body
moon
sun
moon
earth
earth

