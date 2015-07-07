#Faneuil hall solution:<br>
To build on Mac:<br>
clang++ -std=c++14 faneuil_hall.cpp immigrant.cpp spectator.cpp judge.cpp -o faneuil_hall.exe

To run:<br>
./faneuil_hall.exe<br>

7.5 The Faneuil Hall problem<br>
This problem was written by Grant Hutchins, who was inspired by a friend who took her Oath of Citizenship at Faneuil Hall in Boston.
“There are three kinds of threads: immigrants, spectators, and a one judge. Immigrants must wait in line, check in, and then sit down. At some point, the judge enters the building. When the judge is in the building, no one may enter, and the immigrants may not leave. Spectators may leave. Once all immigrants check in, the judge can confirm the naturalization. After the confirmation, the immigrants pick up their certificates of U.S. Citizenship. The judge leaves at some point after the confirmation. Spectators may now enter as before. After immigrants get their certificates, they may leave.”
To make these requirements more specific, let’s give the threads some func- tions to execute, and put constraints on those functions.<br>
• Immigrants must invoke enter, checkIn, sitDown, swear, getCertificate and leave.<br>
• The judge invokes enter, confirm and leave.<br>
• Spectators invoke enter, spectate and leave.<br>
• While the judge is in the building, no one may enter and immigrants may not leave.<br>
• The judge can not confirm until all immigrants who have invoked enter have also invoked checkIn.<br>
Immigrants can not getCertificate until the judge has executed confirm.<br>
