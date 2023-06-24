# Rtos-Communicating-Tasks
#System Description :      
Four tasks communicate via a queue of fixed size as described below:
There are three sender tasks. Two has same priority and one has a higher priority. Each sender task sleeps for a RANDOM period of time Tsender and when it wakes up it sends a message to the queue containing
the string “Time is XYZ” where XYZ is current time in system ticks. If the queue is full, the sending operation fails and a counter counting total number of blocked messages is incremented. Upon successful
sending, a counter counting total number of transmitted messages is incremented. These counters should be kept per task. The sender task is then blocked for another random period again. The random period is drawn
from a uniform distribution as specified below.
