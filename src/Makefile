default:
	@echo "No target found."

server:
	@gcc -pthread server.c -o server && ./server

user: 
	@gcc -pthread user.c -o user && ./user $(socketName) $(Username)

clean:
	@rm -rf ./server ./user