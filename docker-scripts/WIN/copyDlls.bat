copy "J:\GitHub\Rtcw-Pro\output\qagame_mp_x86.dll" "J:\GitHub\Rtcw-Pro\src\rtcwpro\" /Y
copy "J:\GitHub\Rtcw-Pro\output\cgame_mp_x86.dll" "J:\GitHub\Rtcw-Pro\src\rtcwpro\" /Y
copy "J:\GitHub\Rtcw-Pro\output\ui_mp_x86.dll" "J:\GitHub\Rtcw-Pro\src\rtcwpro\" /Y

copy "J:\GitHub\Rtcw-Pro\output\qagame_mp_x86.dll" "C:\Program Files (x86)\Return to Castle Wolfenstein\rtcwpro" /Y
copy "J:\GitHub\Rtcw-Pro\output\cgame_mp_x86.dll" "C:\Program Files (x86)\Return to Castle Wolfenstein\rtcwpro" /Y
copy "J:\GitHub\Rtcw-Pro\output\ui_mp_x86.dll" "C:\Program Files (x86)\Return to Castle Wolfenstein\rtcwpro" /Y

copy "J:\GitHub\Rtcw-Pro\output\qagame_mp_x86.dll" "C:\Program Files (x86)\Return to Castle Wolfenstein Copy\rtcwpro" /Y
copy "J:\GitHub\Rtcw-Pro\output\cgame_mp_x86.dll" "C:\Program Files (x86)\Return to Castle Wolfenstein Copy\rtcwpro" /Y
copy "J:\GitHub\Rtcw-Pro\output\ui_mp_x86.dll" "C:\Program Files (x86)\Return to Castle Wolfenstein Copy\rtcwpro" /Y

cd ../..
docker run -it --rm -v %cd%:/rtcwpro rtcwpro:build dos2unix /rtcwpro/docker-scripts/build/build-pk3.sh 
docker run -it --rm -v %cd%:/rtcwpro rtcwpro:build ./rtcwpro/docker-scripts/build/build-pk3.sh

copy "J:\GitHub\Rtcw-Pro\output\qagame_mp_x86.dll" "J:\GitHub\RtCW-Pro\src\rtcwpro" /Y
copy "J:\GitHub\Rtcw-Pro\output\cgame_mp_x86.dll" "J:\GitHub\RtCW-Pro\src\rtcwpro" /Y
copy "J:\GitHub\Rtcw-Pro\output\ui_mp_x86.dll" "J:\GitHub\RtCW-Pro\src\rtcwpro" /Y
copy "J:\GitHub\Rtcw-Pro\output\rtcwpro.pk3" "J:\GitHub\Rtcw-Pro\src\rtcwpro" /Y
copy "J:\GitHub\Rtcw-Pro\output\rtcwpro.pk3" "C:\Program Files (x86)\Return to Castle Wolfenstein\rtcwpro" /Y
copy "J:\GitHub\Rtcw-Pro\output\rtcwpro.pk3" "C:\Program Files (x86)\Return to Castle Wolfenstein Copy\rtcwpro" /Y
pause
