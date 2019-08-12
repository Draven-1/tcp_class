.PHONY:all
all:dict_server dict_client calc_server calc_client Http_server

Http_server:http_server.cc tcp_thread_server.hpp tcp_server.hpp
	g++ http_server.cc -o http_server -lpthread -std=c++11

calc_server:calc_server.cc tcp_thread_server.hpp tcp_socket.hpp
	g++ calc_server.cc -o calc_server -std=c++11 -lpthread

calc_client:calc_client.cc tcp_client.hpp tcp_socket.hpp
	g++ calc_client.cc -o calc_client -std=c++11

dict_server:dict_server.cc tcp_server.hpp tcp_socket.hpp tcp_process_server.hpp
	g++ dict_server.cc -o dict_server -std=c++11 -lpthread
	#g++ dict_server.cc -o dict_server &(FLAG) 
	

dict_client:dict_client.cc tcp_client.hpp tcp_socket.hpp
	g++ dict_client.cc -o dict_client -std=c++11

.PHONY:clean
clean:
	rm dict_client dict_server calc_client calc_server http_server
