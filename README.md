# Raspberry Pi 5 SG90 Servo Motor Device Driver

라즈베리파이 5(Raspberry Pi 5) 환경에서 리눅스 커널 모듈을 직접 작성하여 SG90 서보모터를 제어하는 캐릭터 디바이스 드라이버(Character Device Driver) 프로젝트입니다. 커널 영역에서 소프트웨어 PWM(Software PWM)을 구현하고, 유저 영역(User Space) 앱과 `read/write` 가상 파일을 통해 상호작용합니다.

---

## 📌 주요 특징
- **라즈베리파이 5 구조적 변경 대응**: 커널 가상 주소 및 오프셋 방식이 변경된 RPi 5 환경에 맞춰 RP1 칩셋 기본 오프셋(+571)을 반영하여 핀 제어 (`GPIO 22` 사용).
- **소프트웨어 PWM 제어**: 20ms 주기(50Hz) 신호 내에서 안정적으로 500us ~ 2500us 범위의 펄스 폭 제어를 수행하여 서보모터 각도 정밀 조정.
- **가상 디바이스 노드 자동 생성**: `class_create` 및 `device_create` 함수를 활용해 모듈 로드 시 `/dev/my_gpio` 가상 파일을 자동으로 생성하고 `sysfs` 등록.
- **유저-커널 공간 데이터 전달**: `copy_from_user`와 `copy_to_user`를 안전하게 구현하여 양방향 제어 및 모니터링 기능 탑재.

---

## 🔌 회로 구성 (Circuit Configuration)

| 서보 모터 핀 (SG90) | 라즈베리파이 핀 | 설명 |
| :--- | :--- | :--- |
| **VCC** (Red) | 2번 핀 (5V) | 모터 전원 공급 |
| **GND** (Brown/Black) | 9번 핀 (GND) | 그라운드 공유 |
| **Signal (PWM)** (Orange/Yellow) | 15번 핀 (GPIO 22) | 커널 드라이버 PWM 제어 신호 출력 |

---

## 📂 파일 구조 (File Structure)

```text
📂 pwm_driver
├── 📄 my_gpio_driver.c    # 커널 디바이스 드라이버 소스 코드 (read/write fops 구현)
├── 📄 my_app.c            # 테스트 및 제어용 유저 애플리케이션 코드
├── 📄 Makefile            # 커널 모듈 및 앱 통합 빌드 스크립트
└── 📄 README.md           
