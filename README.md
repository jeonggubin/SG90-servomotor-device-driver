# Raspberry Pi 5 SG90 Servo Motor Device Driver

라즈베리파이 5(Raspberry Pi 5) 환경에서 리눅스 커널 모듈을 직접 작성하여 SG90 서보모터를 제어하는 캐릭터 디바이스 드라이버(Character Device Driver) 프로젝트입니다. 커널 영역에서 소프트웨어 PWM(Software PWM)을 구현하고, 유저 영역(User Space) 앱과 `read/write` 가상 파일을 통해 상호작용합니다.

---

## 📌 주요 특징
- **라즈베리파이 5 구조적 변경 대응**: 커널 가상 주소 및 오프셋 방식이 변경된 RPi 5 환경에 맞춰 RP1 칩셋 기본 오프셋(+571)을 반영하여 핀 제어 (`GPIO 22` 사용).
- **소프트웨어 PWM 제어**: 20ms 주기(50Hz) 신호 내에서 안정적으로 500us ~ 2500us 범위의 펄스 폭 제어를 수행하여 서보모터 각도 정밀 조정.
- **가상 디바이스 노드 자동 생성**: `class_create` 및 `device_create` 함수를 활용해 모듈 로드 시 `/dev/my_gpio` 가상 파일을 자동으로 생성하고 `sysfs` 등록.
- **유저-커널 공간 데이터 전달**: `copy_from_user`와 `copy_to_user`를 안전하게 구현하여 양방향 제어 및 모니터링 기능 탑재.

---

## 🔌 회로 구성
<img width="800" height="459" alt="image (3)" src="https://github.com/user-attachments/assets/d618bdb7-a8f8-459a-8241-ab444acd66e1" />

| 서보 모터 핀 (SG90) | 라즈베리파이 핀 | 설명 |
| :--- | :--- | :--- |
| **VCC** (Red) | 2번 핀 (5V) | 모터 전원 공급 |
| **GND** (Brown/Black) | 9번 핀 (GND) | 그라운드 공유 |
| **Signal (PWM)** (Orange/Yellow) | 15번 핀 (GPIO 22) | 커널 드라이버 PWM 제어 신호 출력 |

---

## 📂 파일 구조

```text
📂 pwm_driver
├── 📄 Makefile            # 커널 모듈 및 앱 통합 빌드 스크립트
├── 📄 my_app.c            # 테스트 및 제어용 유저 애플리케이션 코드
└── 📄 my_gpio_driver.c    # 커널 디바이스 드라이버 소스 코드 (read/write fops 구현)
```
---

## 🚀 빌드 및 실행 방법 (Commands)

아래 명령어를 터미널에 차례대로 입력하여 디바이스 드라이버를 빌드하고 실행할 수 있습니다.

```bash
# 1. 작업 폴더 작성 및 이동
mkdir pwm_driver
cd pwm_driver

# 2. 커널 소스 코드 및 드라이버 소스 작성 (vi 또는 nano 편집기 사용)
vi my_gpio_driver.c

# 3. 빌드 시작 (이전 파일 정리 후 컴파일)
make clean && make

# 4. 커널 모듈 로드
sudo insmod my_gpio_driver.ko

# 5. 모듈 로드 확인 및 주번호(Major Number) 찾기
dmesg | tail
cat /proc/devices | grep "my_gpio_dev"

# 6. 디바이스 노드 수동 생성 (dmesg나 /proc/devices에서 확인한 주번호를 입력하세요. 예: 509)
# ※ 주의: 소스 코드 내 device_create로 자동 생성된 경우 이 단계는 건너뛰어도 됩니다.
sudo mknod /dev/my_gpio c [주번호] 0

# 7. 일반 사용자도 접근 가능하도록 디바이스 노드에 권한 부여
sudo chmod 666 /dev/my_gpio

# 8. 유저 애플리케이션 컴파일
gcc -o my_app my_app.c

# 9. 애플리케이션 실행
./my_app
```
---

## 💻 실행 및 테스트 결과
애플리케이션을 실행하여 1000us, 1500us, 2000us의 펄스 폭 제어 명령을 차례대로 전달하는 화면입니다.
## 📊 제어 결과 확인 (로그)
<img src="https://github.com/user-attachments/assets/636c462c-3342-4dd3-bd19-469126173170" width="400" />
<br>
<img src="https://github.com/user-attachments/assets/ec079434-355d-427c-ba27-b65c2c6a8986" width="800" />

## 🎬 동작 영상
https://github.com/user-attachments/assets/3a8123bc-0439-435e-a2c4-35660ca64147

---

## 🧠 배운 점
- **라즈베리파이 5의 GPIO 아키텍처 변화 대응**:
  라즈베리파이 5는 이전 모델(RPi 4 이하)과 달리 자체 개발한 RP1 I/O 컨트롤러 칩셋을 사용하여 GPIO 관리 방식이 커널 가상 주소 및 오프셋 기반으로 바뀌었습니다. 
  개발 과정에서 기존 방식대로 `GPIO 22`에 접근했을 때 오동작하는 문제를 겪었으며, 라즈베리파이 5의 커널 기본 핀 오프셋 값인 `571`을 더해주어야 (`22 + 571`) 실제 물리 핀에 정확히 맵핑된다는 점을 파악하고 코드를 수정하여 문제를 해결했습니다.
