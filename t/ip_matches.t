# vi:filetype=

use lib 'lib';
use Test::Nginx::Socket;

#repeat_each(3);

plan tests => repeat_each() * 2 * blocks();

no_long_string();

run_tests();

#no_diff();

__DATA__

=== TEST 1: IP 10.1.0.101 matches 10.0.0.0/8
--- config
    location /bar {
        set_ip_matches $ip_matches 10.0.0.0/8 10.1.0.101;
        echo $ip_matches;
    }
--- request
    GET /bar
--- response_body
1



=== TEST 2: IP 10.1.0.101 does not match 10.0.0.0/24
--- config
    location /bar {
        set_ip_matches $ip_matches 10.0.0.0/24 10.1.0.101;
        echo $ip_matches;
    }
--- request
    GET /bar
--- response_body
0



    

