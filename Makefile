trans_of_city:clean
	g++ keshe.cpp -o trans_of_city -lmysqlclient
clean:
	-rm trans_of_city
