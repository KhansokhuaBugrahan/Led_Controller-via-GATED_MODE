# Led_Controller-via-GATED_MODE

Board: STM32F407G-DISC1                                     
IDE: STMCubeIDE                                           


We demonstrate how gated mode works in STM32F407G-DISC1 using TIMER2.                  
Also, you can observe how we utilized capture mode.                                 
                                                                       
As soon as we hold down the button, timer counter starts counting.                         
Once we release, we reset the counter.                      
                                                                             
X: The exact time when you release the button after first push

Example function in the project;                      
 While the button is hold down,                                        
 0 - 5 second only orange LED blinks,                           
 5 - 10 second only red LED blinks,                         
 10 - X second only blue LED blinks. 

 
![Alt text](https://github.com/KhansokhuaBugrahan/Led_Controller-via-GATED_MODE/blob/main/gated%20mode%20logic%20analyzer.PNG)









You can find the full explanation at the end of the main file as comment.
