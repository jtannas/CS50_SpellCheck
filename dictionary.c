/* -----------------------------------------------------------------------------
*  -----------------------------------------------------------------------------
*   Desc.:      Implements a dictionary's functionality using Tries.
*   Purpose:    Pset5 of CS50X 2017
*   Author:     Joel Tannas
*   Date:       Jan 16, 2017
*   
*   Licensing Info:
*   None
* ------------------------------------------------------------------------------
* ----------------------------------------------------------------------------*/

// Includes:
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "dictionary.h"

// Prototypes:
int ASCII_to_index(const char chr);
bool Create_Word(const char *word);
unsigned int Delete_Trie(WordTrie *Trie);

// Global Variables (bad practice but needed due to the prototypes of the pset)
WordTrie *TrieTrunk;
unsigned int Word_Count;

// Implementation:

/* -------------------------------------------------------------------------
*   Desc.:      Returns true if word is in dictionary else false.
*   Purpose:    Spellchecking - Seeing if a given word exists
*   Author:     Joel Tannas
*   Date:       Jan 16, 2017
*
*   Bugs, Limitations, and Other Notes:
*   - Per instrcutions, assumes only apoostrophes or alpha chars in word
* ------------------------------------------------------------------------*/
bool check(const char *word)
{
    // Initialize a pointer for the current WordTrie
    WordTrie *Trie = TrieTrunk;

    // Iterate down through the layers of Tries, checking each char of *word
    for (int iter = 0; ; iter++)
    {
        // If at the word termination, return whether the word exists
        // Else, update Trie to the next layer (if it exists)
        if (word[iter] == '\0')
        {
            return Trie->EOW;
        }
        else
        {
            // Find the Next Trie associated with the current char
            int Index = ASCII_to_index(word[iter]);
            if (Trie->Next[Index] != NULL)
            {
                Trie = Trie->Next[Index];
            }
            else
            {
                return false;
            }
        }
    }
    
    // This point is reached if LENGTH iterations is exceeded
    return false;
}

/* -------------------------------------------------------------------------
*   Desc.:      Loads dictionary into memory. 
                Returns true if successful, else false.
*   Purpose:    Loading the dictionary into memory for spellchecking
*   Author:     Joel Tannas
*   Date:       Jan 16, 2017
*
*   Bugs, Limitations, and Other Notes:
*   - Learned about calloc from
*  https://www.tutorialspoint.com/c_standard_library/c_function_calloc.htm
* ------------------------------------------------------------------------*/
bool load(const char *dictionary)
{
    // Open the file
    FILE* ptr = fopen(dictionary,"r");
    if (!ptr)
    {
        return false;
    }
    
    // Create the base tree
    TrieTrunk = calloc(1,sizeof(WordTrie));
    
    // Create a pointer to hold a word
    char word[LENGTH + 1];
    
    // Create a counter of errors
    unsigned int errors = 0;
    
    // Scan the File line by line
    while(fscanf(ptr, "%s", word) != EOF)
    {
        Create_Word(word) ? Word_Count++: errors++;
    }
    
    // Close Up
    fclose(ptr);
    return (errors == 0);
}

/* -------------------------------------------------------------------------
*   Desc.:      Returns number of words in dictionary if loaded
                    else 0 if not yet loaded.
*   Purpose:    Reporting back the size of the dictionary
*   Author:     Joel Tannas
*   Date:       Jan 16, 2017
*
*   Bugs, Limitations, and Other Notes:
*   - 
* ------------------------------------------------------------------------*/
unsigned int size(void)
{
    // Tell the user the Word_Count of the dictionary
    return Word_Count;
}

/* -------------------------------------------------------------------------
*   Desc.:      Unloads dictionary from memory.
                Returns true if successful else false.
*   Purpose:    Freeing the memory used by the dictionary
*   Author:     Joel Tannas
*   Date:       Jan 16, 2017
*
*   Bugs, Limitations, and Other Notes:
*   - Most of the work is done via recursion in another function
* ------------------------------------------------------------------------*/
bool unload(void)
{
    // Pass the address of the TrieTrunk for deletion using recursion
    return (Delete_Trie(TrieTrunk) == 0);
}

/* -------------------------------------------------------------------------
*   Desc.:      Returns the array index of an ASCII letter
*   Purpose:    Accessing the correct Trie index
*   Author:     Joel Tannas
*   Date:       Jan 16, 2017
*
*   Bugs, Limitations, and Other Notes:
*   - Per instructions, assumes only alpha characters or apostrophes
* ------------------------------------------------------------------------*/
int ASCII_to_index(const char chr)
{
    // Return an index based on the input character
    // 0 to 25 for letters
    // 26 for apostrophes
    return (chr == '\'') ? 26 : tolower(chr) - 'a';
}

/* -------------------------------------------------------------------------
*   Desc.:      Adds a word to the Dictionary, return bool Success/Failure
*   Purpose:    Growing the Dictionary Trie
*   Author:     Joel Tannas
*   Date:       Jan 17, 2017
*
*   Bugs, Limitations, and Other Notes:
*   - Per instructions, assumes only alpha or apostrophes in word
* ------------------------------------------------------------------------*/
bool Create_Word(const char *word)
{
    // Initialize a Trie pointer and character counter
    WordTrie *Trie = TrieTrunk;
    
    // Iterate through the characters in the word, creating tries until EOW
    for(int chr = 0;; chr++)
    {
        // If at the end of the word, create an EOW flag. Else, next layer.
        if (word[chr] != '\0')
        {
            // Find the array index of the character
            int Index = ASCII_to_index(word[chr]);
            
            // If no next Trie exists there, create it
            if (!Trie->Next[Index])
            {
                Trie->Next[Index] = calloc(1,sizeof(WordTrie));
            }
            
            // Point to the next level of Trie
            Trie = Trie->Next[Index];
        }
        else
        {
            // Set the EOW flag to true and return true
            Trie->EOW = true;
            return true;
        }
    }
}

/* -------------------------------------------------------------------------
*   Desc.:      Returns the # of failed deletions of Trie and sub-Tries
*   Purpose:    Delete a Trie and it's sub-Tries
*   Author:     Joel Tannas
*   Date:       Jan 17, 2017
*
*   Bugs, Limitations, and Other Notes:
*   - Uses recursion to iterate through all sub-Tries
*   - Only frees memory if no failures so that no links are lost
* ------------------------------------------------------------------------*/
unsigned int Delete_Trie(WordTrie *Trie)
{
    // Make a counter for the number of failures
    unsigned int failures = 0;
    
    // Use recursion to delete all the sub-Tries of the current Trie
    for (int Index = 0; Index < LETTERS; Index++)
    {
        // If there are sub-Tries, delete them
        if (Trie->Next[Index] != NULL)
        {
            failures += Delete_Trie(Trie->Next[Index]);
        }
    }
    
    // Delete the current trie if no sub-Tries are left
    if (failures == 0)
    {
        free(Trie);
    }
    
    // Return the number of failures
    return failures;
}