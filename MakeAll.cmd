@Path=E:\Telink\SDK;E:\Telink\SDK\jre\bin;E:\Telink\SDK\opt\tc32\tools;E:\Telink\SDK\opt\tc32\bin;E:\Telink\SDK\usr\bin;E:\Telink\SDK\bin;%PATH%
@set SWVER=_v0102
@del /Q .\bin
make -s -j clean
make -s -j PROJECT_NAME=Z03MMC%SWVER% POJECT_DEF="-DBOARD=BOARD_LYWSD03MMC"
@if not exist "bin\Z03MMC%SWVER%.bin" goto :error
make -s -j clean
make -s -j PROJECT_NAME=ZCGDK2%SWVER% POJECT_DEF="-DBOARD=BOARD_CGDK2"
@if not exist "bin\ZCGDK2%SWVER%.bin" goto :error
make -s -j clean
make -s -j PROJECT_NAME=ZMHOC122%SWVER% POJECT_DEF="-DBOARD=BOARD_MHO_C122"
@if not exist "bin\ZMHOC122%SWVER%.bin" goto :error
@exit
:error
echo "Error!"

         