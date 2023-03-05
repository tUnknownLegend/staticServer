curl 'http://127.0.0.1:8080/' \
  -H 'accept: application/json' \
  -H 'accept-language: en-US,en;q=0.9,ru-RU;q=0.8,ru;q=0.7' \
  -H 'content-type: application/json' \
  -H 'user-agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36' \
  --data-raw '{"password":"123456","email":"vlad@mail.com"}' \
  --compressed