# vi:filetype=

use lib 'lib';
use Test::Nginx::Socket;

#repeat_each(3);

plan tests => repeat_each() * 2 * blocks();

no_long_string();

run_tests();

#no_diff();

__DATA__

=== TEST 1: expired for 1970-01-01 00:00:00
--- config
    location /bar {
        set_expired $expired 0;
        echo $expired;
    }
--- request
    GET /bar
--- response_body
1



=== TEST 2: expired for 1970-01-01 00:00:01
--- config
    location /bar {
        set_expired $expired 1;
        echo $expired;
    }
--- request
    GET /bar
--- response_body
1



=== TEST 3: expired for 2020-01-01 00:00:00
--- config
    location /bar {
        set_expired $expired 1577836800;
        echo $expired;
    }
--- request
    GET /bar
--- response_body
0



=== TEST 4: expired for 9999-01-01 00:00:00
--- config
    location /bar {
        set_expired $expired 253370764800;
        echo $expired;
    }
--- request
    GET /bar
--- response_body
0

