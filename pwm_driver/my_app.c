#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/my_gpio", O_RDWR);
    if (fd < 0) {
        perror("열기 실패");
        return -1;
    }

    int target_duty = 2000;
    int current_duty = 0;

    // 1. 모터 각도 쓰기 (Write)
    printf("모터 각도 설정: %dus\n", target_duty);
    write(fd, &target_duty, sizeof(int));

    // 2. 현재 설정된 값 읽기 (Read)
    read(fd, &current_duty, sizeof(int));
    printf("드라이버로부터 읽어온 현재 값: %dus\n", current_duty);

    close(fd);
    return 0;
}
