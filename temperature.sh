echo "CPU: $(cat /sys/class/thermal/thermal_zone0/temp)"
echo "GPU: $(/opt/vc/bin/vcgencmd measure_temp)"
