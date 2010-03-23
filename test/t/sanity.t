# vi:filetype=perl

use lib 'lib';
use Test::Nginx::Socket;

#repeat_each(3);

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
--- config
    location /foo {
        set $foo "hello\n\r'\"\\";
        set_quote_sql_str $foo $foo;
        echo $foo;
    }
--- request
GET /foo
--- response_body
'hello\n\r\'\"\\'



=== TEST 6: set quote sql value (in place)
--- config
    location /foo {
        set $foo "hello\n\r'\"\\";
        set_quote_sql_str $foo;
        echo $foo;
    }
--- request
GET /foo
--- response_body
'hello\n\r\'\"\\'



=== TEST 7: set unescape uri
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



=== TEST 8: set unescape uri (in-place)
buggy?
--- config
    location /foo {
        set $foo "hello%20world";
        set_unescape_uri $foo;
        echo $foo;
    }
--- request
GET /foo
--- response_body
hello world



=== TEST 9: set hashed upstream
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



=== TEST 10: set hashed upstream (use var for upstream_list name)
buggy?
--- config
    upstream_list universe moon sun earth;
    location /foo {
        set $list_name universe;
        set_hashed_upstream $backend $list_name $arg_id;
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



=== TEST 11: set quote empty sql value
--- config
    location /foo {
        set $foo "";
        set_quote_sql_str $foo;
        echo $foo;
    }
--- request
GET /foo
--- response_body
null



=== TEST 12: set quote null sql value
--- config
    location /foo {
        set_quote_sql_str $foo;
        echo $foo;
    }
--- request
GET /foo
--- response_body
null



=== TEST 13: unescape '+' to ' '
--- config
    location /bar {
        set $a 'a+b';
        set_unescape_uri $a;
        echo $a;
    }
--- request
    GET /bar
--- response_body
a b



=== TEST 14: base32 (5 bytes)
--- config
    location /bar {
        set $a 'abcde';
        set_encode_base32 $a;
        set $b $a;
        set_decode_base32 $b;

        echo $a;
        echo $b;
    }
--- request
    GET /bar
--- response_body
c5h66p35
abcde



=== TEST 15: base32 (1 byte)
--- config
    location /bar {
        set $a '!';
        set_encode_base32 $a;
        set $b $a;
        set_decode_base32 $b;

        echo $a;
        echo $b;
    }
--- request
    GET /bar
--- response_body
44======
!



=== TEST 16: base32 (1 byte) - not in-place editing
--- config
    location /bar {
        set $a '!';
        set_encode_base32 $a $a;
        set_decode_base32 $b $a;

        echo $a;
        echo $b;
    }
--- request
    GET /bar
--- response_body
44======
!



=== TEST 17: base32 (hello world)
--- config
    location /bar {
        set $a '"hello, world!\nhiya"';
        set_encode_base32 $a;
        set $b $a;
        set_decode_base32 $b;

        echo $a;
        echo $b;
    }
--- request
    GET /bar
--- response_body
49k6ar3cdsm20trfe9m6888ad1knio92
"hello, world!
hiya"

