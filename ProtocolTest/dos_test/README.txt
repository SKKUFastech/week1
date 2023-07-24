gcc -c UDP_Client.c -o UDP_Client.o
gcc -c Get_IP_Address.c -o Get_IP_Address.o
gcc -c dos_test.c -o dos_test.o
gcc dos_test.o Get_IP_Address.o -o my_program
./my_program