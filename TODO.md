# TODO

## libkc3
 - [ ] unveil
 - [ ] pledge

## HTTPd
 - [.] partial requests
   - `s_tag * http_request_parse_range(s_str *range_header, s_tag *dest);`
     recoit le header HTTP "Range:" et retourne une liste de tuples
       {debut, fin} .
   - 
 - [ ] SSL

---

[ ] : Not implemented
[.] : Work in progress
[?] : Waiting review
[x] : Done
