docker-build:
	sudo docker build -t myserver .

docker-build-nginx:
	sudo docker build -t nginx ./nginx

docker-run-nginx:
	sudo docker run --rm --network host -d -p 8081:8081 --name nginx -t nginx

docker-run:
	sudo docker run --rm --network host -d -p 8080:8080 --name server -t myserver

docker-fix:
	- sudo killall containerd-shim

docker-stop-nginx:docker-fix
	sudo docker stop nginx

docker-stop:docker-fix
	sudo docker stop server

docker-connect:
	sudo sudo docker exec -it server bash

run-test:
	 ab -c 10000 -n 20000 http://127.0.0.1:8081/

run-vegeta-myserver:
	echo "GET http://127.0.0.1:8080/" | vegeta attack -duration=10s -rate=0 -max-workers=1000 | tee results.bin | vegeta report

run-vegeta-nginx:
	echo "GET http://127.0.0.1:8081/" | vegeta attack -duration=10s -rate=0 -max-workers=1000 | tee results.bin | vegeta report
