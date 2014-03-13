# vi:filetype=

use lib 'lib';
use Test::Nginx::Socket;

#repeat_each(3);

plan tests => repeat_each() * 2 * blocks();

no_long_string();

run_tests();

#no_diff();

__DATA__

=== TEST 1: IPv4 address matches IPv4 CIDR
--- config
    location /bar {
        set_ip_matches $ip_matches 10.0.0.0/8 10.1.0.101;
        echo $ip_matches;
    }
--- request
    GET /bar
--- response_body
1



=== TEST 2: IPv4 address does not match IPv4 CIDR
--- config
    location /bar {
        set_ip_matches $ip_matches 10.0.0.0/24 10.1.0.101;
        echo $ip_matches;
    }
--- request
    GET /bar
--- response_body
0


=== TEST 3: IPv4 address matches IPv4 address
--- config
    location /bar {
        set_ip_matches $ip_matches 10.1.0.101 10.1.0.101;
        echo $ip_matches;
    }
--- request
    GET /bar
--- response_body
1



=== TEST 4: IPv4 address does not match IPv4 address
--- config
    location /bar {
        set_ip_matches $ip_matches 10.1.0.102 10.1.0.101 ;
        echo $ip_matches;
    }
--- request
    GET /bar
--- response_body
0



=== TEST 5: IPv6 address matches IPv6 CIDR
--- config
    location /bar {
        set_ip_matches $ip_matches 3ffe::/16 3ffe:1900:4545:3:200:f8ff::67cf;
        echo $ip_matches;
    }
--- request
    GET /bar
--- response_body
1



=== TEST 6: IPv6 address does not match IPv6 CIDR
--- config
    location /bar {
        set_ip_matches $ip_matches 3fff::/16 3ffe:1900:4545:3:200:f8ff::67cf;
        echo $ip_matches;
    }
--- request
    GET /bar
--- response_body
0


=== TEST 7: IPv6 address matches IPv6 address
--- config
    location /bar {
        set_ip_matches $ip_matches 3ffe:1900:4545:3:200:f8ff::67cf 3ffe:1900:4545:3:200:f8ff::67cf;
        echo $ip_matches;
    }
--- request
    GET /bar
--- response_body
1


=== TEST 7: IPv6 address does not match IPv6 address
--- config
    location /bar {
        set_ip_matches $ip_matches 3ffe:1900:4545:3:200:f8ff::67d0 3ffe:1900:4545:3:200:f8ff::67cf;
        echo $ip_matches;
    }
--- request
    GET /bar
--- response_body
0

