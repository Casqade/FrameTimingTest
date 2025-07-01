@echo off

set monitor_index= 0
set window_width= 2560
set window_height= 1440

:: 0 = windowed
:: 1 = windowed_borderless
:: 2 = fullscreen
:: 3 = fullscreen_borderless
set video_mode= 2

set vsync= 1
set sleep_between_ticks= 0
set tick_rate= 0
set poll_events= 1

:: 0 = raw
:: 1 = average
:: 2 = fixed (floor to tick)
:: 3 = fixed (round to tick)
:: 4 = fixed (interpolated)
set delta_time_mode= 4
set delta_time_buffer_size= 432

:: 0 = hidden
:: 1 = absolute
:: 2 = relative
set delta_time_graph_mode= 1

set subpixel_rendering= 1

set rect_width= 64
set rect_height= %window_height%

:: either horizontal or vertical speed MUST be zero
set rect_speed_x= 640
set rect_speed_y= 0


FrameTimingTest.exe %monitor_index% %window_width% %window_height% %video_mode% %vsync% %sleep_between_ticks% %tick_rate% %poll_events% %delta_time_mode% %delta_time_buffer_size% %delta_time_graph_mode% %subpixel_rendering% %rect_width% %rect_height% %rect_speed_x% %rect_speed_y%
