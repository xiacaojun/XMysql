all:libLXMysql.so test_LXMysql install
	#./test_LXMysql
libLXMysql.so:LXData.cpp LXMysql.cpp LXMysql.h LXData.h
	g++ -fPIC -shared $^ -o $@ -I/usr/include/mysql -lmysqlclient
test_LXMysql:../test_LXMysql/test_LXMysql.cpp
	g++ $^ -o $@ -I./ -lLXMysql -L./
install:
	cp libLXMysql.so /usr/lib/
	@echo "install libLXMysql.so success!"
clean:
	rm -rf *.so
	rm -rf /usr/lib/libLXMysql.so
	rm -rf test_LXMysql
