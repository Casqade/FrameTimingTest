#!/bin/bash

monitor_index=0
window_width=1600
window_height=900

# 0 = windowed
# 1 = windowed_borderless
# 2 = fullscreen
# 3 = fullscreen_borderless
video_mode=0

vsync=1
sleep_between_ticks=1
tick_rate=0
poll_events=1

# 0 = raw
# 1 = average
# 2 = fixed (floor to tick)
# 3 = fixed (round to tick)
# 4 = fixed (interpolated)
delta_time_mode=0
delta_time_buffer_size=300

# 0 = hidden
# 1 = absolute
# 2 = relative
delta_time_graph_mode=1

subpixel_rendering=1

rect_width=64
rect_height=$window_height

# either horizontal or vertical speed MUST be zero
rect_speed_x=640
rect_speed_y=0


./FrameTimingTest $monitor_index $window_width $window_height $video_mode $vsync $sleep_between_ticks $tick_rate $poll_events $delta_time_mode $delta_time_buffer_size $delta_time_graph_mode $subpixel_rendering $rect_width $rect_height $rect_speed_x $rect_speed_y
