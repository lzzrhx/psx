# Playstation 1 Video Game

A Playstation 1 video game made by following the course [PS1 Programming with MIPS Assembly & C](https://pikuma.com/courses/ps1-programming-mips-assembly-language) by Gustavo Pezzi.

> [!NOTE]
> **Windows XP environment set up:**
> - Download and install [Mypal](https://www.mypal-browser.org/)
> - Download and install [Programmer's Notepad v2.4.2.1440](https://github.com/simonsteele/pn/releases/download/v2.4.2/pn2421440_multilang.exe)
> - Download and install [HxD](https://mh-nexus.de/downloads/HxDSetup.zip)
> - Download and install [NO$PSX](http://problemkaputt.de/no$psx.zip)
> - Download and install [Playstation Programmer Tools 2.2](https://drive.google.com/file/d/0B_GAaDjR83rLOGp3aTYzc0haVGs/view?resourcekey=0-uyLemqoC68cNjP5w6dYeQg)
> - Download [PSY-Q SDK](https://drive.google.com/file/d/0B_GAaDjR83rLZGVaZ2pvV2tjSVE/view?resourcekey=0-KKkvWLASG3gmlP3jNqZskQ)
> - Extract PSY-Q SDK to C:\psyq
> - Extract mkpsxiso (from utils\mkpsxiso-1.20.zip) to C:\mkpsxiso
> - Control Panel -> Classic View -> System -> Advanced -> Environment Variables. Add the following:
>     - PATH (Append) -> ;C:\psyq\bin;C:\mkpsxiso
>     - PSX_PATH -> C:\psyq\bin
>     - LIBRARY_PATH -> C:\psyq\lib
>     - C_INCLUDE_PATH -> C:\psyq\include
>     - PSYQ_PATH -> C:\psyq\bin
>     - COMPILER_PATH -> C:\psyq\bin
>     - GO32 -> DPMISTACK 1000000
>     - GO32TMP -> C:\WINDOWS\TEMP
>     - TMP -> C:\WINDOWS\TEMP
> - Run BUILD.BAT to build project as BUILD\GAME.ISO & BUIL\GAME.CUE and load GAME.CUE in a PSX emulator.
> - Build requires:
>     - LICENSEE.DAT
>     - ASSETS\COMMON\ALLSH.CMP
>     - ASSETS\COMMON\ALLSH.PRM
>     - ASSETS\MUSIC\JUNGLE.WAV
>     - ASSETS\SOUND\COUNTGO.VAG
>     - ASSETS\SOUND\COUNT1.VAG
>     - ASSETS\SOUND\COUNT2.VAG
>     - ASSETS\SOUND\COUNT3.VAG
>     - ASSETS\TRACK02\LIBRARY.CMP
>     - ASSETS\TRACK02\LIBRARY.TTF
>     - ASSETS\TRACK02\SCENE.CMP
>     - ASSETS\TRACK02\SCENE.PRM
>     - ASSETS\TRACK02\TRACK.TRF
>     - ASSETS\TRACK02\TRACK.TRS
>     - ASSETS\TRACK02\TRACK.TRV
