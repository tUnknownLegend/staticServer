make docker-build:
	sudo docker build -t myserver .

make docker-run:
	sudo docker run --rm -d -p 8080:8080 --name server -t myserver

docker-fix:
	- sudo killall containerd-shim

make docker-stop:docker-fix
	sudo docker stop server

docker-connect:
	sudo sudo docker exec -it server bash

make run-test:
	vegeta attack -duration=2223s -rate=450 -max-workers=1000 -http2 | tee results.bin | vegeta report

run-test:
	 ab -c 10000 -n 20000 http://127.0.0.1:8080/

run-vegeta:
	echo "GET http://127.0.0.1:8080/" | vegeta attack -duration=10s -rate=0 -max-workers=1000 | tee results.bin | vegeta report
 