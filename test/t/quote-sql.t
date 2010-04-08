# vi:filetype=perl

use lib 'lib';
use Test::Nginx::Socket;

#repeat_each(3);

plan tests => repeat_each() * 2 * blocks();

no_long_string();

run_tests();

#no_diff();

__DATA__

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


