# LesEnfantsCestMoi
wifi controlled baby carriage

spectacle *Les Enfants C'est Moi*  
**compagnie TourneBoulé** 2016

hardware :

-	raspberryPi3
-	Fraise pied USB
-	Fraise Versa 
-	pololu's VNH5019 12VDC motor H driver 
-	security relay for motor
-	MOSFET for 12V lamp

software :

-	main pd patch driver, hosted by the Pi, driving Fraise, and playing sounds. Basic text-based sequencer included. Network interface (tcp/424242).
-	fraise firmware driving the VNH and generating PWM for the lamp.
-	pure vanilla (pd >=46.7) client patch, hosted by control computer.


