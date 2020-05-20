#!/bin/sh

pipe_path="/tmp/dwm_status.fifo"

[ ! -e $pipe_path ] && mkfifo $pipe_path

clock -f 'C%a %d %b %H:%M' -si 10 > $pipe_path &
battery -f 'B%s %i' -si 60 > $pipe_path &
writevol.sh &

while read -r line; do
    case $line in
        B*)
            fbat="${line#?}"
            s=$(echo $fbat | cut -d' ' -f1)
            n=$(echo $fbat | cut -d' ' -f2)
            
            case $s in
                Discharging)
                    bat="-$n"
                ;;
                Charging)
                    bat="+$n"
                ;;
                Full)
                    bat="f$n"
                ;;
            esac
            ;;
        C*)
            clock="${line#?}" 
            ;;
        V*)
            vol="${line#?}" 
            ;;
    esac

    /usr/bin/xsetroot -name " VOL $vol | BAT $bat | $clock"
done < $pipe_path
