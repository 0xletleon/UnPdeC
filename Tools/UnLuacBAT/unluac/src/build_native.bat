@echo off
REM Compile the JAR file to a native executable using GraalVM
native-image -jar unluac.jar

REM Check if the native image compilation was successful
if %ERRORLEVEL% neq 0 (
    echo Native image compilation failed.
    goto end
)

echo Compilation Native image creation were successful.
goto end

:end
pause