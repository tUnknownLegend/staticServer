make docker:build:
	docker build -t myserver

make docker:run:
	docker run --rm -p 80:80 --name server -t myserver