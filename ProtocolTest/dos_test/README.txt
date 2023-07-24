cd C:\Users\dnwls\OneDrive - 성균관대학교\바탕 화면\파스텍\week1\ProtocolTest\dos_test
gcc -c UDP_Client.c -o UDP_Client.o
gcc -c Command.c -o Command.o
gcc -c Get_IP_Address.c -o Get_IP_Address.o
gcc -c dos_test.c -o dos_test.o
gcc dos_test.o Get_IP_Address.o UDP_Client.o Command.o -o my_program
my_program.exe

./my_program