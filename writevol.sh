#!/bin/sh

sink="alsa_output.pci-0000_00_1f.3.analog-stereo"
pipe_path="/tmp/dwm_status.fifo"

line=$(pactl list sinks | grep -n "Name: $sink" | cut -d : -f1)
volume=$(pactl list sinks | sed "$((line + 7))q;d" | awk '/\/ /{print $5}')

echo "V$volume" > $pipe_path
