# Explore

The goal of this project was to create a GUI that emphasises on *exploration* and *experimentation*. It allows users to experience the ping-pong delay effect in a novel and interactive way. To achieve this goal the following features have been added:

- XY Pad as main interface control
- linking of parameters to x, y, as well as both diagonal axes
- option to switch direcetion (min/max value) of axes
- unlabelled parameters
- randomisation of parameter layout position and axis routing
- universe background and rocket icon controller to use metaphor of space exploration as theme for exploring the plugin
- hidden parameter controls (e.g. bpm synced delay time) only available when plugin is opened wiithout UI


The codebase for the delay effect is based on my **StereoDelay** plugin, with some minor additions.

Akash Murthy's [xy-pad-demo](https://github.com/Thrifleganger/xy-pad-demo) was used as a base for the XY Pad and has been further developed and customized to allow registration of 2 diagonal axes as well as switching the direction.

Jan Wilczek's [lowpass-highpass-filter](https://github.com/JanWilczek/lowpass-highpass-filter) was added but still needs to be implemented in a proper way.

This plugin and interface can be seen as a protoype on how an interface could be designed with a focus on expanding/emphasising experimentation of known audio effects. With further research more features and more audio effects could be added. 
