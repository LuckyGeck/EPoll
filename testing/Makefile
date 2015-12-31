CURDIR := $(shell pwd)

all:
	docker run -t --rm -v $(CURDIR)/load.ini:/load.ini -v $(CURDIR)/artifacts:/artifacts --link fastcgi-nginx:nginx zooniverse/yandex-tank