OPaL - Language Specification
---

** PROGRAM STRUCTURE **

* Programs start with the first executable line.
* Statements end with a semicolon character.
* Operators follow the same precedence as standard math.
* Operations can be grouped in parentheses to override operator precedence.
* Single lines comments start with ‘//’ and end with the new-line character.
* Multi-line comments are enclosed between ‘/\*’ & ‘\*/’.
* Variable names should be unique across the program.

** DATA TYPES **

All variables are dynamically typed and hold integer values from -2147483648 to +2147483647.

** VARIABLES **
Variable names must be:
* Alpha-numeric & underscore character
* Begin with a character
* Initialized when declaraed

Eg:
* Define variable `x` & initialize to value `10`

```
x_10 = 10;
```

* Define variable y & initialize to 1 plus x, times 2

```
y = (1 + x) * 2;
```

** Supported Operators **

```
+-----------------------+------+
|      Arithematic      |      |
+-----------------------+------+
| Addition              | (+)  |
| Subtraction           | (-)  |
| Multiplication        | (*)  |
| Division              | (/)  |
| Modulus               | (%)  |
| Unary plus            | (+)  |
| Unary minus           | (-)  |
| Assignment            | (=)  |
+-----------------------+------+
|      Comparison       |      |
+-----------------------+------+
| Equal                 | (==) |
| Not equal             | (!=) |
| Less than             | (<)  |
| Greater than          | (>)  |
| Less than or equal    | (<=) |
| Greater than or equal | (>=) |
+-----------------------+------+
|      Logical          |      |
+-----------------------+------+
| And                   | (&&) |
| Not                   | (!)  |
| Or                    | (||) |
+-----------------------+------+
```

** CONTROL STRUCTURE **

The language supports the ‘if-else’ control structure.
Eg:

```
if (condition) {
  //statements;
} else {
  //statements;
}
```

** LOOP STRUCTURE **
The language supports the while loop structure.
Eg:

```
while (condition) {
  //statements;
}
```

**RESERVED KEYWORDS**

```
+---------+-------------------------------------------------------------------------------------+
| Keyword |                                     Description                                     |
+---------+-------------------------------------------------------------------------------------+
| if      | Control structure that checks for a condition given in parentheses                  |
| else    | Executes when condition in “if” control structure evaluates to False                |
| while   | Loop structure to execute code block until given condition evaluates to False       |
| print   | Print strings and variables inside parentheses                                      |
| input   | Print strings and variables inside parentheses, get user input & assign to variable |
+---------+-------------------------------------------------------------------------------------+
```

** MACROS **

```
+----------+------------------------------------+
|   Name   |            Description             |
+----------+------------------------------------+
| #include | Include contents of file specified |
+----------+------------------------------------+
```