#!/bin/sh

pipe_path="/tmp/dwm_status.fifo"

[ ! -e $pipe_path ] && mkfifo $pipe_path

clock -f 'C%a %d %b %H:%M' -si 10 > $pipe_path &
battery -f 'B%3.0f:%d' -i 60 > $pipe_path &
temp -f 'T%3.0f' -i 15 > $pipe_path &
vol -f 'V%3.0f:%d'> $pipe_path &

while read -r line; do
    case $line in
        B*)
            fbat="${line#?}"
            n=$(echo $fbat | cut -d':' -f1)
            s=$(echo $fbat | cut -d':' -f2)
            
            case $s in
                1)
                    bat="-$n"
                ;;
                2)
                    bat="+$n"
                ;;
                3)
                    bat="f$n"
                ;;
            esac
            ;;
        C*)
            clock="${line#?}" 
            ;;
        V*)
            fvol="${line#?}" 
			v=$(echo $fvol | cut -d':' -f1)
			m=$(echo $fvol | cut -d':' -f2)

			case $m in
				0)
					vol="$v"
					;;
				1)
					vol="mte"
					;;
			esac
            ;;
        T*)
            temp="${line#?}"
            ;;
    esac

	xsetroot -name " VOL $vol | BAT $bat | TEMP $temp | $clock"
done < $pipe_path
