#vi:filetype=perl

use lib 'lib';
use Test::Nginx::Socket;

#repeat_each(3);

plan tests => repeat_each() * 2 * blocks();

no_long_string();

run_tests();

#no_diff();

__DATA__

=== TEST 1: set escape uri
--- config
	location /foo {
		set $foo "hello world";
		set_escape_uri $foo $foo;
		echo $foo;
	}
--- request
GET /foo
--- response_body
hello%20world



=== TEST 2: set escape uri(in-place)
--- config
	location /foo {
		set $foo "hello world";
		set_escape_uri $foo;
		echo $foo;
	}
--- request
GET /foo
--- response_body
hello%20world



=== TEST 3: blank string
--- config
	location /foo {
		set $foo "";
		set_escape_uri $foo;
		echo $foo;
	}
--- request
GET /foo
--- response_body eval
"\n"



=== TEST 4: blank string(in place)
--- config
	location /foo {
		set $foo "";
		set_escape_uri $foo;
		echo $foo;
	}
--- request
GET /foo
--- response_body eval
"\n"



=== TEST 5: eacape chinese character
--- config
    location /foo {
        set $foo "你好";
        set_escape_uri $foo;
        echo $foo;
    }
--- request
GET /foo
--- response_body
%e4%bd%a0%e5%a5%bd

