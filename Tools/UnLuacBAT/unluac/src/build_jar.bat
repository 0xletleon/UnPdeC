@echo off
REM Compile the Java source files in the unluac directory
javac -verbose -deprecation -Werror -d build unluac/*.java

REM Check if the compilation was successful
if %ERRORLEVEL% neq 0 (
    echo Compilation failed.
    goto end
)

REM Create the JAR file
jar cvfm unluac.jar META-INF\MANIFEST.MF -C build .

REM Check if the JAR creation was successful
if %ERRORLEVEL% neq 0 (
    echo JAR creation failed.
    goto end
)

echo Compilation and JAR creation were successful.
goto end

:end
pause