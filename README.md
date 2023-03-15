# Static server

### Functional testing

```text
test_directory_index (__main__.HttpServer)
directory index file exists ... ok
test_document_root_escaping (__main__.HttpServer)
document root escaping forbidden ... ok
test_empty_request (__main__.HttpServer)
Send empty line ... ok
test_file_in_nested_folders (__main__.HttpServer)
file located in nested folders ... ok
test_file_not_found (__main__.HttpServer)
absent file returns 404 ... ok
test_file_type_css (__main__.HttpServer)
Content-Type for .css ... ok
test_file_type_gif (__main__.HttpServer)
Content-Type for .gif ... ok
test_file_type_html (__main__.HttpServer)
Content-Type for .html ... ok
test_file_type_jpeg (__main__.HttpServer)
Content-Type for .jpeg ... ok
test_file_type_jpg (__main__.HttpServer)
Content-Type for .jpg ... ok
test_file_type_js (__main__.HttpServer)
Content-Type for .js ... ok
test_file_type_png (__main__.HttpServer)
Content-Type for .png ... ok
test_file_type_swf (__main__.HttpServer)
Content-Type for .swf ... ok
test_file_urlencoded (__main__.HttpServer)
urlencoded filename ... ok
test_file_with_dot_in_name (__main__.HttpServer)
file with two dots in name ... ok
test_file_with_query_string (__main__.HttpServer)
query string with get params ... ok
test_file_with_slash_after_filename (__main__.HttpServer)
slash after filename ... ok
test_file_with_spaces (__main__.HttpServer)
filename with spaces ... ok
test_head_method (__main__.HttpServer)
head method support ... ok
test_index_not_found (__main__.HttpServer)
directory index file absent ... ok
test_large_file (__main__.HttpServer)
large file downloaded correctly ... ok
test_post_method (__main__.HttpServer)
post method forbidden ... ok
test_request_without_two_newlines (__main__.HttpServer)
Send GET without to newlines ... ok
test_server_header (__main__.HttpServer)
Server header exists ... ok

----------------------------------------------------------------------
Ran 24 tests in 0.030s

OK
```

### Benchmark
Tool: [vegeta](https://github.com/tsenart/vegeta)

Commands to run test
```bash
make run-vegeta-myserver # my server
make run-vegeta-nginx # nginx
```

Results:
```text
Requests      [total, rate, throughput]         192984, 19243.19, 19241.49
Duration      [total, attack, wait]             10.03s, 10.029s, 882.607µs
Latencies     [min, mean, 50, 90, 95, 99, max]  57.113µs, 39.913ms, 35.772ms, 76.156ms, 91.883ms, 136.836ms, 335.396ms
Bytes In      [total, mean]                     26824776, 139.00
Bytes Out     [total, mean]                     0, 0.00
Success       [ratio]                           100.00%
Status Codes  [code:count]                      200:192984 
```

Nginx results:
```text
Requests      [total, rate, throughput]         428467, 42830.61, 42811.92
Duration      [total, attack, wait]             10.008s, 10.004s, 4.367ms
Latencies     [min, mean, 50, 90, 95, 99, max]  32.898µs, 6.812ms, 4.103ms, 17.641ms, 22.803ms, 35.567ms, 155.671ms
Bytes In      [total, mean]                     59556913, 139.00
Bytes Out     [total, mean]                     0, 0.00
Success       [ratio]                           100.00%
Status Codes  [code:count]                      200:428467  
```
