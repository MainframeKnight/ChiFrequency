# ChiFrequency
The **goals** of this project are:
- to design and implement a tool for statistical analysis of ciphertexts;
- to give some examples of its usage in cryptography (in the current state the only cipher supported is the Caesar cipher);
- to design a client-server model using GO and JS to create a user-friendly web-experience.

The main statistical test used here is the **chi-squared test** for bigrams (combinations of pairs of letters are used as groups) with added support for inclusion of spaces in ciphertext.
Because it is quite rare for an enciphered message to contain information about spaces, *a small change* is applied should the ciphertext not contain any spaces: 
the approximate number of spaces in English is known to be 1 space for 4.7 letters, so the number of bigrams of the rarest type is reduced by one for each modelled space 
(as in the string "amhungry" the absent space creates a highly unlikely bigram "mh", which gets in the way of easy statistical analysis).
