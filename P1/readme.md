Group 24

Members:\
Alex Chan\
Emily Huang\
Jayson Yan\
Matthew Godin

Questions:\
1. What are the compilation macros and what do they do. When should we enable/disable them?\
The compilation macros are:\
DEBUG_0: Used to show/hide debug messages\
Should be enabled when debugging code to see debug messages and disabled when actually running.\
K_MSG_ENV: Used to define meta-data fields for the message envelope\
Should be always enabled. If not enabled, program will not work since we access these meta-data fields.\
_DEBUG_HOTKEYS: Used to enable the debug hotkeys "!", "@", "#"\
Should be enabled when you want to have debug hotkeys enabled and disabled when you don't need them.

2. How to run your customized commands\
There are no customized commands to run.

3. Which keys are the hotkeys and what are their functionalities\
The hotkeys are:\
"!": Prints all processes in the ready queue\
"@": Prints all processes blocked on memory\
"#": Prints all processes blocked on messages

4. How to start your test cases to see the hot key output\
The test cases will start as soon as you start running and you can see the hotkey outputs in UART0 console after inputting the hotkey as long as _DEBUG_HOTKEYS is defined.

5. Do you have any extra non-required features you implemented? If yes, what are they and how to use them?\
There are no non-required features implemented.
