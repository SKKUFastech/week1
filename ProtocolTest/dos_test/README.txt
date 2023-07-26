cd C:\Users\dnwls\OneDrive - 성균관대학교\바탕 화면\파스텍\week1\ProtocolTest\dos_test

cd /home/fastech/Desktop/week1
gcc -c UDP_Client_Send.c -o UDP_Client_Send.o
gcc -c UDP_Client_Connect.c -o UDP_Client_Connect.o
gcc -c Command.c -o Command.o
gcc -c Get_IP_Address.c -o Get_IP_Address.o
gcc -c dos_test.c -o dos_test.o
gcc dos_test.o Get_IP_Address.o UDP_Client_Send.o UDP_Client_Connect.o Command.o -o my_program
./my_program

my_program.exe

라즈베리파이 ip설정 방법.
A. 라즈베리파이의 고정ip를 192.168.0.xxx로 설정해줘야 한다. https://khstu-98.tistory.com/38 참고
  1. sudo nano /etc/dhcpcd.conf 파일 들어가기
  2. 마지막 줄에 다음 내용 추가
    interface eth0
    static ip_address=192.168.0.xxx //xxx부분 다른 기기와 겹치지 않게 설정하기.
    static routers=192.168.0.1
  3. crtl+s, ctrl+x 로 저장 및 파일 나오기
  4. sudo /etc/init.d/networking restart networking service를 재시작.
  5. sudo reboot 라즈베리파이 재시작.

https://prod.liveshare.vsengsaas.visualstudio.com/join?62ED0FE949850630D126953AB70F33864617