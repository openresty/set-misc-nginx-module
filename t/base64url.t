# vi:filetype=

use lib 'lib';
use Test::Nginx::Socket;

#repeat_each(3);

plan tests => repeat_each() * 2 * blocks();

no_long_string();

run_tests();

#no_diff();

__DATA__

=== TEST 1: base64 encode binary
--- config
    location /bar {
        set_decode_hex $out "f8f9fafbfcfdfeff";
        set_encode_base64 $out;
        echo $out;
    }
--- request
    GET /bar
--- response_body
+Pn6+/z9/v8=



=== TEST 2: base64 decode binary
--- config
    location /bar {
        set_decode_base64 $out "+Pn6+/z9/v8=";
        set_encode_hex $out;
        echo $out;
    }
--- request
    GET /bar
--- response_body
f8f9fafbfcfdfeff



=== TEST 3: base64url encode binary (url safe)
--- config
    location /bar {
        set_decode_hex $out "f8f9fafbfcfdfeff";
        set_encode_base64url $out;
        echo $out;
    }
--- request
    GET /bar
--- response_body
-Pn6-_z9_v8



=== TEST 4: base64url decode binary (url safe)
--- config
    location /bar {
        set_decode_base64url $out "-Pn6-_z9_v8";
        set_encode_hex $out;
        echo $out;
    }
--- request
    GET /bar
--- response_body
f8f9fafbfcfdfeff

