
Name: Your Name, Fall 2021
Section: T1/2A
Project: Final Project - Computer Assisted Gambling
Description: Track Cryptocurrency prices and report to the user when a price movement is predicted
Documentation Statement: 
I asked C3C Luke Ringe how to make cURL work with GMail, and he explained that SSL options needed to be used without looking at my code
I used the json-c library to parse json files (https://github.com/json-c/json-c/wiki), and used their documentation and tutorials to learn how to use it
I also used the cURL documentation to learn how to write data into memory (https://curl.se/libcurl/c/getinmemory.html)

==============================================================
UserManual/Instructions:
   Install libcurl (https://curl.se/libcurl/c/)
   Ensure that the config.json file is filled out properly:
	* Get an API Key from https://p.nomics.com/pricing
	* Set the mailing list file and put emails into it
	* Add an email username and password to send the email
	  * If using GMail, ensure that access is allowed for less secure sign-in
	* Amount of currencies and EMA Period sizes can be adjusted

   Build using gcc, making sure to link curl (-lcurl) and json-c (-ljson-c)
   Run and specify your config file as an argument
      The program will default to "config.json" if no argument is provided

==============================================================
Required Programming Skills (must have 5 of 6):
  -- Leave blank if the requirement isn't one that you implemented.

   1) Dynamic Memory Allocation
          What you did to meet this req: Any data that was received from the internet is dynamically allocated
              File: final.c
              Line#: 34

   2) Pointers
          What you did to meet this req: Passing Response type by reference
              File: final.c
              Line#: 100

   3) File I/O
          What you did to meet this req: Read emails from file
              File: final.c
              Line#: 77

   4) Structs
          What you did to meet this req: Used multiple types of structs to organize different data
              File: final_functs.c
              Line#: 9

   5) String Manipulation
          What you did to meet this req: Made the names of the cryptocurrencies fit into the API URL
              File: final.c
              Line#: 59

   6) Processing of Array Data
          What you did to meet this req: Keeping track of previous EMA values
              File: final.c
              Line#: 139

  Required Advanced Programming Skills (1 of the below):
   1) Recursion
          What you did to meet this req: N/A
              File: N/A
              Line#: N/A

   2) 2D Dynamic Memory
          What you did to meet this req: Mailing List is a dynamically allocated 2D array of chars (array of strings)
              File: final.c
              Line#: 74


==============================================================
List the requirements from your design document (indicate any changes
or omissions from stated shall requirements from initial design) 
also describe any functionality that isn't working.  You must also
list the percentage that was allocated from the design.  If you
are proposing a change include that as well as a justification
for the change in percentage.

      Requirements from Design:
      -- Indicate changes to requirment and/or percentage with
         an asterisk.  Put errors or nonfuctional implementation
         below the requirment it applies to.

          1) 25% - Get price data live from an API
          2) 15% - Notify the user and terminate if API cannot be reached
          3) 25% - Keep track of two sets of EMAs with different period sizes
	  4) 15% - Notify the user by email *(now using email to SMS) when a price movement is predicted
	  5) 10% - Do all of the above for multiple currencies specified by the user
          6) 10% - Get settings from a configuration file

==============================================================
  How many hours did you spend on this project: 18
  How many lines of code did you write in total: 485