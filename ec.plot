#!/usr/bin/gnuplot
#gnuplot -f ec.plot
plot "< tail -n 2000 ec.txt" using 0:2 w l
pause 0.2; refresh; reread;
