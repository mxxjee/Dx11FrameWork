@echo off
setlocal enabledelayedexpansion

REM ========== 설정 ==========
set TEXCONV_PATH=C:\DevTool\texconv.exe
set INPUT_EXT=png
set OUTPUT_EXT=dds
set OUTPUT_FORMAT=BC7_UNORM        REM 원하는 포맷
REM ===========================

echo.
echo ==============================================
echo   PNG → DDS 변환 시작
echo ==============================================
echo.

for %%f in (*.%INPUT_EXT%) do (
    echo 변환중: %%f → %%~nf.%OUTPUT_EXT%
    "%TEXCONV_PATH%" -f %OUTPUT_FORMAT% -o . "%%f"
)

echo.
echo 변환 완료!
pause