include docker-fastcgi-daemon/Makefile

reload: build restart

test:
	cd testing && make