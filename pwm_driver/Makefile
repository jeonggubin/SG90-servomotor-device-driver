# 빌드할 커널 모듈 객체 지정
# my_gpio_driver.c → my_gpio_driver.o → my_gpio_driver.ko 로 생성됨
obj-m += my_gpio_driver.o

# 기본 빌드 타겟 (make 입력 시 실행됨)
all:
	# 현재 실행 중인 커널 버전에 맞는 빌드 디렉토리로 이동(-C)
	# M=$(PWD) : 현재 디렉토리를 외부 모듈로 빌드하겠다는 의미
	# modules  : 커널 모듈(.ko) 파일 생성
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
	# 유저 공간 애플리케이션을 일반 gcc로 컴파일
	# my_app.c → 실행파일 my_app 생성
	gcc -o my_app my_app.c

# 빌드 산출물 삭제 타겟 (make clean 실행 시 동작)
clean:
	# 커널 모듈 빌드 결과물(.o, .ko, .mod 등) 정리
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	
	# 유저 실행 파일 삭제
	rm -f my_app
