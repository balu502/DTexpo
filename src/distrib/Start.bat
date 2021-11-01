echo off
if not exist Temp\DTExpo.exe copy DTExpo.arch DTExpo.exe & DTExpo.exe -y -oTemp
cd Temp
DTExpo.exe & cd.. & if exist DTExpo.exe del DTExpo.exe

exit