# Static server

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