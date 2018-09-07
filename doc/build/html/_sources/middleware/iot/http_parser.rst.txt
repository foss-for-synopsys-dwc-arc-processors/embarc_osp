.. _middleware_parser:

HTTP Parser
###########

HTTP Parser is a parser for HTTP messages written in C. It parses both
requests and responses. The parser is designed to be used in performance HTTP
applications. It does not make any syscalls nor allocations. It does not
buffer data. It can be interrupted at anytime. Depending on the architecture, it
may require as little as 40 bytes of data per message stream (in a web server that
is per connection).
