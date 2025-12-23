@echo off

set UE_EXE="C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe"

set UPROJECT="%~dp0Project_14.uproject"

echo 로비 서버 실행 (Port: 7777)...
start "LobbyServer" %UE_EXE% %UPROJECT% /Game/Level/LobbyLevel -server -log -port=7777

echo 게임 서버1 실행 (Port: 7779)...
start "GameServer1" %UE_EXE% %UPROJECT% /Game/Level/MainLevel -server -log -port=7778

echo 게임 서버2 실행 (Port: 7780)...
start "GameServer2" %UE_EXE% %UPROJECT% /Game/Level/MainLevel -server -log -port=7779



echo 모든 서버가 실행되었습니다.