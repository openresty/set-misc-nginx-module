# vi:filetype=

use lib 'lib';
use Test::Nginx::Socket;

#repeat_each(3);

plan tests => repeat_each() * 2 * blocks();

no_long_string();

#master_on();
#log_level("warn");

run_tests();

#no_diff();

__DATA__


=== TEST 1: set consistent hashed upstream
buggy?
--- config
    upstream_list universe moon flower earth;
    location /foo {
        set_consistent_hashed_upstream $backend universe $arg_id;
        echo $backend;
    }
    location /main {
        echo_location_async /foo;
        echo_location_async /foo?id=fjdjfjndsaf;
        echo_location_async /foo?id=4588934;
        echo_location_async /foo?id=fdsf;
        echo_location_async /foo?id=752b5302-f017-49b7-a42b-5e817ed3ddb4;
        echo_location_async /foo?id=ccdccacc;
        echo_location_async /foo?id=ccdccacd;
        echo_location_async /foo?id=ccdccace;
        echo_location_async /foo?id=ccdccacf;
        echo_location_async /foo?id=ccdccacg;
        echo_location_async /foo?id=ccdccach;
    }
--- request
GET /main
--- response_body
moon
earth
flower
flower
moon
earth
flower
earth
flower
moon
moon



=== TEST 2: set consistent hashed upstream (checking consistency)
buggy?
--- config
    upstream_list universe1 moon flower earth;
    upstream_list universe2 moon flower earth son;
    location /foo {
        set_consistent_hashed_upstream $backend universe1 $arg_id;
        echo $backend;
    }
    location /bar {
        set_consistent_hashed_upstream $backend universe2 $arg_id;
        echo $backend;
    }
    location /main {
        echo_location_async /foo?id=fjdjfjndsaf;
        echo_location_async /foo?id=752b5302-f017-49b7-a42b-5e817ed3ddb4;
        echo_location_async /foo?id=ccdccacf;
        echo_location_async /foo?id=ccdccacg;
        echo_location_async /foo?id=ccdccach;
        echo_location_async /foo?id=4588934;
        echo_location_async /bar?id=fjdjfjndsaf;
        echo_location_async /bar?id=752b5302-f017-49b7-a42b-5e817ed3ddb4;
        echo_location_async /bar?id=ccdccacf;
        echo_location_async /bar?id=ccdccacg;
        echo_location_async /bar?id=ccdccach;
        echo_location_async /bar?id=4588934; #Expected: 'flower' -> 'son'
    }
--- request
GET /main
--- response_body
earth
moon
flower
moon
moon
flower
earth
moon
flower
moon
moon
son

