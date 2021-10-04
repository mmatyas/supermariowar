# Super Mario War

![Linux status][build-linux-img] ![Windows status][build-mingw-img] [![AppVeyor][appveyor-img]][appveyor-link] [![Discord][discord-img]][discord-link]

Super Mario War는 Super Mario Bros의 팬이 만든 멀티플레이어 버전이다. 다양한 모드에서 플레이어가 서로를 이기는 것이 목적인 데스메치 형식의 게임이다. 당신은 이 게임을 팀으로도 플레이 할 수도 있고, 당신만의 레벨이나 세계를 구현할 수도 있으며, 그 이상도 가능하다!!!

- [History](#history)
- [About](#about)
- [Building](#building-instructions)
  - [Get the code](#get-the-code)
  - [Requirements](#requirements)
  - [Linux](#linux)
  - [Mac OSX](#mac-osx)
  - [Windows](#windows)
  - [ARM / Raspberry Pi](#arm-devices)
  - [Android](#android)
  - [Emscripten](#emscripten)
  - [Other devices](#other-devices)
  - [Build configuration](#build-configuration)
- [How to play](#how-to-play)


## History

시초가 되는 *Mario War*는 2002년 *Samuele Poletto* 가 만들었는데, 4명의 슈퍼마리오 캐릭터가 여러 레벨에서 서로의 머리에 점프를 함으로써 대결을 할 수 있었다. 이 게임은 파스칼/어셈블리어 (Pascal/Assembly)로 제작이 되었고, DOS용으로 배포 되었다. 이후 버전들에는 맵 에디터도 포함되었다.

2004년에 *72dpiarmy*라는 포럼의 설립자 *Florian Hufsky*가 오픈소스로 새롭게 작성하면서 *Super Mario War*로 시작됐다. 이 버전에서는 커스텀 캐릭터와, 추가적인 게임모드, 메카닉 맵과, 일반 아이템과 강화 아이템들까지 소개 했었다. 유저가 만든 커스텀 컨텐츠들은 `smwstuff.com` 라는 수천개의 맵과 스킨들을 이용할 수 있는 사이트에 저장되었다 (지금은 사라진 사이트이다). 오픈 소스라는 특성으로 인해 이 게임은 거의 모든 데스크탑과 콘솔 시스템으로 이식되었다.

2009년 말, Florian이 죽고 게임의 개발은 점차 느려졌고 결국 Super Mario War 1.8 beta 2 가 마지막 공식 릴리즈 버전으로 개발이 중단 되었다. 개발을 계속하거나 다시 시작하려는 여러 시도들이 있었지만, 그 시도들 모두 장기적으로 봤을 때 성공한 것 같지 않았다. 또한 기술적인 문제로 인해 `smwstuff.com` 사이트 전체와 모든 내용이 손실 되었고, 포럼에서 이용가능 하던 업로더의 백업들의 부분도 같이 손실 되었다.

이것이 2014년쯤 부터 내가 포크를 하여 작업을 시작한 이유이며, 크로스 플랫폼을 구현하여 서로 다른 플랫폼에서 네트워크 멀티플레이를 구현하는 것이 초기 목표이다. 그리고 위에서 `smwstuff.com` 포럼에서 흩어진 콘텐츠들을 한 곳에 수집할 사이트를 아래 주소에 새롭게 다시 한번 열게 되었다. 아래 주소는 현재 운영중인 사이트이다. 주소: [`smwstuff.net`](http://smwstuff.net).

게임의 경우, 불행하게도 원본 소스코드의 수준과 구조가 효과적인 작업이 불가능하다는 것이 확실해졌다. 대부분의 소스 파일에는 약 10000줄가량이었고 1000개 이상의 LOC함수가 공통적이며, 대부분의 전역 변수를 수정 했고, 100개의 분기를 가진 `if-else`도 있었다.

결국 나는 네트워크 멀티플레이를 작동하게 할 수는 있었지만, 완벽하지는 않았다. 좋은 조건에서는 로컬 네트워크에서는 잘 작동했지만, 미묘한 버그와 지연이 일반적으로 인터넷을 이용한 게임플레이를 불안하게 했다. 적절한 구현을 위해서는 게임의 몇가지 핵심 부분과 코드를 유지 관리하기 위해 수백개의 정리와 리팩터링 패치를 다시 디자인해야 할 것입니다. 물론 이런 종류의 프로젝트를 통해 언제든지 어떤 회사에서든 중단하라는 연락을 받을 수 있다.

2016년은 나에게 이 프로젝트에 시간을 더 이상 투자하지 않은 시점이었다. 장기적으로 보면 전체를 새로 쓰는 것은 원래 게임을 패치하는 것보다 더 빠르고 효과적일 수 있었다. 어느 쪽이든, 나는 프로젝트를 이전보다 더 나은 상태로 두고 미래 개발자에게 유용하기를 바란다. 재밌게 즐기길!


## About

Super Mario War는 Super Mario의 멀티플레이 버전 게임이다. 게임의 목표는 마리오가 가능한 많이 승리하는 것이다. 게임에는 서로 다른 여러개의 게임 모드가 있는데, 깃발잡기, 일인자, 데스매치, 팀 데스매치, 토너먼트 모드, 코인 모으기, 경주 그외에 여러가지가 있다. 이 게임에는 레벨 에디터가 포함되어 있어 맵을 처음부터 새롭게 만들거나 좋아하는 마리오 게임에서 구역을 재창조 할 수 있습니다. 당신의 상상력이 한계를 결정합니다! 최근 버전에는 토너먼트와 통합된 슈퍼마리오 브라더스 3 와 같은 플레이를 만들기 위해서 지정된 조건으로 여러 레벨을 묶을 수 있는 월드 에디터도 포함되어 있다. 이 게임은 Nintendo와 Samuele Poletto가 만든 원조 팬메이드 게임 Mario War에 대한 찬사다.

이 게임은 Nintendo의 아트워크와 사운드를 사용한다. 우리는 이 비상업적 팬 게임이 공정한 사용으로 자격을 얻기를 바란다. 단지 우리는 Nintendo의 캐릭터와 게임을 우리가 얼마나 좋아하는지 보여주기 위해 이 게임을 만들고 싶었다.

### Features

- 최대 4명의 플레이어가 즐길 수 있는 데스매치
- 다양한 게임 모드 (featuring GetTheChicken, Domination, CTF, ...)
- 온라인과 로컬 멀티플레이
- 레벨 에디터와 함께 제공 - 자신만의 맵 제작
- 그리고 많은 사람들의 그들만의 맵 제작. 이로 인해 현재 맵은 1000개를 넘어간다
- 막대기로 원숭이를 찌르는 것들보다 훨씬 재밌다(텀블링 몽키?)
- 모든 게임에 관련된 소스코드는 무료로 이용가능하
- SDL을 사용하여 windows, linux, mac 등에 모두 호환된다
- CPU 플레이어와 함께 할 수 있다
- 당신을 즐겁게 할 것이다!!!

### Supported platforms

- Linux
- Windows
- Mac OS X
- ARM devices (예를 들어 Raspberry Pi-라즈베리 파이)
- XBox (?)
- Android (실험중)
- asm.js (실험중)


## Building instructions

### Requirements

- C++11를 지원하는 컴파일러 (예 gcc-4.8)
- CMake (>= 2.6)
- SDL (1.2 또는 2.0) 그리고,
    - SDL_image
    - SDL_mixer
- zlib
- yaml-cpp (포함됨)
- ENet (선택, 포함됨)

저런 조건을 위해 패키지 매니저를 사용 할 수 있다. 각 시스템 별 코드:

- Debian-based: `apt-get install cmake libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev zlib1g-dev`
- Fedora/RPM: `yum install cmake SDL-devel SDL_image-devel SDL_mixer-devel zlib-devel`
- Arch: `pacman -S cmake sdl sdl_image sdl_mixer zlib`
- MSYS2: `pacman -S mingw-w64-x86_64-SDL mingw-w64-x86_64-SDL_image mingw-w64-x86_64-SDL_mixer mingw-w64-x86_64-zlib`

다른 시스템들을 사용한다면 아래의 주소에서 개발파일을 수동 다운로드:

- http://www.cmake.org/cmake/resources/software.html#latest
- http://www.libsdl.org/download-1.2.php
- http://www.libsdl.org/projects/SDL_image/release-1.2.html
- http://www.libsdl.org/projects/SDL_mixer/release-1.2.html
- http://zlib.net

### Get the code

이 저장소에는 당신의 운영체제와 맞지 않거나 당신이 시스템에 설치하는 것을 원하지 않는 경우들에 사용할 수 있는 일부 하위 모듈들을 포함하고 있다. 포함되어 있는 라이브러리들을 이용하면 재귀적인 복사를 할 수 있을 것이다:

`git clone --recursive https://github.com/mmatyas/supermariowar.git`

또는 하위 모듈을 수동으로 초기화도 가능하다:

```sh
git clone https://github.com/mmatyas/supermariowar.git
cd supermariowar
git submodule update --init
```

오히려 시스템 라이브러리를 사용하려는 경우, [Build configuration](#build-configuration) 의 이 기능을 비활성화 시키는 부분을 참고할 수 있다.

### Linux

빌드할 디렉토리를 작성하고 CMake를 실행하여 프로젝트를 구성한다. 그리고 빌드하려고 할 때마다 `make`를 호출해라. 바이너리는 기본적으로 `./build/Binaries/Release` 에 생성된다. 간단히 말하자면:

```sh
unzip data.zip
mkdir build && cd build
cmake ..
make -j4 # -jN = build on N threads
./Binaries/Release/smw ../data
```

`make`의 주요 빌드 대상:

- smw
- smw-leveleditor
- smw-worldeditor
- smw-server

당신이 IDE에서 개발하기를 더 선호한다면 (CodeBlocks, Eclipse, ...), CMake를 활용해서 프로젝트 파일을 생성할 수 있을 것이다. 여기서 더 많은 정보를 찾을 수 있을 것이다 [Build configuration](#build-configuration).

설치 가능한 패키지를 만들려면 `make package`를 실행하라. 그러면 TGZ, DEB, RPM 파일들이 만들어 질 것이다.

### Mac OSX

당신은 아마 Xcode와 command line 툴들이 필요 할 것이다; 사이트에서 SDL과 CMake를 수동으로 설치하거나 Homebrew를 통해 받을 수 있다: `brew install cmake sdl2 sdl2_image sdl2_mixer`. 그리고나서 Linux의 설명을 따라 Super Mario War 를 빌드하면 된다.

### Windows

만약 당신이 MinGW Shell/MSYS 와 Cygwin를 사용한다면, Linux 가이드를 따르면 된다. 당신은 CodeBlocks, Eclipse, Visual Studio 같은 많은 IDE들을 CMake를 이용하여 프로젝트 파일을 생성할 수도 있다.

더 자세한 것은 wiki에서: [Building on Windows](https://github.com/mmatyas/supermariowar/wiki/Building-on-Windows).

### ARM devices

당신은 라즈베리 파이 같은 ARM기반 기기에서 Linux의 가이드를 따라 SMW를 빌드할 수 있다. 만약 당신이 어떻게 하는지 안다면, 크로스 툴체인을 설정하거나 장치를 에뮬레이션하는 방법들로 크로스 컴파일을 할 수 있다. 더 자세한 것은 wiki에서: [Cross compiling to ARM](https://github.com/mmatyas/supermariowar/wiki/Cross-compiling-to-ARM).

빌드 구성에는 기본 컴파일러 신호가 포함되어 있지만, 그러나 가능한 많은 조합이 있기 때문에 (hard float, Thumb, NEON, ...), 변수를 새로 만들어 사용 하고 싶을 수 있다. 이런 경우에, CFLAGS 와 CXXFLAGS의 변수를 정의하고, `DISABLE_DEFAULT_CFLAGS` 옵션으로 CMake를 실행해라 (참고 [Build configuration](#build-configuration)).

### Android

Android 포트는 다른 빌드 시스템을 사용하므로 아래 주소에서 자세한 내용을 찾을 수 있다. [이곳](https://github.com/mmatyas/supermariowar-android).

### Emscripten

SMW 당신의 브라우저에서 [Emscripten](https://kripken.github.io/emscripten-site/)이것을 사용해서 빌드할 수 있다. 당신은 빌드 방법을 여기서 찾을 수 있다[wiki](https://github.com/mmatyas/supermariowar/wiki/Building-the-Emscripten-port).

### Other devices

SDL(1.2 또는 2.0) 이 작동하는 모든 장치에 SMW을 이식할 수 있을 것이다. 일반적으로, 다음 단계들을 포함한다:

- 너의 기기에 맞는 크로스 컴파일러 툴체인을 받아라 
- SDL 라이브러리들이 포함되어있지 않으면 크로스 컴파일해라
  - SDL 빌드
  - PNG를 이용해 SDL_image 빌드
    - zlib 필요
    - libpng 필요
  - OGG를 이용해 SDL_mixer 빌드
    - libogg 필요
    - libvorbis 필요
- CMake 툴체인 파일 생성 및 컴파일러와 경로 정의 
- 툴체인 파일을 이용하여 빌드 

### Build configuration

*TODO: expand this section*

다양한 CMake 신호를 설정하여 빌드 구성을 변경할 수 있다. 이걸 하는 가장 간단한 방법은 `Build` 디렉토리에서 `cmake-gui ..` 파일을 실행하는 것이다. 이름에 마우스를 가져다 대면 요소에 관한 간단한 설명을 읽을 수 있다.

대신에, 이런 옵션들을 아래처럼 CMake에 직접 전달할 수 있다 `-DFLAGNAME=VALUE` (예 `cmake .. -DUSE_SDL2_LIBS=1`).


## How to play

docs/ 폴더 안의 문서를 참고해 주세요.


[build-linux-img]: https://github.com/mmatyas/supermariowar/actions/workflows/build_linux.yml/badge.svg
[build-mingw-img]: https://github.com/mmatyas/supermariowar/actions/workflows/build_mingw.yml/badge.svg
[appveyor-img]: https://ci.appveyor.com/api/projects/status/github/mmatyas/supermariowar?svg=true
[appveyor-link]: https://ci.appveyor.com/project/mmatyas/supermariowar
[discord-img]: https://img.shields.io/badge/Discord-7389D8?logo=discord&logoColor=white
[discord-link]: https://discord.gg/SC4uXQB
