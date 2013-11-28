# vi:filetype=perl

use lib 'lib';
use Test::Nginx::Socket;
use POSIX qw(strftime);

our $str_gmt = strftime "%a %b %e %H:%M %Y\n", gmtime;
our $str_local = strftime "%a %b %e %H:%M %Y\n", localtime;

#repeat_each(2);

plan tests => repeat_each() * 2 * blocks();

log_level('warn');

run_tests();

#no_diff();

__DATA__

=== TEST 1: local time format
--- config
    location /foo {
        set_formatted_local_time $today "%a %b %e %H:%M %Y";
        echo $today;
    }
--- request
GET /foo
--- response_body eval: $main::str_local

=== TEST 2: GMT time format
--- config
    location /bar {
        set_formatted_gmt_time $today "%a %b %e %H:%M %Y";
        echo $today;
    }
--- request
GET /bar
--- response_body eval: $main::str_gmt

