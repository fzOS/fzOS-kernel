# ZCrystal Window System
### Version 1
## Structure
+ the GUI window system mainly seperate into two parts, render system and window manage system
## Normal Operation
### A. Window Setup and Draw
```
software 
|-(request a window)-> window manage system 
                       |-(allocate a window)-> render system 
                                               |-(draw screen buffer)-> GOP
                       |
                       |-(assign a window and properity) -> self list
                       |-(allocate buffer for this window) -> kmalloc
                       |
|<-( buffer properity)-|
|
|-(draw on allocated buffer)-> buffer
|
|-(cal frame update)-> window manage system 
                       |-(call buffer update)-> render system 
                                                |-(draw screen buffer)-> GOP
```
### B. Focus/Layer System
```
window manage system
|-(send input to focused window)-> software

```