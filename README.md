# (unfinished) Playstation 1 Video Game

A Playstation 1 video game made by following the course [PS1 Programming with MIPS Assembly & C](https://pikuma.com/courses/ps1-programming-mips-assembly-language) by Gustavo Pezzi.

Environment set up (on Windows XP):
- Download and install [Mypal](https://www.mypal-browser.org/)
- Download and install [Programmer's Notepad v2.4.2.1440](https://github.com/simonsteele/pn/releases/download/v2.4.2/pn2421440_multilang.exe)
- Download and install [HxD](https://mh-nexus.de/downloads/HxDSetup.zip)
- Download and install [NO$PSX](http://problemkaputt.de/no$psx.zip)
- Download and install [Playstation Programmer Tools 2.2](https://drive.google.com/file/d/0B_GAaDjR83rLOGp3aTYzc0haVGs/view?resourcekey=0-uyLemqoC68cNjP5w6dYeQg)
- Download [PSY-Q SDK](https://drive.google.com/file/d/0B_GAaDjR83rLZGVaZ2pvV2tjSVE/view?resourcekey=0-KKkvWLASG3gmlP3jNqZskQ)
- Extract PSY-Q SDK to C:\psyq
- Extract CDTOOLS (from utils\CDTOOLS.zip) to C:\psyq\bin
- Control Panel -> Classic View -> System -> Advanced -> Environment Variables. Add the following:
    - PATH (Append) -> ;C:\psyq\bin
    - PSX_PATH -> C:\psyq\bin
    - LIBRARY_PATH -> C:\psyq\lib
    - C_INCLUDE_PATH -> C:\psyq\include
    - PSYQ_PATH -> C:\psyq\bin
    - COMPILER_PATH -> C:\psyq\bin
    - GO32 -> DPMISTACK 1000000
    - GO32TMP -> C:\WINDOWS\TEMP
    - TMP -> C:\WINDOWS\TEMP

