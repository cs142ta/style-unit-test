# Style Unit Test
The style unit test is essentially a very basic c++ linter that integrates well with the zyBooks online learning environment. This way, students can get immediate feedback on some parts of their style.

## Checks
The test checks for:
- Proper indentation, allowing for 2, 3, or 4 space indents so long as the student is consistent
- Bracing on all blocks - the CS 142 Style Guide requires bracing on all block statements, even if they are single-line
- Consistent bracing - students may use "one true brace style" or Allman style bracing, but if they are mixed the test fails
- Global variables - which are marked as bad
- Single letter variable names outside of for loops - which are warned about, but do not fail the test

## Integrating with ZyBooks
To use this test, all `.h` files must be uploaded as "Additional files" to the zylab you are adding the test to. Then you must create a new "Test bench" of type "unit test". An example of the code for the test is provided in `exampleTest.cpp` - the only thing that should be changed is the vector of filenames to be checked.

## How it Works (in brief)
The test makes use of two different tokenizers.
1. A 'lossless' tokenizer that ignores whitespace and retains the names of identifiers. This is used for detecting single letter variables.
2. A 'lossy' tokenizer that loses a large amount of detail in the code but retains whitespace detail. This is used in all other checks.

After tokenizing, each check is run by passing the appropriate tokens through an enhanced state machine. Most of these machines are documented in graphic format [here](https://drive.google.com/drive/folders/1UkU5Op0FgpOCOJpJaeeD4Q_gheGBQt8t).