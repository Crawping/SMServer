SMServer
========
IOCP Multi Thread 기반의 Server

-글로벌 버퍼 할당 방식으로 캐시 효율이 높음.

-CircularBuffer의 구현으로 버퍼 재사용이 가능해 메모리 효율이 높음.

-손쉬운 패킷 핸들링.

-주요 객체에 오브젝트풀 적용.

사용법
========
PacketDefine에서 패킷 정의해주고,

ClientSession.h에서 패킷 등록해주고, 플레이어에 대한 정보를 수정하고,

PacketHandler.cpp에서 패킷 핸들링해준다.
