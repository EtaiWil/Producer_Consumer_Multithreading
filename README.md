# Producer-Consumer Multithreading
## General Information
This program deals with the famous Producer-Consumer problem.<br>
The Producer process creates an item and adds it to the shared buffer. 
The Consumer process takes items out of the shared buffer and “consumes” them.

Certain conditions must be met by the Producer and the Consumer processes to have consistent data synchronization:

* The Producer process must not produce an item if the shared buffer is full.

* The Consumer process must not consume an item if the shared buffer is empty.

* Access to the shared buffer must be mutually exclusive; this means that at any given instance, only one process should be able to access the shared buffer and make changes to it.

The overall scenario we are simulating is that of news broadcasting.
Different types of stories are produced and the system sorts them and displays them to the public.

In the scenario, there are 4 types of active actors: 


### 1. Producer 

Each producer creates a number of strings in the following format:
```
“producer <i>  <type>  <j>” 
```
Where ‘i’ is the producer's ID, type is a random type it chooses which can be ‘SPORTS’, ‘NEWS’, WEATHER’,
and ‘j’ is the number of strings of type ‘type’ this producer has already produced.

### 2. Dispatcher
The Dispatcher continuously accepts messages from the Producers queues. 
It scans the Producers queue using a Round Robin algorithm.<br>
Each message is "sorted" by the Dispatcher and inserted to a one of the Dispatcher queues which includes strings of a single type.


### 3. Co-Editor
For each type of possible messages there is a Co-Editor that receives the message through the Dispatchers queue, "edits" it, 
and passes it to the screen manager via a single shared queue.

### 4. Screen-Manager
The Screen-Manager displays the strings it receives via the Co-Editors  queue to the screen.

## System Design
<br>
<br>

![systemDesign.png](images%2FsystemDesign.png)
<br>
<br>
Three producers communicate with the dispatcher via their Producer queues.
The Dispatcher communicates with the Co-Editors via three queues corresponding to the three types of messages. 
The Co- Editors communicate with the Screen-Manager via a single shared queue, and the Screen manager displays the systems output.


There is configuration file. with the folowing format:
```
PRODUCER 1
[number of products]
queue size = [size]


PRODUCER 2
[number of products]
queue size = [size]

…
…
…
PRODUCER n
[number of products]
queue size = [size]

Co-Editor queue size = [size]
```

For example:
```
1
30
5

2
25
3    

3
16
30

17
```
We have 3 producers  with co-editor size 17,  producer 1 makes 30 items and it's queue size is 5 etc..



## Setup
### Option 1 
1. Clone the repository:
```
https://github.com/EtaiWil/Producer_Consumer_Multithreading.git
```
2. Go to Linux terminal and write ``` make ```. 
3. Change the ```conf.txt``` file if you want and please stay attached to the format explained above.  
4. Write ``` ex3.out config.txt ``` and see the result on the screen. 

### Option 2 - With Ant
1. Clone the repository:
  ```
https://github.com/EtaiWil/Producer_Consumer_Multithreading.git
```
2. Enter in your IDE debug option the absolute as the directory you cloned the project in the working directory option. 

3. Enter in your IDE debug option ```conf.txt``` in the program arguments option.
4. Run the code.


## Built with

-C


## Author

**Etai Wilentzik**
- [Profile](https://github.com/EtaiWilentzik )
- [LinkedIn]( https://www.linkedin.com/in/etai-wilentzik/ "Welcome")

## Support 🤝

Contributions, issues, and feature requests are welcome!

Give a ⭐️ if you like this project!
