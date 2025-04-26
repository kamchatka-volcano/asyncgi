#!/bin/bash
{
  printf "HTTP/1.1 200 OK\r\n"
  printf "Content-Length: 5\r\n"
  printf "Content-Type: text/plain\r\n"
  printf "Connection: close\r\n"
  printf "Set-Cookie: foo=bar; Secure\r\n"
  printf "X-Test: foobar\r\n"
  printf "\r\n"
  printf "Hello"
} | nc -l 8091

