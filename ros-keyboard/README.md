# ros-keyboard
+ A good repository for keyboard commands when using ROS.
+ Forked from https://github.com/lrse/ros-keyboard

# = Description =

This node uses the SDL library to grab keypresses. To do so, it opens a window where input is received. This window needs to becurrently focused, otherwise this node will not receive any key presses. 

# = Topics =

~keydown and ~keyup (keyboard/Key)
