echo  *** copying OpenMama files
rem %1=$(Configuration) %2="$(OutDir)" %3="$(ProjectDir)" 

xcopy %TICK42_OPENMAMA%\x64\%1\mamalistenc.exe %2 /FY
xcopy %TICK42_OPENMAMA%\x64\%1\libwombatcommonmdd.dll %2 /FY
xcopy %TICK42_OPENMAMA%\x64\%1\libmamacmdd.dll %2 /FY

echo *** copying bloomberg files ***
xcopy %TICK42_BLPAPI_CPP%\bin\blpapi3_64.dll %2 /FY

echo *** copying local files ***

xcopy %3mama.properties %2 /FY
xcopy %3mamalistenc.cmd %2 /FY
xcopy %3readme.txt %2 /FY

echo *** copying fieldsmap.csv file ***
xcopy %3fieldmap.csv %2 /FY