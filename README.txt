Instructions for Executing the Caesar and BreakCaesar Scripts

1. Granting Permissions to the Shell Scripts:

    Before executing the shell scripts, you need to grant them execution permissions. You can do this using the following command:

    ```
    chmod +x caesar
    chmod +x breakcaesar
    ```

    This command will grant execution permissions to the `caesar` and `breakcaesar` shell scripts.

2. Executing the Shell Scripts:

    Once you have granted execution permissions to the shell scripts, follow these steps to execute them:

    a. Open a terminal.
    b. Navigate to the directory where the shell scripts are located.
    c. To execute the Caesar script, use the following command:

        ```
        ./caesar
        ```

	Examples:
        
        ```
        ./caesar 23 'THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG'
        ```
        ```
        echo 'THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG' | ./caesar 23
        ```


    d. To execute the BreakCaesar script, use the following command:

        ```
        ./breakcaesar
        ```

      	Examples:
      	
      	```
        ./breakcaesar 23 'QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD'
        ```
        ```
        echo 'QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD' | ./breakcaesar
        ```
