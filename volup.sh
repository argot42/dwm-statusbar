#!/bin/sh

sink="alsa_output.pci-0000_00_1f.3.analog-stereo"

pactl -- set-sink-volume "$sink" "+5%"

writevol.sh
